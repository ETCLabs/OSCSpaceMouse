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

#ifndef LOG_H
#define LOG_H

#include <QString>
#include <QMutex>
#include <set>

////////////////////////////////////////////////////////////////////////////////

class Log
{
public:

	class Client
	{
	public:
		virtual void LogClientPreDispatch() {}
		virtual void LogClientRecvMessage(const QString &text, const qint64 &timestamp) = 0;
		virtual void LogClientPostDispatch() {}
	};

	Log();
	virtual ~Log();

	virtual void Clear();
	virtual void Subscribe(Client *pClient);
	virtual void Unsubscribe(Client *pClient);
	virtual void Tick();
	virtual void AddMessage(const QString &text);

	static void InitSingleton();
	static void ShutdownSingleton();
	static Log* Instance() {return sm_Instance;}

protected:
	struct sMessage
	{
		qint64		timestamp;
		QString		text;
		sMessage	*next;
	};

	typedef std::set<Client*> CLIENTS;

	QMutex		m_QMutex;
	sMessage	*m_Head;
	sMessage	*m_Tail;

	QMutex		m_ClientMutex;
	CLIENTS		m_Clients;

	static Log*	sm_Instance;
};

////////////////////////////////////////////////////////////////////////////////

#define LOG(x) Log::Instance()->AddMessage(x)

////////////////////////////////////////////////////////////////////////////////

#endif
