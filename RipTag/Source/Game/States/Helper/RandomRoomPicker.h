#pragma once
#include <vector>
class RandomRoomPicker
{
	struct RoomPicker
	{
		bool isRandom = false;
		int roomNumber = 0;
		int seedNumber = 0;
	};

private:

	

public:
	static std::vector<RoomPicker> RoomPick(int seed);
};

inline std::vector<RandomRoomPicker::RoomPicker> RandomRoomPicker::RoomPick(int seed)
{
	std::vector<RoomPicker> temp;
	RoomPicker t;
	t.roomNumber = 0;
	t.isRandom = false;
	t.seedNumber = -1;
	temp.push_back(t);
	return temp;
}


