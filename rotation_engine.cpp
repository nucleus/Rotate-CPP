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
*	File: rotation_engine.cpp
*	-------------------------
*	Implementation file for the rotation object.
*/

/* INCLUDES */
#include "rotation_engine.h"

#define PI M_PI
#define PRECISION 3
#define printPoint(a) printf("(%d,%d)\n",(int)a.x,(int)a.y)

using namespace std;

/**********************************************************************************
				PUBLIC FUNCTIONS
***********************************************************************************/

/*
*	Function: Constructor
*	---------------------
*	Performs initial state setup.
*/
RotateEngine::RotateEngine() {
	done = false;
	initialized = false;
}

/*
*	Function: init
*	------------------
*	Prepares the rotation core for running the kernel. Sets up needed
*   parameters and loads in the input image.
*/
bool RotateEngine::init(string srcname, string destname, unsigned int angle) {
    this->angle = angle;
    this->srcname = srcname;
    this->destname = destname;
	c1 = c2 = c3 = c4 = {0.0, 0.0};
    cout << "Trying to open image file " << srcname << " ... " << endl;
    if(input.createImageFromFile(srcname.c_str()) != true) return false;
	// Fill input image corner coordinates
	float xc = (float)input.getWidth()/2.0;
	float yc = (float)input.getHeight()/2.0;
	ul.x = -xc;
	ul.y = yc;
	ur.x = xc;
	ur.y = yc;
	ll.x = -xc;
	ll.y = -yc;
	lr.x = xc;
	lr.y = -yc;
	initialized = true;
    return true;
}

/*
*   Function: run
*   -------------
*   Runs the benchmark kernel. When completed successfully, done will be set to true
*   and output will contain the output image.
*/
void RotateEngine::run() {
	if(!initialized) {
		fprintf(stderr, "Kernel Called Without Initialization\n");
		return;
	}
	unsigned int height = input.getHeight();
	unsigned int width = input.getWidth();
	unsigned int depth = input.getDepth();
	float x_offset_source = (float)width / 2.0;
	float y_offset_source = (float)height / 2.0;
	
	Pixel * buffer;
	
	/* Steps for rotation:
		1. Determine target image size by rotating corners
		2. For each pixel in target image, do
			- backwards rotation to determine origin location
			- for origin location, sample and filter 4 closest neighbour pixels
			- write colour value appropriately
	*/
	
	/* STEP 1 */
	c1 = rotatePoint(&ul, angle);
	c2 = rotatePoint(&ur, angle);
	c3 = rotatePoint(&ll, angle);
	c4 = rotatePoint(&lr, angle);
	int target_h = computeTargetHeight();
	int target_w = computeTargetWidth();
	buffer = new Pixel[target_h * target_w];
		
	/* STEP 2 */
	unsigned int rev_angle = 360 - angle;
	float x_offset_target = (float)target_w/2.0;
	float y_offset_target = (float)target_h/2.0;
	
	for(int i = 0; i < target_h; i++) {
		for(int j = 0; j < target_w; j++) {
			/* Find origin pixel for current destination pixel */
			Coord cur = {-x_offset_target + (float)j, y_offset_target - (float)i};
			Coord origin_pix = rotatePoint(&cur, rev_angle);
			/* If original image contains point, sample colour and write back */
			if(input.containsPixel(&origin_pix)) {
				int samples[4][2];
				Pixel colors[4];
				/* Get sample positions */
				for(int k = 0; k < 4; k++) {
					samples[k][0] = (int)(origin_pix.x + x_offset_source) + ((k == 2 || k == 3) ? 1 : 0);
					samples[k][1] = (int)abs(origin_pix.y - y_offset_source) + ((k == 1 || k == 3) ? 1 : 0);
				}
				/* Get colors for samples */
				for(int k = 0; k < 4; k++) {
					colors[k] = input.getPixelAt(samples[k][0], samples[k][1]);
				}
				/* Filter colors */
				Pixel final = filter(colors, &origin_pix);
				/* Write output */
				buffer[i*target_w + j] = final;
			}
			else {
				/* Pixel is not in source image, write black color */
				buffer[i*target_w + j] = {0,0,0};
			}
		}
	}
	output.createImageFromBuffer(target_w, target_h, depth, buffer);
	done = true;
}

/*
*   Function: finish
*   ----------------
*   Performs garbage collection, i.e. deletes dynamically allocated memory
*   and writes the output back to disk.
*/
void RotateEngine::finish() {
    if(!writeOutImage()) fprintf(stderr, "Could Not Write Rotation Output\n");
	input.clean();
	output.clean();
}

/*
*   Function: printRotationState
*   ----------------------------
*   Prints the current information contained in the RotateEngine object
*   on the console.
*/
void RotateEngine::printRotationState() {
    fprintf(stdout, "_____ Kernel State _____\n");
	fprintf(stdout, "Width: %d\t Height: %d\n", input.getWidth(), input.getHeight());
	fprintf(stdout, "Pixels: %.2fM\t Angle: %d°\n", (double)(input.getWidth()*input.getHeight())/1000000.0, (int)angle);
	fprintf(stdout, "Source file: %s\t Dest. File: %s\n", srcname.c_str(), destname.c_str());
}

/**********************************************************************************
				PRIVATE FUNCTIONS
***********************************************************************************/

/*
*	Function: writeOutImage
*	-----------------------
*	Writes output image of the kernel back to disk. Returns false if
*	called before an output image is produced. Returns true if writing back
*	the image was successful.
*/
bool RotateEngine::writeOutImage() {
	if(!done) 
		return false;
    fstream out;
    out.open(destname.c_str(),fstream::out);
    if(!out.is_open()) {
        return false;
	}
    if(output.getDepth() == 3) {
        out << "P6\n";
    } else {
        return false;
	}
    out << output.getWidth() << " " << output.getHeight() << "\n" << output.getMaxcolor() << "\n";
    for(int i = 0; i < (int)output.getHeight(); i++) {
        for(int j = 0; j < (int)output.getWidth(); j++) {
            Pixel p = output.getPixelAt(j, i);
			out.put(p.r);
			out.put(p.g);
			out.put(p.b);
        }
    }
	out.close();
    return true;
}

/*
*	Function: rotatePoint
*	---------------------
*	Takes a point in 2d space and rotates it by angle.
*/
inline Coord RotateEngine::rotatePoint(Coord *pt, unsigned int angle) {
	Coord result;
	float rad = (float)angle/180 * PI;
	result.x = pt->x * cos(rad) - pt->y * sin(rad);
	result.y = pt->x * sin(rad) + pt->y * cos(rad);
	return result;
}

/*
*	Function: round
*	---------------
*	Simple, optimized function for rounding fp numbers. 
*/
double RotateEngine::round(double num, int digits) {
    double v[] = {1, 10, 1e2, 1e3, 1e4, 1e5, 1e6, 1e7, 1e8};
	if(digits > (sizeof(v)/sizeof(double))) return num;
    return floor(num * v[digits] + 0.5) / v[digits];
}

/*
*	Function: computeTargetHeight
*	-----------------------
*	Computes the height of the output image.
*/
int RotateEngine::computeTargetHeight() {
	float seq[4] = {c1.y, c2.y, c3.y, c4.y};
	float max = findMax(seq);
	float min = findMin(seq);
	int height = (int)(max - min);
	return height;
}

/*
*	Function: computeTargetWidth
*	-----------------------
*	Computes the width of the output image.
*/
int RotateEngine::computeTargetWidth() {
	float seq[4] = {c1.x, c2.x, c3.x, c4.x};
	float max = findMax(seq);
	float min = findMin(seq);
	int width = (int)(max - min);
	return width;
}

/*
*	Function: findMax
*	-----------------
*	Find the maximum number in a 4-element sequence.
*/
inline float RotateEngine::findMax(float* seq) {
	float max = FLT_MIN;
	for(int i = 0; i < 4; i++) {
		if(seq[i] > max) 
			max = seq[i];
	}
	return max;
}

/*
*	Function: findMin
*	-----------------
*	Find the minimum number in a 4-element sequence.
*/
inline float RotateEngine::findMin(float* seq) {
	float min = FLT_MAX;
	for(int i = 0; i < 4; i++) {
		if(seq[i] < min) 
			min = seq[i];
	}
	return min;

}

/*
*	Function: filter
*	---------------------
*	Filters a given array of pixel colours of length 4, blending
*	color values into a final pixel. The algorithm used is bilinear
*	filtering, using the sample position as a weight for color blend.
*/
inline Pixel RotateEngine::filter(Pixel* colors, Coord* sample_pos) {
	uint32_t r, g, b;
	float x_weight = round(sample_pos->x - floor(sample_pos->x), PRECISION);
	float y_weight = round(sample_pos->y - floor(sample_pos->y), PRECISION);
	
	Pixel sample_v_upper = interpolateLinear(&colors[0], &colors[3], x_weight);
	Pixel sample_v_lower = interpolateLinear(&colors[1], &colors[2], x_weight);
	Pixel sample_h = interpolateLinear(&sample_v_upper, &sample_v_lower, y_weight);
	
	return sample_h;
}

/*
*	Function: interpolateLinear
*	---------------------------
*	Linearly interpolates two pixel colors according to a given weight factor.
*/
inline Pixel RotateEngine::interpolateLinear(Pixel* a, Pixel* b, float weight) {
	Pixel final;
	final.r = a->r * (1.0-weight) + b->r * weight;
	final.g = a->g * (1.0-weight) + b->g * weight;
	final.b = a->b * (1.0-weight) + b->b * weight;
	return final;
}

