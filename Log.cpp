// Copyright (c) 2015 Electronic Theatre Controls, Inc., http://www.etcconnect.com
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include "Log.h"
#include <QDateTime>

////////////////////////////////////////////////////////////////////////////////

Log* Log::sm_Instance = 0;

////////////////////////////////////////////////////////////////////////////////

Log::Log()
	: m_Head(0)
	, m_Tail(0)
{
}

////////////////////////////////////////////////////////////////////////////////

Log::~Log()
{
	Clear();
}

////////////////////////////////////////////////////////////////////////////////

void Log::Clear()
{
	m_QMutex.lock();
	while( m_Head )
	{
		sMessage *p = m_Head;
		m_Head = p->next;
		delete p;
	}
	m_Head = m_Tail = 0;
	m_QMutex.unlock();
}

////////////////////////////////////////////////////////////////////////////////

void Log::Subscribe(Client *pClient)
{
	if( pClient )
	{
		m_ClientMutex.lock();
		m_Clients.insert(pClient);
		m_ClientMutex.unlock();
	}
}

////////////////////////////////////////////////////////////////////////////////

void Log::Unsubscribe(Client *pClient)
{
	if( pClient )
	{
		m_ClientMutex.lock();
		m_Clients.erase(pClient);
		m_ClientMutex.unlock();
	}
}

////////////////////////////////////////////////////////////////////////////////

void Log::Tick()
{
	m_QMutex.lock();
	sMessage *head = m_Head;
	m_Head = m_Tail = 0;
	m_QMutex.unlock();

	if( head )
	{
		m_ClientMutex.lock();

		for(CLIENTS::const_iterator i=m_Clients.begin(); i!=m_Clients.end(); i++)
			(*i)->LogClientPreDispatch();

		while( head )
		{
			for(CLIENTS::const_iterator i=m_Clients.begin(); i!=m_Clients.end(); i++)
				(*i)->LogClientRecvMessage(head->text, head->timestamp);

			sMessage *p = head;
			head = p->next;
			delete p;
		}

		for(CLIENTS::const_iterator i=m_Clients.begin(); i!=m_Clients.end(); i++)
			(*i)->LogClientPostDispatch();

		m_ClientMutex.unlock();
	}
}

////////////////////////////////////////////////////////////////////////////////

void Log::AddMessage(const QString &text)
{
	sMessage *msg = new sMessage;
	msg->timestamp = QDateTime::currentMSecsSinceEpoch();
	msg->text = text;
	msg->next = 0;

	m_QMutex.lock();
	if( m_Tail )
	{
		m_Tail->next = msg;
		m_Tail = msg;
	}
	else
		m_Head = m_Tail = msg;
	m_QMutex.unlock();
}

////////////////////////////////////////////////////////////////////////////////

void Log::InitSingleton()
{
	if( !sm_Instance )
		sm_Instance = new Log;
}

////////////////////////////////////////////////////////////////////////////////

void Log::ShutdownSingleton()
{
	if( sm_Instance )
	{
		delete sm_Instance;
		sm_Instance = 0;
	}
}

////////////////////////////////////////////////////////////////////////////////
