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
#include "Log.h"
#include <QApplication>
#include <QPlastiqueStyle>

////////////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	app.setStyle(new QPlastiqueStyle());

	QPalette pal;
	pal.setColor(QPalette::Window, QColor(40,40,40));
	pal.setColor(QPalette::WindowText, QColor(119,167,255));
	pal.setColor(QPalette::Base, QColor(60,60,60));
	pal.setColor(QPalette::Button, QColor(60,60,60));
	pal.setColor(QPalette::Light, pal.color(QPalette::Button).lighter(20));
	pal.setColor(QPalette::Midlight, pal.color(QPalette::Button).lighter(10));
	pal.setColor(QPalette::Dark, pal.color(QPalette::Button).darker(20));
	pal.setColor(QPalette::Mid, pal.color(QPalette::Button).darker(10));
	pal.setColor(QPalette::Text, QColor(119,167,255));
	pal.setColor(QPalette::Highlight, QColor(80,80,80));
	pal.setColor(QPalette::HighlightedText, QColor(255,142,51));
	pal.setColor(QPalette::ButtonText, QColor(119,167,255));
	app.setPalette(pal);

	Log::InitSingleton();

	MainWindow *mainWindow = new MainWindow();
	mainWindow->Go();

	int result = app.exec();
	delete mainWindow;

	Log::ShutdownSingleton();

	return result;
}

////////////////////////////////////////////////////////////////////////////////
