#include "PostalManagerRender.h"

PostalManagerRender::PostalManagerRender()
{
}

PostalManagerRender::~PostalManagerRender()
{
}

void PostalManagerRender::AddMessage(CoreMessage & message)
{
	m_incommingMessages.push_back(message);
}

bool PostalManagerRender::PostMessages()
{
	/*
	TODO:
	THREADING
	switch(m_incommingMessages)
	send messages to right component
	*/
	m_incommingMessages.clear();
	return true;
}

void PostalManagerRender::AddReply(CoreMessage & message)
{
	/*
	TODO
	MUTEX
	
	*/

	m_outgoingMessages.push_back(message);
}

std::vector<CoreMessage>& PostalManagerRender::retrieveMessages()
{
	std::vector<CoreMessage> replies = m_outgoingMessages;
	m_outgoingMessages.clear();
	return replies;
}
