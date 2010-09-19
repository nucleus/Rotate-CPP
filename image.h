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
*	File: image.h
*	-------------
*	File containing the definition of the Image class.
*	Image class represents an RGB image as the algorithms
*	in- or output. The class provides functions for accessing
*   information about the image as well as image I/O.
*/

#ifndef IMAGE_H
#define IMAGE_H

/**********************************************************************************
				INCLUDES & DEFINES
***********************************************************************************/

#include <fstream>
#include <iostream>
#include <stdint.h>
#include <stdio.h>

#define RGB_DEPTH 3
#define PGM_DEPTH 1
#define RGB_MAX_COLOR 255

using namespace std;

/*
*	Structure: Pixel
*	-------------
*	Structure representing the color of an RGB image pixel.
*/
typedef struct {
	uint8_t r, g, b;
} Pixel;

/*
*	Structure: Coord
*	-------------
*	Structure for representation of 2-dimensional coordinates.
*/
typedef struct {
	float x;
	float y;
} Coord;

/*
*	Class: Image
*	------------
*	Class representing an RGB image. Knows image size, color depth
*   and contains the RGB color values of the image.
*/
class Image {
	public:
        void createImageFromBuffer(int width, int height, int depth, Pixel* pels);
		bool createImageFromFile(const char *fname);
		void createImageFromTemplate(int width, int height, int depth);
		Pixel getPixelAt(int x, int y);
		void setPixelAt(int x, int y, Pixel* p);
		bool containsPixel(Coord* pix);
		unsigned int getWidth();
        unsigned int getHeight();
        unsigned int getDepth();
        unsigned int getMaxcolor();
		void clean();
	private:
		Pixel** pixels;
		unsigned int width, height;
		unsigned int depth, maxcolor;
		float x_off, y_off;
		int ppmGetInt(fstream &src);
		char ppmGetChar(fstream &src);
};
#endif
