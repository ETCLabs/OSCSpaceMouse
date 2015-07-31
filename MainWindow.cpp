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

#include "MainWindow.h"
#include "Globals.h"
#include "Converter.h"
#include "Options.h"
#include "SpaceMousePacket.h"
#include <QListWidget>
#include <QGridLayout>
#include <QDateTime>
#include <QTimer>
#include <QPushButton>
#include <QLineEdit>

#ifdef WIN32
#include <Windows.h>
#include "resource.h"
#endif

////////////////////////////////////////////////////////////////////////////////

#define MAX_LOG_ITEMS	100

////////////////////////////////////////////////////////////////////////////////

MainWindow::MainWindow(QWidget* parent/* =0 */, Qt::WindowFlags f/* =0 */)
	: QWidget(parent, f)
	, m_SiHandle(0)
	, m_SiGrabbed(false)
	, m_FirstRun(true)
{
	Log::Instance()->Subscribe(this);

	LOG( QString("Version %1").arg(APP_VERSION) );

	QTimer *timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(onTick()));
	timer->start(60);

	QGridLayout *layout = new QGridLayout(this);

	m_Log = new QListWidget(this);
	QFont fnt("Monospace");
	fnt.setStyleHint(QFont::TypeWriter);
	m_Log->setFont(fnt);
	layout->addWidget(m_Log, 0, 0, 1, 5);

	QPushButton *button = new QPushButton("OSC Config", this);
	connect(button, SIGNAL(clicked(bool)), this, SLOT(onShowOptionsClicked(bool)));
	layout->addWidget(button, 1, 0);

	button = new QPushButton("SpaceMouse Config", this);
	connect(button, SIGNAL(clicked(bool)), this, SLOT(onShowSpaceMouseOptionsClicked(bool)));
	layout->addWidget(button, 1, 1);

	button = new QPushButton("Grab", this);
	connect(button, SIGNAL(clicked(bool)), this, SLOT(onGrabClicked(bool)));
	layout->addWidget(button, 1, 2);

	button = new QPushButton("Clear Log", this);
	connect(button, SIGNAL(clicked(bool)), this, SLOT(onClearLogClicked(bool)));
	layout->addWidget(button, 1, 3);

	m_SendPort = new QLineEdit(this);
	m_SendPort->setReadOnly(true);
	m_SendPort->setAlignment(Qt::AlignCenter);
	m_SendPort->setEnabled(false);
	layout->addWidget(m_SendPort, 1, 4);

	m_Converter = new Converter();

#ifdef WIN32
	HICON hIcon = static_cast<HICON>( LoadImage(GetModuleHandle(0),MAKEINTRESOURCE(IDI_ICON1),IMAGE_ICON,128,128,LR_LOADTRANSPARENT) );
	if( hIcon )
	{
		setWindowIcon( QIcon(QPixmap::fromWinHICON(hIcon)) );
		DestroyIcon(hIcon);
	}
#endif

	resize(500, sizeHint().height());
}

////////////////////////////////////////////////////////////////////////////////

MainWindow::~MainWindow()
{
	delete m_Converter;
	Log::Instance()->Unsubscribe(this);
	Grab(false);
	if( m_SiHandle )
	{
		if( m_SiGrabbed )
		{
			SiReleaseDevice(m_SiHandle);
			m_SiGrabbed = false;
		}

		SiClose(m_SiHandle);
		m_SiHandle = 0;

		SiTerminate();
	}
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::Go()
{
	Options options;
	if( options.Load() )
		m_FirstRun = false;
	options.Save();
	m_Converter->Go(options);

	m_SendPort->setText( QString("Port: %1").arg(options.sendPort) );

	show();
	QTimer::singleShot(1, this, SLOT(onInit()));
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::InitSpaceMouse()
{
	SpwRetVal result = SiInitialize();
	if(result == SPW_NO_ERROR)
	{
		SiOpenData siOpenData;
		SiOpenWinInit(&siOpenData, winId());
		m_SiHandle = SiOpen("OSCSpaceMouse", SI_ANY_DEVICE, SI_NO_MASK, SI_EVENT, &siOpenData);
		if(m_SiHandle == 0)
		{
			LOG("SiOpen failed");
			SiTerminate();
		}
		else if( m_FirstRun )
		{
			// set all buttons for "Application Use"
			for(SPWuint32 i=1; i<=Options::MAX_BUTTONS; i++)
				SiSyncSetButtonAssignmentAbsolute(m_SiHandle, i, 0);

			Grab(true);
		}
	}
	else
		LOG( QString("SiInitialize failed with error %1").arg(result) );
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::Grab(bool b)
{
	if(m_SiHandle != 0)
	{
		if( m_SiGrabbed )
		{
			SiReleaseDevice(m_SiHandle);
			m_SiGrabbed = false;
		}

		if( b )
		{
			SpwRetVal result = SiGrabDevice(m_SiHandle,/*exclusive*/true);
			if(result == SPW_NO_ERROR)
			{
				m_SiGrabbed = true;
			}
			else
			{
				LOG( QString("SiGrabDevice(exclusive) failed with error %1").arg(result) );

				result = SiGrabDevice(m_SiHandle,/*exclusive*/false);
				if(result == SPW_NO_ERROR)
					m_SiGrabbed = true;
				else
					LOG( QString("SiGrabDevice(non-exclusive) failed with error %1").arg(result) );
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::onInit()
{
	InitSpaceMouse();
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::onTick()
{
	Log::Instance()->Tick();
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::onClearLogClicked(bool /*checked*/)
{
	m_Log->clear();
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::onShowOptionsClicked(bool /*checked*/)
{
	Options::Open();
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::onShowSpaceMouseOptionsClicked(bool /*checked*/)
{
	if(m_SiHandle != 0)
		SiSetUiMode(m_SiHandle, SI_UI_ALL_CONTROLS);
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::onGrabClicked(bool /*checked*/)
{
	Grab(true);
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::LogClientRecvMessage(const QString &text, const qint64 &timestamp)
{
	QString itemText = QString("[%1]  %2")
		.arg(QDateTime::fromMSecsSinceEpoch(timestamp).toString("ddd hh:mm:ss:zzz"))
		.arg(text);

	m_Log->addItem(itemText);
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::LogClientPostDispatch()
{
	m_Log->setUpdatesEnabled(false);
	while(m_Log->count() > MAX_LOG_ITEMS)
	{
		QListWidgetItem *item = m_Log->takeItem(0);
		if( item )
			delete item;
	}
	m_Log->setUpdatesEnabled(true);

	m_Log->setCurrentRow(m_Log->count() - 1);
}

////////////////////////////////////////////////////////////////////////////////

bool MainWindow::winEvent(MSG *message, long *result)
{
	if(m_SiHandle!=0 && message)
	{
		SiSpwEvent siEvent;
		SiGetEventData siEventData;
		SiGetEventWinInit(&siEventData, message->message, message->wParam, message->lParam);
		if(SiGetEvent(m_SiHandle,SI_AVERAGE_EVENTS,&siEventData,&siEvent) == SI_IS_EVENT)
		{
			switch( siEvent.type )
			{
				case SI_BUTTON_EVENT:
					HandleSiButtonEvent(siEvent);
					break;

				case SI_MOTION_EVENT:
					HandleSiMotionEvent(siEvent);
					break;

				case SI_ZERO_EVENT:
					HandleSiZeroEvent(siEvent);
					break;
			}

			if( result )
				*result = 0;

			return true;
		}
	}

	return QWidget::winEvent(message, result);
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::closeEvent(QCloseEvent *event)
{
	m_Converter->Stop();
	QWidget::closeEvent(event);
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::HandleSiButtonEvent(SiSpwEvent &siEvent)
{
	int buttonIndex = SiButtonPressed( &siEvent );
	if(buttonIndex == SI_NO_BUTTON)
	{
		buttonIndex = SiButtonReleased( &siEvent );
		if(buttonIndex != SI_NO_BUTTON)
		{
			buttonIndex--;
			if(buttonIndex>=0 && buttonIndex<Options::MAX_BUTTONS)
			{
				LOG( QString("Button %1 Released").arg(buttonIndex) );

				sSpaceMousePacket *packet = new sSpaceMousePacket;
				packet->type = SPACE_MOUSE_PACKET_BUTTON;
				packet->index = buttonIndex;
				packet->value = 0;
				m_Converter->SendPacket(reinterpret_cast<unsigned char*>(packet), sizeof(sSpaceMousePacket));
			}
		}
	}
	else
	{
		buttonIndex--;
		if(buttonIndex>=0 && buttonIndex<Options::MAX_BUTTONS)
		{
			LOG( QString("Button %1 Pressed").arg(buttonIndex) );

			sSpaceMousePacket *packet = new sSpaceMousePacket;
			packet->type = SPACE_MOUSE_PACKET_BUTTON;
			packet->index = buttonIndex;
			packet->value = 1;
			m_Converter->SendPacket(reinterpret_cast<unsigned char*>(packet), sizeof(sSpaceMousePacket));
		}
	}
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::HandleSiMotionEvent(SiSpwEvent &siEvent)
{
	sMotionData motionData;
	motionData.translate[0] = siEvent.u.spwData.mData[SI_TX];
	motionData.translate[1] = siEvent.u.spwData.mData[SI_TY];
	motionData.translate[2] = siEvent.u.spwData.mData[SI_TZ];
	motionData.rotate[0] = siEvent.u.spwData.mData[SI_RX];
	motionData.rotate[1] = siEvent.u.spwData.mData[SI_RY];
	motionData.rotate[2] = siEvent.u.spwData.mData[SI_RZ];
	HandleMotion(motionData);
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::HandleSiZeroEvent(SiSpwEvent& /*siEvent*/)
{
	sMotionData motionData;
	memset(&motionData, 0, sizeof(motionData));
	HandleMotion(motionData);
}

////////////////////////////////////////////////////////////////////////////////

void MainWindow::HandleMotion(const sMotionData &motionData)
{
	if(m_MotionData.translate[0] != motionData.translate[0])
	{
		m_MotionData.translate[0] = motionData.translate[0];
		LOG( QString("Tx=%1").arg(m_MotionData.translate[0]) );

		sSpaceMousePacket *packet = new sSpaceMousePacket;
		packet->type = SPACE_MOUSE_PACKET_TRANSLATE_X;
		packet->value = m_MotionData.translate[0];
		m_Converter->SendPacket(reinterpret_cast<unsigned char*>(packet), sizeof(sSpaceMousePacket));
	}

	if(m_MotionData.translate[1] != motionData.translate[1])
	{
		m_MotionData.translate[1] = motionData.translate[1];
		LOG( QString("Ty=%1").arg(m_MotionData.translate[1]) );

		sSpaceMousePacket *packet = new sSpaceMousePacket;
		packet->type = SPACE_MOUSE_PACKET_TRANSLATE_Y;
		packet->value = m_MotionData.translate[1];
		m_Converter->SendPacket(reinterpret_cast<unsigned char*>(packet), sizeof(sSpaceMousePacket));
	}

	if(m_MotionData.translate[2] != motionData.translate[2])
	{
		m_MotionData.translate[2] = motionData.translate[2];
		LOG( QString("Tz=%1").arg(m_MotionData.translate[2]) );

		sSpaceMousePacket *packet = new sSpaceMousePacket;
		packet->type = SPACE_MOUSE_PACKET_TRANSLATE_Z;
		packet->value = m_MotionData.translate[2];
		m_Converter->SendPacket(reinterpret_cast<unsigned char*>(packet), sizeof(sSpaceMousePacket));
	}

	if(m_MotionData.rotate[0] != motionData.rotate[0])
	{
		m_MotionData.rotate[0] = motionData.rotate[0];
		LOG( QString("Rx=%1").arg(m_MotionData.rotate[0]) );

		sSpaceMousePacket *packet = new sSpaceMousePacket;
		packet->type = SPACE_MOUSE_PACKET_ROTATE_X;
		packet->value = m_MotionData.rotate[0];
		m_Converter->SendPacket(reinterpret_cast<unsigned char*>(packet), sizeof(sSpaceMousePacket));
	}

	if(m_MotionData.rotate[1] != motionData.rotate[1])
	{
		m_MotionData.rotate[1] = motionData.rotate[1];
		LOG( QString("Ry=%1").arg(m_MotionData.rotate[1]) );

		sSpaceMousePacket *packet = new sSpaceMousePacket;
		packet->type = SPACE_MOUSE_PACKET_ROTATE_Y;
		packet->value = m_MotionData.rotate[1];
		m_Converter->SendPacket(reinterpret_cast<unsigned char*>(packet), sizeof(sSpaceMousePacket));
	}

	if(m_MotionData.rotate[2] != motionData.rotate[2])
	{
		m_MotionData.rotate[2] = motionData.rotate[2];
		LOG( QString("Rz=%1").arg(m_MotionData.rotate[2]) );

		sSpaceMousePacket *packet = new sSpaceMousePacket;
		packet->type = SPACE_MOUSE_PACKET_ROTATE_Z;
		packet->value = m_MotionData.rotate[2];
		m_Converter->SendPacket(reinterpret_cast<unsigned char*>(packet), sizeof(sSpaceMousePacket));
	}

	m_MotionData.initialized = true; 
}

////////////////////////////////////////////////////////////////////////////////
