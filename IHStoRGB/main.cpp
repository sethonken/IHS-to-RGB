/*
-
hsv2rgb() function completely based on code from:
http://stackoverflow.com/questions/3018313/algorithm-to-convert-rgb-to-hsv-and-hsv-to-rgb-in-range-0-255-for-both
-
*/

#include "GL/freeglut.h"
#include "GL/SOIL.h"
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

//global variables
int width = 800;
int height = 600;
int yStart = -1; //y-postion of clicker when clicked

bool movingSlider = false; //true if slider is being moved
bool settingRGB = false; //used to determine when to set complement colors
int sliderMoved = -1; //0 for i, 1 for h, 2 for s

bool i_maxWidth = false; //sliders hit maxWidth
bool h_maxWidth = false; //sliders hit maxWidth
bool s_maxWidth = false; //sliders hit maxWidth

float redV;
float greenV;
float blueV;

//complement colors
float c_red;
float c_green;
float c_blue;

float rgbMax = -1;
float rgbMin = 2;

int intensityX = width / 2;
int hueX = width / 2;
int saturationX = width / 2;
double intensityRatio = 0.5;
double hueRatio = 0.5;
double saturationRatio = 0.5;
float largestColor = 0;
float smallestColor = 1;
GLuint images[4];

//functions
static void display();

static void mouse_active_move(int x, int y) {
	double widthd = width;
	if (!movingSlider) {
		movingSlider = TRUE;
		yStart = y;
	}
	if (yStart < height / 2) { //"Color frame"
		movingSlider = false; //no slider in color frame
	}
	else if (yStart > height / 2 && yStart < (height * 2 / 3)) { //"Intensity frame"
		sliderMoved = 0;
		intensityX = x;
		if (x < 0) {
			intensityX = 0;
		}
		else if (x > width) {
			i_maxWidth = TRUE;
			intensityX = width;
		}
		else {
			i_maxWidth = false;
		}
		intensityRatio = (double)(intensityX / widthd);
	}
	else if (yStart > (height * 2 / 3) && yStart < (height * 5 / 6)) { //"Hue frame"
		sliderMoved = 1;
		hueX = x;
		if (x < 0) {
			hueX = 0;
		}
		else if (x > width) {
			h_maxWidth = TRUE;
			hueX = width;
		}
		else {
			h_maxWidth = false;
		}
		hueRatio = (double)(hueX / widthd);
	}
	else if (yStart > (height * 5 / 6)) { //"Saturation frame"
		sliderMoved = 2;
		saturationX = x;
		if (x < 0) {
			saturationX = 0;
		}
		else if (x > width) {
			s_maxWidth = TRUE;
			saturationX = width;
		}
		else {
			s_maxWidth = false;
		}
		saturationRatio = (double)(saturationX / widthd);
	}
	display();
}

static void mouse_press(int button, int state, int x, int y) {
	if (button == 0 && state == 1) { //left button released
		sliderMoved = -1;
		movingSlider = false;
	}
}

void updateWindowTitle(float x, float y, float z) {
	int redV = x * 255;
	int greenV = y * 255;
	int blueV = z * 255;

	if (!movingSlider && sliderMoved == -1) {
		char * c = new char[400];
		sprintf(c, "IHS (%.3f,%.3f,%.3f) = RGB (%.3f,%.3f,%.3f) = #%02X%02X%02X",
			intensityRatio, hueRatio, saturationRatio, x, y, z, redV, greenV, blueV);
		glutSetWindowTitle(c);
	}
}

void setColor(int i) {
	double widthd = width;
	if (i < (widthd * (1.0 / 6))) {
		glColor3f(1, i / ((1.0 / 6) * widthd), 0);
	}
	else if (i < (widthd * (2.0 / 6))) {
		glColor3f(2 - i / ((1.0 / 6) * widthd), 1, 0);
	}
	else if (i < (widthd * (3.0 / 6))) {
		glColor3f(0, 1, i / ((1.0 / 6) * widthd) - 2);
	}
	else if (i < (widthd * (4.0 / 6))) {
		glColor3f(0, 4 - i / ((1.0 / 6) * widthd), 1);
	}
	else if (i < (widthd * (5.0 / 6))) {
		glColor3f(i / ((1.0 / 6) * widthd) - 4, 0, 1);
	}
	else {
		glColor3f(1, 0, 6 - i / ((1.0 / 6) * widthd));
	}
}

void hsv2rgb(float i_ratio, float h_ratio, float s_ratio) {
	float hueP = h_ratio * 360;;

	double      hh, p, q, t, ff;
	long        i;
	float redP, greenP, blueP;

	if (s_ratio <= 0.0) {       // < is bogus, just shuts up warnings
		redP = i_ratio;
		greenP = i_ratio;
		blueP = i_ratio;
		glColor3f(redP, greenP, blueP);
		if (settingRGB) {
			updateWindowTitle(redP, greenP, blueP);
			c_red = 1 - redP;
			c_green = 1 - greenP;
			c_blue = 1 - blueP;
		}
		return;
	}
	hh = hueP;
	if (hh >= 360.0) hh = 0.0;
	hh /= 60.0;
	i = (long)hh;
	ff = hh - i;
	p = i_ratio * (1.0 - s_ratio);
	q = i_ratio * (1.0 - (s_ratio * ff));
	t = i_ratio * (1.0 - (s_ratio * (1.0 - ff)));

	switch (i) {
	case 0:
		redP = i_ratio;
		greenP = t;
		blueP = p;
		break;
	case 1:
		redP = q;
		greenP = i_ratio;
		blueP = p;
		break;
	case 2:
		redP = p;
		greenP = i_ratio;
		blueP = t;
		break;

	case 3:
		redP = p;
		greenP = q;
		blueP = i_ratio;
		break;
	case 4:
		redP = t;
		greenP = p;
		blueP = i_ratio;
		break;
	case 5:
	default:
		redP = i_ratio;
		greenP = p;
		blueP = q;
		break;
	}
	glColor3f(redP, greenP, blueP);
	if (settingRGB) {
		updateWindowTitle(redP, greenP, blueP);
		c_red = 1 - redP;
		c_green = 1 - greenP;
		c_blue = 1 - blueP;
	}
}

void setComplement() {
	glColor3f(c_red, c_green, c_blue);
}


static void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//draw color frame
	settingRGB = TRUE;
	hsv2rgb(intensityRatio, hueRatio, saturationRatio);
	glViewport(0, height / 2, width, height / 2);
	glBegin(GL_QUADS);
	glVertex2f(-width, -height / 2);
	glVertex2f(-width, height / 2);
	glVertex2f(width, height / 2);
	glVertex2f(width, -height / 2);
	glEnd();
	settingRGB = false;

	//draw intensity frame
	for (int i = 0; i < width; i++) {
		double widthd = width;
		float newIntRatio = i / widthd;
		glViewport(i, height - (height * 2 / 3), 1, height / 6);
		glBegin(GL_QUADS);
		hsv2rgb(newIntRatio, hueRatio, saturationRatio);
		glVertex2f(5.0f, -height / 2);
		glVertex2f(-5.f, height / 2);
		glVertex2f(5.0f, height / 2);
		glVertex2f(-5.0f, -height / 2);
		glEnd();
	}

	//draw intensity slider
	if (i_maxWidth) {
		intensityX -= 5;
	}
	glViewport(intensityX, height - (height * 2 / 3), 9, height / 6);
	glBegin(GL_QUADS);
	setComplement();
	glVertex2f(5.0f, -height / 2);
	glVertex2f(-5.f, height / 2);
	glVertex2f(5.0f, height / 2);
	glVertex2f(-5.0f, -height / 2);
	glEnd();
	if (i_maxWidth) {
		intensityX += 5;
	}

	//draw hue frame
	for (int i = 0; i < width; i++) {
		double widthd = width;
		float newHueRatio = i / widthd;
		glViewport(i, height - (height * 5 / 6), 1, height / 6);
		glBegin(GL_QUADS);
		hsv2rgb(intensityRatio, newHueRatio, saturationRatio);
		glVertex2f(5.0f, -height / 2);
		glVertex2f(-5.f, height / 2);
		glVertex2f(5.0f, height / 2);
		glVertex2f(-5.0f, -height / 2);
		glEnd();
	}

	//draw hue slider
	if (h_maxWidth) {
		hueX -= 5;
	}
	glViewport(hueX, height - (height * 5 / 6), 9, height / 6);
	glBegin(GL_QUADS);
	setComplement();
	glVertex2f(5.0f, -height / 2);
	glVertex2f(-5.f, height / 2);
	glVertex2f(5.0f, height / 2);
	glVertex2f(-5.0f, -height / 2);
	glEnd();
	if (h_maxWidth) {
		hueX += 5;
	}

	//draw saturation frame
	for (int i = 0; i < width; i++) {
		double widthd = width;
		float newSatRatio = i / widthd;
		glViewport(i, 0, 1, height / 6);
		glBegin(GL_QUADS);
		hsv2rgb(intensityRatio, hueRatio, newSatRatio);
		glVertex2f(5.0f, -height / 2);
		glVertex2f(-5.f, height / 2);
		glVertex2f(5.0f, height / 2);
		glVertex2f(-5.0f, -height / 2);
		glEnd();
	}

	//draw saturation slider
	if (s_maxWidth) {
		saturationX -= 5;
	}
	glViewport(saturationX, 0, 9, height / 6);
	glBegin(GL_QUADS);
	setComplement();
	glVertex2f(5.0f, -height / 2);
	glVertex2f(-5.f, height / 2);
	glVertex2f(5.0f, height / 2);
	glVertex2f(-5.0f, -height / 2);
	glEnd();
	if (s_maxWidth) {
		saturationX += 5;
	}

	//place images for each section
	glColor3f(1.0f, 1.0f, 1.0f);
	glViewport(0, 0, width, height);
	glEnable(GL_TEXTURE_2D);
	float displacement = 1.0;

	glBindTexture(GL_TEXTURE_2D, images[0]);
	glBegin(GL_POLYGON);
	glTexCoord2d(0.0, 0.0); glVertex2d(-1.0, 0.92);
	glTexCoord2d(1.0, 0.0); glVertex2d(-0.8, 0.92);
	glTexCoord2d(1.0, 1.0); glVertex2d(-0.8, 1.0);
	glTexCoord2d(0.0, 1.0); glVertex2d(-1.0, 1.0);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, images[1]);
	glBegin(GL_POLYGON);
	glTexCoord2d(0.0, 0.0); glVertex2d(-1.0, 0.92 - displacement);
	glTexCoord2d(1.0, 0.0); glVertex2d(-0.8, 0.92 - displacement);
	glTexCoord2d(1.0, 1.0); glVertex2d(-0.8, 1.0 - displacement);
	glTexCoord2d(0.0, 1.0); glVertex2d(-1.0, 1.0 - displacement);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, images[2]);
	glBegin(GL_POLYGON);
	displacement = 1.3333;
	glTexCoord2d(0.0, 0.0); glVertex2d(-1.0, 0.92 - displacement);
	glTexCoord2d(1.0, 0.0); glVertex2d(-0.8, 0.92 - displacement);
	glTexCoord2d(1.0, 1.0); glVertex2d(-0.8, 1.0 - displacement);
	glTexCoord2d(0.0, 1.0); glVertex2d(-1.0, 1.0 - displacement);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, images[3]);
	glBegin(GL_POLYGON);
	displacement = 1.6666;
	glTexCoord2d(0.0, 0.0); glVertex2d(-1.0, 0.92 - displacement);
	glTexCoord2d(1.0, 0.0); glVertex2d(-0.8, 0.92 - displacement);
	glTexCoord2d(1.0, 1.0); glVertex2d(-0.8, 1.0 - displacement);
	glTexCoord2d(0.0, 1.0); glVertex2d(-1.0, 1.0 - displacement);
	glEnd();

	glDisable(GL_TEXTURE_2D);

	//SwapBuffers
	glutSwapBuffers();
}

void loadImages() {
	images[0] = SOIL_load_OGL_texture("Images/Color.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);
	images[1] = SOIL_load_OGL_texture("Images/Intensity.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);
	images[2] = SOIL_load_OGL_texture("Images/Hue.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);
	images[3] = SOIL_load_OGL_texture("Images/Saturation.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);
}

void reshape(int w, int h) {
	movingSlider = false;
	width = w;
	height = h;

	//set ratios
	intensityX = (int)(intensityRatio * w);
	hueX = (int)(hueRatio * w);
	saturationX = (int)(saturationRatio * w);

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	display();
}

void init(void) {
	/* select clearing (background) color */
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	float s = width / 2.0;
	if (height < width) s = height / 2;
	glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
	//apply viewing direction
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow) {
	glutInit(&__argc, __argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);

	glutInitWindowSize(width, height);
	glutInitWindowPosition(700, 200);
	glutCreateWindow("");

	init();
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);

	//mouse callbacks
	glutMouseFunc(mouse_press);
	glutMotionFunc(mouse_active_move);

	loadImages();
	glClearColor(0, 0, 0, 0);

	glutMainLoop();
	return 0;
}