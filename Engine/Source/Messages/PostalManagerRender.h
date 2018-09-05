#pragma once
#include "CoreMessage.h"
#include <vector>
#include "../Window/window.h"


class PostalManagerRender
{
private:
	std::vector<CoreMessage> m_incommingMessages;
	std::vector<CoreMessage> m_outgoingMessages;
public:
	PostalManagerRender();
	~PostalManagerRender();
	void Init();
	void AddMessage(CoreMessage &message);
	bool PostMessages();
	void AddReply(CoreMessage & message);
	std::vector<CoreMessage>& retrieveMessages();
};