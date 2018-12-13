#include "RipTagPCH.h"
#include "RandomRoomGrid.h"

RandomRoomGrid::RandomRoomGrid(int width, int depth)
{
	m_width = width;
	m_depth = depth;
	m_gridSize = m_width * m_depth;
	m_roomGrid = new int[m_gridSize];
	m_rooms = new RandomRoom[m_gridSize];

	int randPlaceWinRoom = rand() % m_width;
	randPlaceWinRoom *= m_depth;
	int randPlaceStartRoom = (rand() % m_width) * (m_width - 1);

	for (int i = 0; i < m_gridSize; i++)
	{
		m_roomGrid[i] = 2;
	}
	m_roomGrid[randPlaceWinRoom] = 3;
	m_rooms[randPlaceWinRoom].type = WIN_ROOM;
	m_roomGrid[randPlaceStartRoom] = 4;
	m_rooms[randPlaceStartRoom].type = START_ROOM;
}

RandomRoomGrid::~RandomRoomGrid()
{
	if (m_roomGrid)
		delete [] m_roomGrid;
	m_roomGrid = nullptr;
	if (m_rooms)
		delete [] m_rooms;
	m_rooms = nullptr;
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

	nrOfBigRooms = 4;

	_insertRooms(nrOfBigRooms);
	_connectRooms();
	_checkConnections();
	DrawEachRoom();
 }

DirectX::XMINT2 RandomRoomGrid::GetSize() const
{
	return DirectX::XMINT2(m_width, m_depth);
}

void RandomRoomGrid::Print()
{
	for (int i = 0; i < m_depth; i++)
	{
		for (int j = 0; j < m_width; j++)
			std::cout << m_roomGrid[j + i * m_width] << " ";
		std::cout << "\n";
	}
}

void RandomRoomGrid::DrawConnections()
{
	for (int i = 0; i < m_depth; i++)
	{
		for (int j = 0; j < m_width; j++)
			std::cout << " " << m_rooms[j + i * m_width].north << "    ";
		std::cout << "\n";
		for (int j = 0; j < m_width; j++)
			std::cout << m_rooms[j + i * m_width].west << " " << m_rooms[j + i * m_width].east << "   ";
		std::cout << "\n";
		for (int j = 0; j < m_width; j++)
			std::cout << " " << m_rooms[j + i * m_width].south << "    ";
		std::cout << "\n\n";
	}
}

void RandomRoomGrid::DrawEachRoom()
{
	for (int i = 0; i < m_width; i++)
	{
		for (int j = 0; j < m_width; j++)
		{
			std::cout << m_rooms[i + j * m_width].type << " ";
		}
		std::cout << std::endl;
	}
	std::cout << "\n";
}

void RandomRoomGrid::_insertRooms(int count)
{
	for (int i = 0; i < count; i++)
	{
		int randomPosition = rand() % m_gridSize;
		int otherPosition = 0;
		int pickRoom = rand() % 2;

		if (m_roomGrid[randomPosition] == 2)
		{
			if (pickRoom == 0)
			{
				// Division by Width gives y value
				int y = randomPosition / m_width;
				int firstRowIndex = y * m_width;
				int lastRowIndex = y * m_width + (m_width - 1);
				int left = randomPosition - 1;
				int right = randomPosition + 1;

				if (m_roomGrid[firstRowIndex] != 2 && m_roomGrid[lastRowIndex] != 2)
				{
					i--;
					continue;
				}

				if (randomPosition == firstRowIndex)
				{
					if (m_roomGrid[right] != 2)
					{
						i--;
						continue;
					}
					otherPosition = right;
				}
				else if (randomPosition == lastRowIndex)
				{
					if (m_roomGrid[left] != 2)
					{
						i--;
						continue;
					}
					otherPosition = left;
				}
				else
				{
					if (left >= firstRowIndex && m_roomGrid[left] != 2)
						otherPosition = right;
					else if (right <= lastRowIndex && m_roomGrid[right] != 2)
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
				int y = randomPosition / m_width;
				int previousColumnIndex = randomPosition - m_width;
				int nextColumnIndex = randomPosition + m_width;

				if (previousColumnIndex > 0 && nextColumnIndex < m_gridSize)
				{
					if (m_roomGrid[previousColumnIndex] != 2 && m_roomGrid[nextColumnIndex] != 2)
					{
						i--;
						continue;
					}
				}
				else if (previousColumnIndex < 0)
				{
					if (m_roomGrid[nextColumnIndex] != 2)
					{
						i--;
						continue;
					}
				}
				else if (nextColumnIndex >= m_gridSize)
				{
					if (m_roomGrid[previousColumnIndex] != 2)
					{
						i--;
						continue;
					}
				}

				if (previousColumnIndex < 0)
					otherPosition = nextColumnIndex;
				else if (nextColumnIndex >= m_gridSize)
					otherPosition = previousColumnIndex;
				else
				{
					if (m_roomGrid[previousColumnIndex] != 2)
						otherPosition = nextColumnIndex;
					else if (m_roomGrid[nextColumnIndex] != 2)
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
			m_roomGrid[randomPosition] = pickRoom;
			m_roomGrid[otherPosition] = pickRoom;
			m_oddRooms.push_back(RoomLocations(RoomType(pickRoom), randomPosition, otherPosition));
		}
		else
			i--;
	}
}

void RandomRoomGrid::_connectRooms()
{
	for (int i = 0; i < m_oddRooms.size(); i++)
	{
		int start = m_oddRooms.at(i).startIndex;
		int end = m_oddRooms.at(i).endIndex;
		_createRoomConnection(start, end);
		m_rooms[start].type = m_oddRooms.at(i).type;
		m_rooms[start].pairedWith = end;
		m_rooms[end].type = m_oddRooms.at(i).type;
		m_rooms[end].pairedWith = start;
	}

	for (int i = 0; i < m_depth; i++)
	{
		for (int j = 0; j < m_width; j++)
		{
			bool validDirections[4];
			for (int i = 0; i < 4; i++)
				validDirections[i] = false;

			int directionIndex[4];
			for (int i = 0; i < 4; i++)
				directionIndex[i] = -1;

			int index = j + i * m_width;
			int northIndex = index - m_width;
			int eastIndex = index + 1;
			int southIndex = index + m_width;
			int westIndex = index - 1;
			int doorCount = 0;

			if (northIndex >= 0)
			{
				if (!m_rooms[northIndex].south)
				{
					validDirections[0] = true;
					directionIndex[0] = northIndex;
				}
				else
					doorCount++;
			}
			if (eastIndex < i * m_width + m_width)
			{
				if (!m_rooms[eastIndex].west)
				{
					validDirections[1] = true;
					directionIndex[1] = eastIndex;
				}
				else
					doorCount++;
			}
			if (southIndex < m_gridSize)
			{
				if (!m_rooms[southIndex].north)
				{
					validDirections[2] = true;
					directionIndex[2] = southIndex;
				}
				else
					doorCount++;
			}
			if (westIndex >= i * m_width)
			{
				if (!m_rooms[westIndex].east)
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
	for (int i = 0; i < m_depth; i++)
	{
		for (int j = 0; j < m_width; j++)
		{
			int index = j + i * m_width;
			if (!visited[index])
			{
				bool alreadyConnected[4];
				for (int i = 0; i < 4; i++)
					alreadyConnected[i] = false;
				int directionIndexes[4];
				// North
				directionIndexes[0] = index - m_width;
				// East
				directionIndexes[1] = index + 1;
				// South
				directionIndexes[2] = index + m_width;
				// West
				directionIndexes[3] = index - 1;

				if (directionIndexes[0] >= 0)
					if (visited[directionIndexes[0]])
						alreadyConnected[0] = true;
				if (directionIndexes[1] < i * m_width + m_width)
					if (visited[directionIndexes[1]])
						alreadyConnected[1] = true;
				if (directionIndexes[2] < m_gridSize)
					if (visited[directionIndexes[2]])
						alreadyConnected[2] = true;
				if (directionIndexes[3] >= i * m_width)
					if (visited[directionIndexes[3]])
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
	bool * visited = new bool[m_gridSize];

	for (int i = 0; i < m_gridSize; i++)
		visited[i] = false;

	_followConnection(visited, 0);

	_forcePath(visited);

	delete [] visited;
	visited = nullptr;
}

void RandomRoomGrid::_followConnection(bool * visited, int roomIndex)
{
	visited[roomIndex] = true;

	if (m_rooms[roomIndex].north && !visited[m_rooms[roomIndex].leadsToRoom[0]])
		_followConnection(visited, m_rooms[roomIndex].leadsToRoom[0]);
	if (m_rooms[roomIndex].east && !visited[m_rooms[roomIndex].leadsToRoom[1]])
		_followConnection(visited, m_rooms[roomIndex].leadsToRoom[1]);
	if (m_rooms[roomIndex].south && !visited[m_rooms[roomIndex].leadsToRoom[2]])
		_followConnection(visited, m_rooms[roomIndex].leadsToRoom[2]);
	if (m_rooms[roomIndex].west && !visited[m_rooms[roomIndex].leadsToRoom[3]])
		_followConnection(visited, m_rooms[roomIndex].leadsToRoom[3]);
}

void RandomRoomGrid::_createRoomConnection(int start, int end)
{
	int direction = start - end;
	if (direction > 0)
	{
		if (direction == 1)
		{
			m_rooms[start].west = true;
			m_rooms[start].leadsToRoom[3] = end;
			m_rooms[end].east = true;
			m_rooms[end].leadsToRoom[1] = start;

		}
		else
		{
			m_rooms[start].north = true;
			m_rooms[start].leadsToRoom[0] = end;
			m_rooms[end].south = true;
			m_rooms[end].leadsToRoom[2] = start;
		}
	}
	else
	{
		if (direction == -1)
		{
			m_rooms[start].east = true;
			m_rooms[start].leadsToRoom[1] = end;
			m_rooms[end].west = true;
			m_rooms[end].leadsToRoom[3] = start;
		}
		else
		{
			m_rooms[start].south = true;
			m_rooms[start].leadsToRoom[2] = end;
			m_rooms[end].north = true;
			m_rooms[end].leadsToRoom[0] = start;
		}
	}
}