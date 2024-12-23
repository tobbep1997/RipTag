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
	t.roomNumber = 0;
	t.isRandom = false;
	t.seedNumber = seed;
	temp.push_back(t);


	t.roomNumber = 2;
	t.isRandom = false;
	t.seedNumber = seed;
	temp.push_back(t);

	t.roomNumber = -1;
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

	t.roomNumber = 4;
	t.isRandom = false;
	t.seedNumber = seed;
	temp.push_back(t);

	for (int i = 0; i < 5; i++)
	{
		t.roomNumber = -1;
		t.isRandom = false;
		t.seedNumber = seed;
		temp.push_back(t);
	}

	t.roomNumber = 4;
	t.isRandom = false;
	t.seedNumber = seed;
	temp.push_back(t);


	return temp;
}