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

#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#ifndef LOG_H
#include "Log.h"
#endif

#include <windows.h>

#ifndef SPWMACRO_H
#include <spwmacro.h>
#endif

#ifndef _SI_H_
#include <si.h>
#endif

#ifndef SIAPP_H
#include <siapp.h>
#endif

#include <QWidget>

class Converter;
class QLineEdit;
class QListWidget;

////////////////////////////////////////////////////////////////////////////////

class MainWindow
	: public QWidget
	, private Log::Client
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent=0, Qt::WindowFlags f=0);
	virtual ~MainWindow();

	virtual void Go();

private slots:
	void onInit();
	void onTick();
	void onClearLogClicked(bool checked);
	void onShowOptionsClicked(bool checked);
	void onShowSpaceMouseOptionsClicked(bool checked);
	void onGrabClicked(bool checked);

protected:
	struct sMotionData
	{
		sMotionData()
			: initialized(false)
		{}
		bool	initialized;
		long	translate[3];
		long	rotate[3];
	};

	QListWidget	*m_Log;
	QLineEdit	*m_SendPort;
	SiHdl		m_SiHandle;
	bool		m_SiGrabbed;
	sMotionData	m_MotionData;
	Converter	*m_Converter;
	bool		m_FirstRun;

	virtual void InitSpaceMouse();
	virtual void LogClientRecvMessage(const QString &text, const qint64 &timestamp);
	virtual void LogClientPostDispatch();
	virtual bool winEvent(MSG *message, long *result);
	virtual void closeEvent(QCloseEvent *event);
	virtual void HandleSiButtonEvent(SiSpwEvent &siEvent);
	virtual void HandleSiMotionEvent(SiSpwEvent &siEvent);
	virtual void HandleSiZeroEvent(SiSpwEvent &siEvent);
	virtual void HandleMotion(const sMotionData &motionData);
	virtual void Grab(bool b);
};

////////////////////////////////////////////////////////////////////////////////

#endif
