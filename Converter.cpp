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

#include "Converter.h"
#include "Log.h"
#include "Options.h"
#include "SpaceMousePacket.h"
#include <OSCParser.h>
#include <QUdpSocket>

////////////////////////////////////////////////////////////////////////////////

UDPSendThread::UDPSendThread(const Options &options)
	: m_Run(true)
	, m_Options(options)
	, m_Head(0)
	, m_Tail(0)
{
}

////////////////////////////////////////////////////////////////////////////////

UDPSendThread::~UDPSendThread()
{
	m_Run = false;
	wait();
	Clear();
}

////////////////////////////////////////////////////////////////////////////////

void UDPSendThread::Clear()
{
	m_Mutex.lock();
	while( m_Head )
	{
		sPacket *p = m_Head;
		m_Head = p->next;
		delete[] p->data;
		delete p;
	}
	m_Head = m_Tail = 0;
	m_Mutex.unlock();
}

////////////////////////////////////////////////////////////////////////////////

void UDPSendThread::AddPacket(unsigned char *data, const qint64 &size)
{
	sPacket *packet = new sPacket;
	packet->size = size;
	packet->data = data;
	packet->next = 0;

	m_Mutex.lock();
	if( m_Tail )
	{
		m_Tail->next = packet;
		m_Tail = packet;
	}
	else
		m_Head = m_Tail = packet;
	m_Mutex.unlock();
}

////////////////////////////////////////////////////////////////////////////////

void UDPSendThread::run()
{
	LOG( QString("Creating udp send socket on port %1").arg(m_Options.sendPort) );
	QUdpSocket *udp = new QUdpSocket();
	std::string oscPath;

	while( m_Run )
	{
		m_Mutex.lock();
		sPacket *head = m_Head;
		m_Head = m_Tail = 0;
		m_Mutex.unlock();

		while( head )
		{
			if( m_Run )
			{
				// convert to OSC
				if(head->data && head->size==sizeof(sSpaceMousePacket))
				{
					sSpaceMousePacket *smp = reinterpret_cast<sSpaceMousePacket*>( head->data );
					
					float value = smp->value;
					OSCDataForSpaceMousePacket(smp->type, smp->index, oscPath, value);

					if( !oscPath.empty() )
					{
						char *oscData = 0;
						size_t oscSize = 0;
						OSCPacketWriter packet(oscPath);
						packet.AddFloat32(value);
						oscData = packet.Create(oscSize);
						if( oscData )
						{
							if(oscSize != 0)
							{
								if(udp->writeDatagram(oscData,oscSize,QHostAddress::Broadcast,m_Options.sendPort) != oscSize)
									LOG( QString("writeDatagram failed with error %1").arg(udp->errorString()) );
							}

							delete[] oscData;
						}
					}
				}
			}

			sPacket *p = head;
			head = p->next;
			delete[] p->data;
			delete p;
		}

		msleep(1);
	}

	delete udp;
	LOG("Ending udp send thread");
}


////////////////////////////////////////////////////////////////////////////////

void UDPSendThread::OSCDataForSpaceMousePacket(unsigned int type, int index, std::string &path, float &value)
{
	switch( type )
	{
		case SPACE_MOUSE_PACKET_BUTTON:
			{
				if(index>=0 && index<Options::MAX_BUTTONS)
				{
					path = m_Options.buttons[index].path;
					value = Options::GetOutputValue(m_Options.buttons[index], value);
				}
			}
			break;

		case SPACE_MOUSE_PACKET_TRANSLATE_X:
			path = m_Options.translateX.path;
			value = Options::GetOutputValue(m_Options.translateX, value);
			break;

		case SPACE_MOUSE_PACKET_TRANSLATE_Y:
			path = m_Options.translateY.path;
			value = Options::GetOutputValue(m_Options.translateY, value);
			break;

		case SPACE_MOUSE_PACKET_TRANSLATE_Z:
			path = m_Options.translateZ.path;
			value = Options::GetOutputValue(m_Options.translateZ, value);
			break;

		case SPACE_MOUSE_PACKET_ROTATE_X:
			path = m_Options.rotateX.path;
			value = Options::GetOutputValue(m_Options.rotateX, value);
			break;

		case SPACE_MOUSE_PACKET_ROTATE_Y:
			path = m_Options.rotateY.path;
			value = Options::GetOutputValue(m_Options.rotateY, value);
			break;

		case SPACE_MOUSE_PACKET_ROTATE_Z:
			path = m_Options.rotateZ.path;
			value = Options::GetOutputValue(m_Options.rotateZ, value);
			break;
	}
}

////////////////////////////////////////////////////////////////////////////////

UDPRecvThread::UDPRecvThread(const Options &options)
	: m_Run(true)
	, m_Options(options)
{
}

////////////////////////////////////////////////////////////////////////////////

UDPRecvThread::~UDPRecvThread()
{
	m_Run = false;
	wait();
}

////////////////////////////////////////////////////////////////////////////////

void UDPRecvThread::run()
{
	LOG( QString("Creating udp listen socket on port %1").arg(m_Options.listenPort) );
	QUdpSocket *udp = new QUdpSocket();
	if( udp->bind(m_Options.listenPort,QUdpSocket::DefaultForPlatform) )
	{
		while( m_Run )
		{
			while(m_Run && udp->hasPendingDatagrams())
			{
				qint64 size = udp->pendingDatagramSize();
				if(size != 0)
				{
					unsigned char *data = new unsigned char[size];
					if(udp->readDatagram(reinterpret_cast<char*>(data),size) == size)
					{
						// TODO
					}
					else
						LOG( QString("readDatagram failed with error %1").arg(udp->errorString()) );
				}
			}

			msleep(1);
		}
	}
	else
		LOG( QString("Bind for udp listen socket failed with error %1").arg(udp->errorString()) );

	delete udp;

	LOG("Ending udp receive thread");
}

////////////////////////////////////////////////////////////////////////////////

Converter::Converter()
	: m_SendThread(0)
	, m_RecvThread(0)
{
}

////////////////////////////////////////////////////////////////////////////////

Converter::~Converter()
{
	Stop();
}

////////////////////////////////////////////////////////////////////////////////

void Converter::Go(const Options &options)
{
	Stop();

	LOG("Starting udp send thread");
	m_SendThread = new UDPSendThread(options);
	m_SendThread->start();

	LOG("Starting udp receive thread");
	m_RecvThread = new UDPRecvThread(options);
	m_RecvThread->start();
}

////////////////////////////////////////////////////////////////////////////////

void Converter::Stop()
{
	if( m_RecvThread )
	{
		LOG("Stopping udp receive thread");
		delete m_RecvThread;
		m_RecvThread = 0;
	}

	if( m_SendThread )
	{
		LOG("Stopping udp send thread");
		delete m_SendThread;
		m_SendThread = 0;
	}
}

////////////////////////////////////////////////////////////////////////////////

bool Converter::IsRunning()
{
	return (m_SendThread && m_SendThread->isRunning() &&
			m_RecvThread && m_RecvThread->isRunning() );
}

////////////////////////////////////////////////////////////////////////////////

void Converter::SendPacket(unsigned char *data, const qint64 &size)
{
	if(m_SendThread && m_SendThread->isRunning())
		m_SendThread->AddPacket(data, size);
	else if( data )
		delete[] data;
}

////////////////////////////////////////////////////////////////////////////////
