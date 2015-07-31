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

#ifndef CONVERTER_H
#define CONVERTER_H

#ifndef OPTIONS_H
#include "Options.h"
#endif

#include <QMutex>
#include <QThread>

class QUdpSocket;

////////////////////////////////////////////////////////////////////////////////

class UDPSendThread
	: public QThread
{
public:
	UDPSendThread(const Options &options);
	virtual ~UDPSendThread();

	virtual void Clear();
	virtual void AddPacket(unsigned char *data, const qint64 &size);

protected:
	struct sPacket
	{
		qint64			size;
		unsigned char	*data;
		sPacket			*next;
	};

	bool	m_Run;
	Options	m_Options;
	QMutex	m_Mutex;
	sPacket	*m_Head;
	sPacket	*m_Tail;

	virtual void run();

	virtual void OSCDataForSpaceMousePacket(unsigned int type, int index, std::string &path, float &value);
};

////////////////////////////////////////////////////////////////////////////////

class UDPRecvThread
	: public QThread
{
public:
	UDPRecvThread(const Options &options);
	virtual ~UDPRecvThread();

protected:
	bool	m_Run;
	Options	m_Options;

	virtual void run();
};

////////////////////////////////////////////////////////////////////////////////

class Converter
{
public:
	Converter();
	virtual ~Converter();

	virtual void Go(const Options &options);
	virtual void Stop();
	virtual bool IsRunning();
	virtual void SendPacket(unsigned char *data, const qint64 &size);

protected:
	UDPSendThread	*m_SendThread;
	UDPRecvThread	*m_RecvThread;
};

////////////////////////////////////////////////////////////////////////////////

#endif
