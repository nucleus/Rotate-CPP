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
*	File: image.cpp
*	---------------
*	Implementation of the image class.
*/

/* INCLUDES */
#include "image.h"

/*
*   Function: createImageFromFile
*   -----------------------------
*   Tries to open the file specified by the given file name
*   and, if successful, fills the Image object with resolution and
*   depth information and fills the pixel vector.
*/
bool Image::createImageFromFile(const char* fname) {
    fstream in;
    char buf[2];

    in.open(fname);
    if(!in.is_open()) {
        cerr << "Cannot Open File " << fname << endl;
        return false;
    }
    /* Make sure it is an RGB binary file */
	in.read(buf, 2);
    if((buf[0] != 'P') || ((buf[1] != '6') && (buf[1] != '5'))) {
        cerr << "Wrong Image File Format: " << buf[0] << buf[1] << endl;
        in.close();
        return false;
    }
    if(buf[1] == '5') {
        depth = PGM_DEPTH;
        cerr << "Grayscale Currently Not Supported" << endl;
        in.close();
        return false;
    }
    else
        depth = RGB_DEPTH;
	
    width = ppmGetInt(in);
    height = ppmGetInt(in);
    maxcolor = ppmGetInt(in);
		
    pixels.reserve(width * height);
    for(int i = 0; i < width * height; i++) {
        Pixel *p = new Pixel;
		in.read((char*)p,3);
		pixels.push_back(p);
	}

	in.close();
    return true;
}

/*
*   Function: createImageFromBuffer
*   -------------------------------
*   Creates an Image object from a given buffer of pixels.
*   To accomplish this, image size information must be passed along.
*/
void Image::createImageFromBuffer(int width, int height, int depth, Pixel* pels) {
    this->width = width;
    this->height = height;
    this->depth = depth;
    this->maxcolor = RGB_MAX_COLOR;
    pixels.reserve(width*height);
    for(int i = 0; i < width*height; i++) {
		Pixel *p = pels + i;
        pixels.push_back(p);
	}
}

/*
*	Function: createImageFromTemplate
*	---------------------------------
*	Creates an "empty" image whose contents can be filled by using
*	the setPixelAt function.
*/
void Image::createImageFromTemplate(int width, int height, int depth) {
	this->width = width;
	this->height = height;
	this->depth = depth;
	this->maxcolor = RGB_MAX_COLOR;
	pixels.reserve(width*height);
	for(int i = 0; i < pixels.capacity(); i++) {
		Pixel *p = new Pixel;
		p->r = p->g = p->b = 0;
		pixels.push_back(p);
	}
}
/*
*   Function: getWidth
*   ------------------
*   Getter for image width.
*/
unsigned int Image::getWidth() {
    return width;
}

/*
*   Function: getHeight
*   ------------------
*   Getter for image height.
*/
unsigned int Image::getHeight() {
    return height;
}

/*
*   Function: getDepth
*   ------------------
*   Getter for image color depth (returns 3 for RGB images).
*/
unsigned int Image::getDepth() {
    return depth;
}

/*
*   Function: getMaxcolor
*   ---------------------
*   Getter for image color value range.
*/
unsigned int Image::getMaxcolor() {
    return maxcolor;
}

/*
*   Function: getPixelAt
*   --------------------
*   Returns the pixel structure at the image coordinates x (width) and y (height).
*	If the supplied coordinates are not valid, returns a black pixel.
*/
Pixel Image::getPixelAt(int x, int y) {
    Pixel p = {0,0,0};
    if(!(x >= 0 && y >= 0 && x < (int)width && y < (int)height)) 
		return p;
    return *(pixels.at(y*width + x));
}

/*
*   Function: setPixelAt
*   --------------------
*   Sets the pixel value at the specified coordinates.
*/
void Image::setPixelAt(int x, int y, Pixel* p) {
	if(x < width && y < height)
		if(pixels[width*y+x] != NULL)
			delete(pixels[width*y + x]);
		Pixel *pel = new Pixel;
		pel->r = p->r;
		pel->g = p->g;
		pel->b = p->b;
		pixels[width*y + x] = pel;
}

/*
*	Function: containsPixel
*	-----------------------
*	Returns true if the supplied coordinates lie within the picture
*	or at maximum one pixel outside the picture.
*/
bool Image::containsPixel(Coord* pix) {
	
	int true_x = (int)(pix->x + (float)width/2.0);
	int true_y = (int)abs(pix->y - (float)height/2.0);
	bool x_cond = (true_x >= 0) || (true_x < width);
	bool y_cond = (true_y >= 0) || (true_y < height);
	return x_cond && y_cond;
}

/*
*	Function: clean
*	---------------
*	Cleans up the memory used for storing the pixel colors. TODO: DEBUG
*/
void Image::clean() {
// 	for(int i = 0; i < pixels.size(); i++) {
// 		if(pixels[i] != NULL) {
// 			Pixel *p = pixels[i];
// 			delete(p);
// 			pixels[i] = NULL;
// 		}
// 	}
}

/*
*   Function: ppmGetInt
*   -------------------
*   Extracts the next integer value from the source file stream.
*   Used for getting the width, height and maxcolor values of the
*   source image.
*/
int Image::ppmGetInt(fstream &src) {
    char ch;
	int i;

	do {
		ch = ppmGetChar(src);
	} while (ch==' ' || ch=='\t' || ch=='\n' || ch=='\r' || ch=='\x00A');

	i = 0;

	do {
		i = i*10 + ch-'0';
		ch = ppmGetChar(src);
	} while (ch>='0' && ch<='9');

	return i;
}

/*
*	Function: ppmGetChar
*	--------------------
*	Returns the next char from the fstream src,
*	ignores .ppm user comments
*/
char Image::ppmGetChar(fstream &src) {
   char ch;

   ch = (char)src.get();

   if (ch=='#')
   {
      do {
         ch = (char)src.get();
      } while (ch!='\n' && ch!='\r');
   }

   return ch;
}
