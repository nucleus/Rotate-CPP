/*
*	"Image Rotate", a program to rotate images by a user-specifyable angle.
*	
*	Copyright (C) 2010 Michael Andersch
*	
*	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.
*	
*	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
*	
*	You should have received a copy of the GNU General Public License along with this program; if not, see <http://www.gnu.org/licenses/>.
*/

/*
*	File: rotation_engine.h
*	-----------------------
*	Header file for the rotation core. Contains the rotateengine object
*	definition.
*/

/**********************************************************************************
				INCLUDES & DEFINES
***********************************************************************************/
#ifndef R_ENGINE_H
#define R_ENGINE_H

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <cmath>
#include <float.h>
#include "image.h"

using namespace std;

/*
*	Class: RotateEngine
*	-------------------
*   Container object for the benchmark. Contains benchmark state, i.e.
*   input and output images as well as additional kernel data.
*/
class RotateEngine {
    public:
		RotateEngine();
		void run();
		void finish();
		bool init(string srcname, string destname, unsigned int angle);
        void printRotationState();
    private:
        string srcname, destname;
		Image input, output;
		unsigned int angle;
        bool initialized, done;
		Coord ul, ur, ll, lr, c1, c2, c3, c4;
        bool writeOutImage();
		Coord rotatePoint(Coord *pt, unsigned int angle);
		double round(double num, int digits);
		int computeTargetHeight();
		int computeTargetWidth();
		float findMax(float* seq);
		float findMin(float* seq);
		Pixel filter(Pixel* colors, Coord* sample_pos);
		Pixel interpolateLinear(Pixel* a, Pixel* b, float weight);
};

#endif
