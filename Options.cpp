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

#include "Options.h"
#include <QDir>
#include <QSettings>
#include <QCoreApplication>
#include <QDesktopServices>
#include <QUrl>

////////////////////////////////////////////////////////////////////////////////

Options::Options()
	: listenPort(8001)
	, sendPort(8000)
{
	buttons[0].path = "/eos/key/Last";
	buttons[0].inputRange.rangeMin = 0.0f;
	buttons[0].inputRange.rangeMax = 1.0f;
	buttons[0].outputRange.rangeMin = 0.0f;
	buttons[0].outputRange.rangeMax = 1.0f;

	buttons[1].path = "/eos/key/Next";
	buttons[1].inputRange.rangeMin = 0.0f;
	buttons[1].inputRange.rangeMax = 1.0f;
	buttons[1].outputRange.rangeMin = 0.0f;
	buttons[1].outputRange.rangeMax = 1.0f;

	translateY.path = "/eos/switch/fine/edge";
	translateY.inputRange.rangeMin = -2062.0f;
	translateY.inputRange.rangeMax = 2062.0f;
	translateY.outputRange.rangeMin = -0.25f;
	translateY.outputRange.rangeMax = 0.25f;

	rotateX.path = "/eos/switch/fine/tilt";
	rotateX.inputRange.rangeMin = -10000.0f;
	rotateX.inputRange.rangeMax = 10000.0f;
	rotateX.outputRange.rangeMin = -15.0f;
	rotateX.outputRange.rangeMax = 15.0f;

	rotateZ.path = "/eos/switch/fine/pan";
	rotateZ.inputRange.rangeMin = -10000.0f;
	rotateZ.inputRange.rangeMax = 10000.0f;
	rotateZ.outputRange.rangeMin = -15.0f;
	rotateZ.outputRange.rangeMax = 15.0f;

	rotateY.path = "/eos/switch/level";
	rotateY.inputRange.rangeMin = -10307.0f;
	rotateY.inputRange.rangeMax = 10307.0f;
	rotateY.outputRange.rangeMin = 2.0f;
	rotateY.outputRange.rangeMax = -2.0f;
}

////////////////////////////////////////////////////////////////////////////////

bool Options::Load()
{
	QString path;
	GetConfigFilePath(path);
	bool loadedExisting = QFileInfo(path).exists();
	QSettings settings(path, QSettings::IniFormat);

	listenPort = static_cast<quint16>( settings.value("OSC/ListenPort",static_cast<int>(listenPort)).toInt() );
	sendPort = static_cast<quint16>( settings.value("OSC/SendPort",static_cast<int>(sendPort)).toInt() );

	for(int i=0; i<MAX_BUTTONS; i++)
	{
		sSpaceMouseData &button = buttons[i];
		button.path = settings.value(QString("OSC/button%1/path").arg(i),button.path.c_str()).toString().toUtf8().constData();
		button.inputRange.rangeMin = settings.value(QString("OSC/button%1/inputMin").arg(i),button.inputRange.rangeMin).toFloat();
		button.inputRange.rangeMax = settings.value(QString("OSC/button%1/inputMax").arg(i),button.inputRange.rangeMax).toFloat();
		button.outputRange.rangeMin = settings.value(QString("OSC/button%1/outputMin").arg(i),button.outputRange.rangeMin).toFloat();
		button.outputRange.rangeMax = settings.value(QString("OSC/button%1/outputMax").arg(i),button.outputRange.rangeMax).toFloat();
	}

	translateX.path = settings.value("OSC/translateX/path",translateX.path.c_str()).toString().toUtf8().constData();
	translateX.inputRange.rangeMin = settings.value("OSC/translateX/inputMin",translateX.inputRange.rangeMin).toFloat();
	translateX.inputRange.rangeMax = settings.value("OSC/translateX/inputMax",translateX.inputRange.rangeMax).toFloat();
	translateX.outputRange.rangeMin = settings.value("OSC/translateX/outputMin",translateX.outputRange.rangeMin).toFloat();
	translateX.outputRange.rangeMax = settings.value("OSC/translateX/outputMax",translateX.outputRange.rangeMax).toFloat();

	translateY.path = settings.value("OSC/translateY/path",translateY.path.c_str()).toString().toUtf8().constData();
	translateY.inputRange.rangeMin = settings.value("OSC/translateY/inputMin",translateY.inputRange.rangeMin).toFloat();
	translateY.inputRange.rangeMax = settings.value("OSC/translateY/inputMax",translateY.inputRange.rangeMax).toFloat();
	translateY.outputRange.rangeMin = settings.value("OSC/translateY/outputMin",translateY.outputRange.rangeMin).toFloat();
	translateY.outputRange.rangeMax = settings.value("OSC/translateY/outputMax",translateY.outputRange.rangeMax).toFloat();

	translateZ.path = settings.value("OSC/translateZ/path",translateZ.path.c_str()).toString().toUtf8().constData();
	translateZ.inputRange.rangeMin = settings.value("OSC/translateZ/inputMin",translateZ.inputRange.rangeMin).toFloat();
	translateZ.inputRange.rangeMax = settings.value("OSC/translateZ/inputMax",translateZ.inputRange.rangeMax).toFloat();
	translateZ.outputRange.rangeMin = settings.value("OSC/translateZ/outputMin",translateZ.outputRange.rangeMin).toFloat();
	translateZ.outputRange.rangeMax = settings.value("OSC/translateZ/outputMax",translateZ.outputRange.rangeMax).toFloat();

	rotateX.path = settings.value("OSC/rotateX/path",rotateX.path.c_str()).toString().toUtf8().constData();
	rotateX.inputRange.rangeMin = settings.value("OSC/rotateX/inputMin",rotateX.inputRange.rangeMin).toFloat();
	rotateX.inputRange.rangeMax = settings.value("OSC/rotateX/inputMax",rotateX.inputRange.rangeMax).toFloat();
	rotateX.outputRange.rangeMin = settings.value("OSC/rotateX/outputMin",rotateX.outputRange.rangeMin).toFloat();
	rotateX.outputRange.rangeMax = settings.value("OSC/rotateX/outputMax",rotateX.outputRange.rangeMax).toFloat();

	rotateY.path = settings.value("OSC/rotateY/path",rotateY.path.c_str()).toString().toUtf8().constData();
	rotateY.inputRange.rangeMin = settings.value("OSC/rotateY/inputMin",rotateY.inputRange.rangeMin).toFloat();
	rotateY.inputRange.rangeMax = settings.value("OSC/rotateY/inputMax",rotateY.inputRange.rangeMax).toFloat();
	rotateY.outputRange.rangeMin = settings.value("OSC/rotateY/outputMin",rotateY.outputRange.rangeMin).toFloat();
	rotateY.outputRange.rangeMax = settings.value("OSC/rotateY/outputMax",rotateY.outputRange.rangeMax).toFloat();

	rotateZ.path = settings.value("OSC/rotateZ/path",rotateZ.path.c_str()).toString().toUtf8().constData();
	rotateZ.inputRange.rangeMin = settings.value("OSC/rotateZ/inputMin",rotateZ.inputRange.rangeMin).toFloat();
	rotateZ.inputRange.rangeMax = settings.value("OSC/rotateZ/inputMax",rotateZ.inputRange.rangeMax).toFloat();
	rotateZ.outputRange.rangeMin = settings.value("OSC/rotateZ/outputMin",rotateZ.outputRange.rangeMin).toFloat();
	rotateZ.outputRange.rangeMax = settings.value("OSC/rotateZ/outputMax",rotateZ.outputRange.rangeMax).toFloat();

	return loadedExisting;
}

////////////////////////////////////////////////////////////////////////////////

void Options::Save()
{
	QString path;
	GetConfigFilePath(path);
	QSettings settings(path, QSettings::IniFormat);

	settings.setValue("OSC/ListenPort", static_cast<int>(listenPort));
	settings.setValue("OSC/SendPort", static_cast<int>(sendPort));
	for(int i=0; i<MAX_BUTTONS; i++)
	{
		sSpaceMouseData &button = buttons[i];
		settings.setValue(QString("OSC/button%1/path").arg(i), button.path.c_str());
		settings.setValue(QString("OSC/button%1/inputMin").arg(i), QString::number(button.inputRange.rangeMin,'f',6));
		settings.setValue(QString("OSC/button%1/inputMax").arg(i), QString::number(button.inputRange.rangeMax,'f',6));
		settings.setValue(QString("OSC/button%1/outputMin").arg(i), QString::number(button.outputRange.rangeMin,'f',6));
		settings.setValue(QString("OSC/button%1/outputMax").arg(i), QString::number(button.outputRange.rangeMax,'f',6));
	}

	settings.setValue("OSC/translateX/path", translateX.path.c_str());
	settings.setValue("OSC/translateX/inputMin", QString::number(translateX.inputRange.rangeMin,'f',6));
	settings.setValue("OSC/translateX/inputMax", QString::number(translateX.inputRange.rangeMax,'f',6));
	settings.setValue("OSC/translateX/outputMin", QString::number(translateX.outputRange.rangeMin,'f',6));
	settings.setValue("OSC/translateX/outputMax", QString::number(translateX.outputRange.rangeMax,'f',6));

	settings.setValue("OSC/translateY/path", translateY.path.c_str());
	settings.setValue("OSC/translateY/inputMin", QString::number(translateY.inputRange.rangeMin,'f',6));
	settings.setValue("OSC/translateY/inputMax", QString::number(translateY.inputRange.rangeMax,'f',6));
	settings.setValue("OSC/translateY/outputMin", QString::number(translateY.outputRange.rangeMin,'f',6));
	settings.setValue("OSC/translateY/outputMax", QString::number(translateY.outputRange.rangeMax,'f',6));

	settings.setValue("OSC/translateZ/path", translateZ.path.c_str());
	settings.setValue("OSC/translateZ/inputMin", QString::number(translateZ.inputRange.rangeMin,'f',6));
	settings.setValue("OSC/translateZ/inputMax", QString::number(translateZ.inputRange.rangeMax,'f',6));
	settings.setValue("OSC/translateZ/outputMin", QString::number(translateZ.outputRange.rangeMin,'f',6));
	settings.setValue("OSC/translateZ/outputMax", QString::number(translateZ.outputRange.rangeMax,'f',6));

	settings.setValue("OSC/rotateX/path", rotateX.path.c_str());
	settings.setValue("OSC/rotateX/inputMin", QString::number(rotateX.inputRange.rangeMin,'f',6));
	settings.setValue("OSC/rotateX/inputMax", QString::number(rotateX.inputRange.rangeMax,'f',6));
	settings.setValue("OSC/rotateX/outputMin", QString::number(rotateX.outputRange.rangeMin,'f',6));
	settings.setValue("OSC/rotateX/outputMax", QString::number(rotateX.outputRange.rangeMax,'f',6));

	settings.setValue("OSC/rotateY/path", rotateY.path.c_str());
	settings.setValue("OSC/rotateY/inputMin", QString::number(rotateY.inputRange.rangeMin,'f',6));
	settings.setValue("OSC/rotateY/inputMax", QString::number(rotateY.inputRange.rangeMax,'f',6));
	settings.setValue("OSC/rotateY/outputMin", QString::number(rotateY.outputRange.rangeMin,'f',6));
	settings.setValue("OSC/rotateY/outputMax", QString::number(rotateY.outputRange.rangeMax,'f',6));

	settings.setValue("OSC/rotateZ/path", rotateZ.path.c_str());
	settings.setValue("OSC/rotateZ/inputMin", QString::number(rotateZ.inputRange.rangeMin,'f',6));
	settings.setValue("OSC/rotateZ/inputMax", QString::number(rotateZ.inputRange.rangeMax,'f',6));
	settings.setValue("OSC/rotateZ/outputMin", QString::number(rotateZ.outputRange.rangeMin,'f',6));
	settings.setValue("OSC/rotateZ/outputMax", QString::number(rotateZ.outputRange.rangeMax,'f',6));
}

////////////////////////////////////////////////////////////////////////////////

void Options::GetConfigFilePath(QString &path)
{
	path = QDir(QCoreApplication::applicationDirPath()).absoluteFilePath("config.ini");
}

////////////////////////////////////////////////////////////////////////////////

float Options::GetOutputValue(const sSpaceMouseData &data, float value)
{
	if(value < data.inputRange.rangeMin)
		value = data.inputRange.rangeMin;
	else if(value > data.inputRange.rangeMax)
		value = data.inputRange.rangeMax;

	float inputRange = (data.inputRange.rangeMax - data.inputRange.rangeMin);
	if(inputRange != 0)
	{
		float percent = (value - data.inputRange.rangeMin)/inputRange;
		return (data.outputRange.rangeMin + (data.outputRange.rangeMax-data.outputRange.rangeMin)*percent);
	}

	return data.outputRange.rangeMin;
}

////////////////////////////////////////////////////////////////////////////////

void Options::Open()
{
	QString path;
	GetConfigFilePath(path);
	QDesktopServices::openUrl( QUrl::fromLocalFile(path) );
}

////////////////////////////////////////////////////////////////////////////////
