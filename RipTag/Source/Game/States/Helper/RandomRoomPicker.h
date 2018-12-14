#pragma once
#include <vector>
class RandomRoomPicker
{
	

private:

	

public:
	struct RoomPicker
	{
		bool isRandom = false;
		int roomNumber = 0;
		int seedNumber = 0;
	};
	static std::vector<RoomPicker> RoomPick(int seed);
};

inline std::vector<RandomRoomPicker::RoomPicker> RandomRoomPicker::RoomPick(int seed)
{
	srand(seed);
	//StartRoom
	std::vector<RoomPicker> temp;
	RoomPicker t;
	t.roomNumber = 5;
	t.isRandom = false;
	t.seedNumber = seed;
	temp.push_back(t);


	t.roomNumber = 2;
	t.isRandom = false;
	t.seedNumber = seed;
	temp.push_back(t);

	t.roomNumber = 3;
	t.isRandom = false;
	t.seedNumber = seed;
	temp.push_back(t);

	t.roomNumber = -1;
	t.isRandom = false;
	t.seedNumber = seed;
	temp.push_back(t);

	//for (unsigned int i = 0; i < 10000; ++i)
	//{
	//	t.roomNumber = (rand() % 3) - 1;
	//	while (t.roomNumber == 0)
	//	{
	//		t.roomNumber = (rand() % 3) - 1;
	//	}
	//	t.isRandom = false;
	//	t.seedNumber = seed;
	//	temp.push_back(t); 
	//}


	return temp;
}


