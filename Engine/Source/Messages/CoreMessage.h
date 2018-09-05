#pragma once

class CoreMessage
{
public:
	CoreMessage(unsigned int ID = 0);
	virtual ~CoreMessage();
	unsigned int MessageID;
};