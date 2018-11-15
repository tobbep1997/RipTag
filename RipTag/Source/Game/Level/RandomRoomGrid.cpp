#include "RipTagPCH.h"
#include "RandomRoomGrid.h"

void RandomRoomGrid::_insertRooms(int count)
{
	for (int i = 0; i < count; i++)
	{
		int randomPosition = rand() % GRID_SIZE;
		int otherPosition = 0;
		int pickRoom = rand() % 2;

		if (roomGrid[randomPosition] == 2)
		{
			if (pickRoom == 0)
			{
				// Division by Width gives y value
				int y = randomPosition / width;
				int firstRowIndex = y * width;
				int lastRowIndex = y * width + (width - 1);
				int left = randomPosition - 1;
				int right = randomPosition + 1;

				if (roomGrid[firstRowIndex] != 2 && roomGrid[lastRowIndex] != 2)
				{
					i--;
					continue;
				}

				if (randomPosition == firstRowIndex)
				{
					if (roomGrid[right] != 2)
					{
						i--;
						continue;
					}
					otherPosition = right;
				}
				else if (randomPosition == lastRowIndex)
				{
					if (roomGrid[left] != 2)
					{
						i--;
						continue;
					}
					otherPosition = left;
				}
				else
				{
					if (left >= firstRowIndex && roomGrid[left] != 2)
						otherPosition = right;
					else if (right <= lastRowIndex && roomGrid[right] != 2)
						otherPosition = left;
					else
					{
						int direction = rand() % 2;
						if (direction == 0)
							otherPosition = left;
						else
							otherPosition = right;
					}
				}
			}
			else
			{
				// Division by Width gives y value
				int y = randomPosition / width;
				int previousColumnIndex = randomPosition - width;
				int nextColumnIndex = randomPosition + width;

				if (previousColumnIndex > 0 && nextColumnIndex < GRID_SIZE)
				{
					if (roomGrid[previousColumnIndex] != 2 && roomGrid[nextColumnIndex] != 2)
					{
						i--;
						continue;
					}
				}
				else if (previousColumnIndex < 0)
				{
					if (roomGrid[nextColumnIndex] != 2)
					{
						i--;
						continue;
					}
				}
				else if (nextColumnIndex >= GRID_SIZE)
				{
					if (roomGrid[previousColumnIndex] != 2)
					{
						i--;
						continue;
					}
				}

				if (previousColumnIndex < 0)
					otherPosition = nextColumnIndex;
				else if (nextColumnIndex >= GRID_SIZE)
					otherPosition = previousColumnIndex;
				else
				{
					if (roomGrid[previousColumnIndex] != 2)
						otherPosition = nextColumnIndex;
					else if (roomGrid[nextColumnIndex] != 2)
						otherPosition = previousColumnIndex;
					else
					{
						int direction = rand() % 2;
						if (direction == 0)
							otherPosition = previousColumnIndex;
						else
							otherPosition = nextColumnIndex;
					}
				}
			}
			roomGrid[randomPosition] = pickRoom;
			roomGrid[otherPosition] = pickRoom;
			oddRooms.push_back(RoomLocations(RoomType(pickRoom), randomPosition, otherPosition));
		}
		else
			i--;
	}
}

void RandomRoomGrid::GenerateRoomLayout()
{
	int threshold = rand() % 101;
	int nrOfBigRooms = 1;

	if (threshold > 40 && threshold <= 60)
	{
		nrOfBigRooms = 2;
	}
	else if (threshold > 60 && threshold <= 80)
	{
		nrOfBigRooms = 3;
	}
	else if (threshold > 80)
	{
		nrOfBigRooms = 4;
	}

	_insertRooms(nrOfBigRooms);
	_connectRooms();
	_checkConnections();
}

void RandomRoomGrid::Print()
{
	for (int i = 0; i < depth; i++)
	{
		for (int j = 0; j < width; j++)
			std::cout << roomGrid[j + i * width] << " ";
		std::cout << "\n";
	}
}

void RandomRoomGrid::DrawConnections()
{
	for (int i = 0; i < depth; i++)
	{
		for (int j = 0; j < width; j++)
			std::cout << " " << rooms[j + i * width].north << "    ";
		std::cout << "\n";
		for (int j = 0; j < width; j++)
			std::cout << rooms[j + i * width].west << " " << rooms[j + i * width].east << "   ";
		std::cout << "\n";
		for (int j = 0; j < width; j++)
			std::cout << " " << rooms[j + i * width].south << "    ";
		std::cout << "\n\n";
	}
}

void RandomRoomGrid::_connectRooms()
{
	for (int i = 0; i < oddRooms.size(); i++)
	{
		_createRoomConnection(oddRooms.at(i).startIndex, oddRooms.at(i).endIndex);
		rooms[oddRooms.at(i).startIndex].type = oddRooms.at(i).type;
		rooms[oddRooms.at(i).endIndex].type = oddRooms.at(i).type;
	}

	for (int i = 0; i < depth; i++)
	{
		for (int j = 0; j < width; j++)
		{
			bool validDirections[4];
			memset(validDirections, false, sizeof(validDirections));
			int directionIndex[4];
			for (int i = 0; i < 4; i++)
				directionIndex[i] = -1;

			int index = j + i * width;
			int northIndex = index - width;
			int eastIndex = index + 1;
			int southIndex = index + width;
			int westIndex = index - 1;
			int doorCount = 0;

			if (northIndex >= 0)
			{
				if (!rooms[northIndex].south)
				{
					validDirections[0] = true;
					directionIndex[0] = northIndex;
				}
				else
					doorCount++;
			}
			if (eastIndex < i * width + width)
			{
				if (!rooms[eastIndex].west)
				{
					validDirections[1] = true;
					directionIndex[1] = eastIndex;
				}
				else
					doorCount++;
			}
			if (southIndex < GRID_SIZE)
			{
				if (!rooms[southIndex].north)
				{
					validDirections[2] = true;
					directionIndex[2] = southIndex;
				}
				else
					doorCount++;
			}
			if (westIndex >= i * width)
			{
				if (!rooms[westIndex].east)
				{
					validDirections[3] = true;
					directionIndex[3] = westIndex;
				}
				else
					doorCount++;
			}

			_generateDoors(validDirections, doorCount, index, directionIndex);
		}
	}
}

void RandomRoomGrid::_generateDoors(bool * validDirections, int doorCount, int roomIndex, int * directionIndex)
{
	int placeDoor = rand() % 2;
	if (validDirections[0])
		if (placeDoor == 0)
		{
			_createRoomConnection(roomIndex, directionIndex[0]);
			placeDoor = rand() % 2;
			doorCount++;
		}
	if (validDirections[1])
		if (placeDoor == 0)
		{
			_createRoomConnection(roomIndex, directionIndex[1]);
			placeDoor = rand() % 2;
			doorCount++;
		}
	if (validDirections[2])
		if (placeDoor == 0)
		{
			_createRoomConnection(roomIndex, directionIndex[2]);
			placeDoor = rand() % 2;
			doorCount++;
		}
	if (validDirections[3])
		if (placeDoor == 0)
		{
			_createRoomConnection(roomIndex, directionIndex[3]);
			doorCount++;
		}

	if (doorCount == 0)
	{
		bool validDirection = false;
		int randomRoomDoor = 0;
		while (!validDirection)
		{
			randomRoomDoor = rand() % 4;
			if (directionIndex[randomRoomDoor] != -1)
				validDirection = true;
		}
		switch (randomRoomDoor)
		{
		case 0:
			_createRoomConnection(roomIndex, directionIndex[0]);
			break;
		case 1:
			_createRoomConnection(roomIndex, directionIndex[1]);
			break;
		case 2:
			_createRoomConnection(roomIndex, directionIndex[2]);
			break;
		default:
			_createRoomConnection(roomIndex, directionIndex[3]);
		}
	}
}

void RandomRoomGrid::_forcePath(bool * visited)
{
	for (int i = 0; i < depth; i++)
	{
		for (int j = 0; j < width; j++)
		{
			int index = j + i * width;
			if (!visited[index])
			{
				bool alreadyConnected[4];
				int directionIndexes[4];
				// North
				directionIndexes[0] = index - width;
				// East
				directionIndexes[1] = index + 1;
				// South
				directionIndexes[2] = index + width;
				// West
				directionIndexes[3] = index - 1;

				if (directionIndexes[0] >= 0)
					if (rooms[directionIndexes[0]].south)
						alreadyConnected[0] = true;
				if (directionIndexes[1] < i * width + width)
					if (rooms[directionIndexes[1]].west)
						alreadyConnected[1] = true;
				if (directionIndexes[2] < GRID_SIZE)
					if (rooms[directionIndexes[2]].north)
						alreadyConnected[2] = true;
				if (directionIndexes[3] >= i * width)
					if (rooms[directionIndexes[3]].east)
						alreadyConnected[3] = true;

				// Check already connected paths to see where the new door will be
				bool validDirection = false;
				int doorDirectionIndex = 0;
				while (!validDirection)
				{
					doorDirectionIndex = rand() % 4;
					if (alreadyConnected[doorDirectionIndex])
					{
						_createRoomConnection(index, directionIndexes[doorDirectionIndex]);
						_followConnection(visited, index);
						validDirection = true;
					}
				}
			}
		}
	}
}

void RandomRoomGrid::_checkConnections()
{
	bool * visited = new bool[GRID_SIZE];
	memset(visited, false, GRID_SIZE);

	_followConnection(visited, 0);
	_forcePath(visited);

	for (int i = 0; i < depth; i++)
	{
		for (int j = 0; j < width; j++)
			std::cout << " " << visited[j + i * width] << " ";
		std::cout << "\n\n";
	}

	delete visited;
}

void RandomRoomGrid::_followConnection(bool * visited, int roomIndex)
{
	visited[roomIndex] = true;

	if (rooms[roomIndex].north && !visited[rooms[roomIndex].leadsToRoom[0]])
		_followConnection(visited, rooms[roomIndex].leadsToRoom[0]);
	if (rooms[roomIndex].east && !visited[rooms[roomIndex].leadsToRoom[1]])
		_followConnection(visited, rooms[roomIndex].leadsToRoom[1]);
	if (rooms[roomIndex].south && !visited[rooms[roomIndex].leadsToRoom[2]])
		_followConnection(visited, rooms[roomIndex].leadsToRoom[2]);
	if (rooms[roomIndex].west && !visited[rooms[roomIndex].leadsToRoom[3]])
		_followConnection(visited, rooms[roomIndex].leadsToRoom[3]);
}

void RandomRoomGrid::_createRoomConnection(int start, int end)
{
	int direction = start - end;
	if (direction > 0)
	{
		if (direction == 1)
		{
			rooms[start].west = true;
			rooms[start].leadsToRoom[3] = end;
			rooms[end].east = true;
			rooms[end].leadsToRoom[1] = start;

		}
		else
		{
			rooms[start].north = true;
			rooms[start].leadsToRoom[0] = end;
			rooms[end].south = true;
			rooms[end].leadsToRoom[2] = start;
		}
	}
	else
	{
		if (direction == -1)
		{
			rooms[start].east = true;
			rooms[start].leadsToRoom[1] = end;
			rooms[end].west = true;
			rooms[end].leadsToRoom[3] = start;
		}
		else
		{
			rooms[start].south = true;
			rooms[start].leadsToRoom[2] = end;
			rooms[end].north = true;
			rooms[end].leadsToRoom[0] = start;
		}
	}
}