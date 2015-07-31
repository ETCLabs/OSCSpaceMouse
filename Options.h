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

#ifndef OPTIONS_H
#define OPTIONS_H

#ifndef QGLOBAL_H
#include <qglobal.h>
#endif

#include <string>

////////////////////////////////////////////////////////////////////////////////

class Options
{
public:
	struct sRangeData
	{
		sRangeData()
			: rangeMin(-1.0f)
			, rangeMax(1.0f)
		{}
		float	rangeMin;
		float	rangeMax;
	};

	struct sSpaceMouseData
	{
		std::string	path;
		sRangeData	inputRange;
		sRangeData	outputRange;
	};

	enum EnumButtons
	{
		MAX_BUTTONS = 32
	};

	quint16	listenPort;
	quint16	sendPort;
	sSpaceMouseData	buttons[MAX_BUTTONS];
	sSpaceMouseData	translateX;
	sSpaceMouseData	translateY;
	sSpaceMouseData	translateZ;
	sSpaceMouseData	rotateX;
	sSpaceMouseData	rotateY;
	sSpaceMouseData	rotateZ;

	Options();

	virtual bool Load();
	virtual void Save();
	static void GetConfigFilePath(QString &path);
	static float GetOutputValue(const sSpaceMouseData &data, float value); 
	static void Open();
};

////////////////////////////////////////////////////////////////////////////////

#endif
