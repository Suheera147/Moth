/*********
 CTIS164
----------
STUDENT : Suheera Tanvir
STUDENT ID: 21801211
SECTION : 02
HOMEWORK: 04
----------
PROBLEMS:

----------
ADDITIONAL FEATURES:
1) Lots of shapes and objects.
2) Added 3 different backgrounds you can toggle between.
3) Added Gamestates. Howtoplay gamestate, pause/unpause (different for each background) and gameover state.
*********/

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>
#include "vec.h"

#define WINDOW_WIDTH  1050
#define WINDOW_HEIGHT 700

#define TIMER_PERIOD  7 // Period for the timer.
#define TIMER_ON         1 // 0:disable timer, 1:enable timer

#define D2R 0.0174532

#define HOWTOPLAY 0   //gamestates
#define BACKGROUND 1
#define BACKGROUND2 2
#define BACKGROUND3 3
#define PAUSESCREEN 4
#define PAUSESCREEN2 5
#define PAUSESCREEN3 6
#define GAMEOVER 7

int gameState = HOWTOPLAY;

bool up = false, down = false, right = false, left = false;
int  winWidth, winHeight;

typedef struct {
	vec_t pos;
	vec_t vel;  //velocity of bullet/moth/rocket
} bullet_t;

bullet_t bullet = { {-245, 100} };


bool gameover = false; //for gamestate
bool upT = true;       //to toggle between whether the light source must come from the top of the screen or bottom
bool showBullet = false; //no bullet at the beginning of the game

typedef struct {
	float r, g, b;
} color_t;

typedef struct {
	vec_t   pos;
	color_t color;
	vec_t   vel;
} light_t;

typedef struct {
	vec_t pos;
	vec_t N;
} vertex_t;

light_t light = { {-460, 300}, { 0.764, 0.588, 0.094 }, };

color_t mulColor(float k, color_t c) {
	color_t tmp = { k * c.r, k * c.g, k * c.b };
	return tmp;
}

color_t addColor(color_t c1, color_t c2) {
	color_t tmp = { c1.r + c2.r, c1.g + c2.g, c1.b + c2.b };
	return tmp;
}

double distanceImpact(double d) {
	return (-1.0 / 350.0) * d + 1.0;
}

color_t calculateColor(light_t source, vertex_t v) {
	vec_t L = subV(source.pos, v.pos);
	vec_t uL = unitV(L);
	float factor = dotP(uL, v.N) * distanceImpact(magV(L));
	return mulColor(factor, source.color);
}

void circle(int x, int y, int r)
{
#define PI 3.1415
	float angle;
	glBegin(GL_POLYGON);
	for (int i = 0; i < 100; i++)
	{
		angle = 2 * PI*i / 100;
		glVertex2f(x + r * cos(angle), y + r * sin(angle));
	}
	glEnd();
}

void circle_wire(int x, int y, int r)
{
#define PI 3.1415
	float angle;

	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < 100; i++)
	{
		angle = 2 * PI*i / 100;
		glVertex2f(x + r * cos(angle), y + r * sin(angle));
	}
	glEnd();
}

void print(int x, int y, const char *string, void *font)
{
	int len, i;

	glRasterPos2f(x, y);
	len = (int)strlen(string);
	for (i = 0; i < len; i++)
	{
		glutBitmapCharacter(font, string[i]);
	}
}

void vprint(int x, int y, void *font, const char *string, ...)
{
	va_list ap;
	va_start(ap, string);
	char str[1024];
	vsprintf_s(str, string, ap);
	va_end(ap);

	int len, i;
	glRasterPos2f(x, y);
	len = (int)strlen(str);
	for (i = 0; i < len; i++)
	{
		glutBitmapCharacter(font, str[i]);
	}
}


void vprint2(int x, int y, float size, const char *string, ...) {
	va_list ap;
	va_start(ap, string);
	char str[1024];
	vsprintf_s(str, string, ap);
	va_end(ap);
	glPushMatrix();
	glTranslatef(x, y, 0);
	glScalef(size, size, 1);

	int len, i;
	len = (int)strlen(str);
	for (i = 0; i < len; i++)
	{
		glutStrokeCharacter(GLUT_STROKE_ROMAN, str[i]);
	}
	glPopMatrix();
}


void bulb()
{
	glColor3f(0.937, 0.839, 0.164);
	glBegin(GL_QUADS);
	glVertex2f(light.pos.x - 20, light.pos.y);
	glVertex2f(light.pos.x + 20, light.pos.y);
	glVertex2f(light.pos.x + 20, light.pos.y - 30);
	glVertex2f(light.pos.x - 20, light.pos.y - 30);
	glEnd();
	glColor3f(0, 0, 0);
	glBegin(GL_LINE_LOOP);
	glVertex2f(light.pos.x - 20, light.pos.y);
	glVertex2f(light.pos.x + 20, light.pos.y);
	glVertex2f(light.pos.x + 20, light.pos.y - 30);
	glVertex2f(light.pos.x - 20, light.pos.y - 30);
	glEnd();

	glColor3f(0.937, 0.839, 0.164);
	circle(light.pos.x, light.pos.y + 20, 33);

	glLineWidth(1.5);
	glColor3f(0, 0, 0);
	glBegin(GL_LINE_STRIP);
	glVertex2f(light.pos.x - 20, light.pos.y - 6.7);
	glVertex2f(light.pos.x - 20, light.pos.y + 30);
	glVertex2f(light.pos.x - 10, light.pos.y + 10);
	glVertex2f(light.pos.x, light.pos.y + 30);
	glVertex2f(light.pos.x + 10, light.pos.y + 10);
	glVertex2f(light.pos.x + 20, light.pos.y + 30);
	glVertex2f(light.pos.x + 20, light.pos.y - 6.7);
	glEnd();
	glLineWidth(1);

	glColor3f(0.552, 0.533, 0.384);
	glBegin(GL_QUADS);
	glVertex2f(light.pos.x - 20, light.pos.y - 30);
	glVertex2f(light.pos.x + 20, light.pos.y - 30);
	glVertex2f(light.pos.x + 20, light.pos.y - 35);
	glVertex2f(light.pos.x - 20, light.pos.y - 35);
	glEnd();
	glColor3f(0, 0, 0);
	glBegin(GL_LINE_LOOP);
	glVertex2f(light.pos.x - 20, light.pos.y - 30);
	glVertex2f(light.pos.x + 20, light.pos.y - 30);
	glVertex2f(light.pos.x + 20, light.pos.y - 35);
	glVertex2f(light.pos.x - 20, light.pos.y - 35);
	glEnd();


	glColor3f(0.552, 0.533, 0.384);
	glBegin(GL_QUADS);
	glVertex2f(light.pos.x - 15, light.pos.y - 40);
	glVertex2f(light.pos.x + 15, light.pos.y - 40);
	glVertex2f(light.pos.x + 15, light.pos.y - 35);
	glVertex2f(light.pos.x - 15, light.pos.y - 35);
	glEnd();
	glColor3f(0, 0, 0);
	glBegin(GL_LINE_LOOP);
	glVertex2f(light.pos.x - 15, light.pos.y - 40);
	glVertex2f(light.pos.x + 15, light.pos.y - 40);
	glVertex2f(light.pos.x + 15, light.pos.y - 35);
	glVertex2f(light.pos.x - 15, light.pos.y - 35);
	glEnd();

	glColor3f(0.552, 0.533, 0.384);
	glBegin(GL_QUADS);
	glVertex2f(light.pos.x - 15, light.pos.y - 40);
	glVertex2f(light.pos.x + 15, light.pos.y - 40);
	glVertex2f(light.pos.x + 15, light.pos.y - 45);
	glVertex2f(light.pos.x - 15, light.pos.y - 45);
	glEnd();
	glColor3f(0, 0, 0);
	glBegin(GL_LINE_LOOP);
	glVertex2f(light.pos.x - 15, light.pos.y - 40);
	glVertex2f(light.pos.x + 15, light.pos.y - 40);
	glVertex2f(light.pos.x + 15, light.pos.y - 45);
	glVertex2f(light.pos.x - 15, light.pos.y - 45);
	glEnd();


	glColor3f(0.552, 0.533, 0.384);
	glBegin(GL_QUADS);
	glVertex2f(light.pos.x - 15, light.pos.y - 45);
	glVertex2f(light.pos.x + 15, light.pos.y - 45);
	glVertex2f(light.pos.x + 15, light.pos.y - 50);
	glVertex2f(light.pos.x - 15, light.pos.y - 50);
	glEnd();
	glColor3f(0, 0, 0);
	glBegin(GL_LINE_LOOP);
	glVertex2f(light.pos.x - 15, light.pos.y - 50);
	glVertex2f(light.pos.x + 15, light.pos.y - 50);
	glVertex2f(light.pos.x + 15, light.pos.y - 45);
	glVertex2f(light.pos.x - 15, light.pos.y - 45);
	glEnd();
}

void moth()
{
	glColor3f(0.662, 0.219, 0.117);
	glBegin(GL_TRIANGLES);
	glVertex2f(bullet.pos.x, bullet.pos.y + 10);
	glVertex2f(bullet.pos.x - 20, bullet.pos.y - 30);
	glVertex2f(bullet.pos.x, bullet.pos.y - 20);
	glEnd();
	glBegin(GL_TRIANGLES);
	glVertex2f(bullet.pos.x, bullet.pos.y - 20);
	glVertex2f(bullet.pos.x + 20, bullet.pos.y - 30);
	glVertex2f(bullet.pos.x, bullet.pos.y + 10);
	glEnd();
	glColor3f(0, 0, 0);
	glBegin(GL_LINE_LOOP);
	glVertex2f(bullet.pos.x, bullet.pos.y + 10);
	glVertex2f(bullet.pos.x - 20, bullet.pos.y - 30);
	glVertex2f(bullet.pos.x, bullet.pos.y - 20);
	glVertex2f(bullet.pos.x + 20, bullet.pos.y - 30);
	glEnd();


	glLineWidth(1.9);
	glColor3f(0, 0, 0);
	glBegin(GL_LINE_STRIP);
	glVertex2f(bullet.pos.x - 10, bullet.pos.y + 30);
	glVertex2f(bullet.pos.x, bullet.pos.y + 10);
	glVertex2f(bullet.pos.x + 10, bullet.pos.y + 30);
	glEnd();
	glLineWidth(1);

	glLineWidth(3);
	glColor3f(0, 0, 0);
	glBegin(GL_LINE_STRIP);
	glVertex2f(bullet.pos.x, bullet.pos.y - 25);
	glVertex2f(bullet.pos.x, bullet.pos.y + 10);
	glEnd();
	glLineWidth(1);

	glColor3f(0.392, 0.223, 0.047);
	circle(bullet.pos.x, bullet.pos.y + 10, 5);
	glColor3f(0, 0, 0);
	circle_wire(bullet.pos.x, bullet.pos.y + 10, 5);

	glColor3f(0.219, 0.074, 0.019);
	circle(bullet.pos.x - 8, bullet.pos.y - 16, 3);
	glColor3f(0, 0, 0);
	circle_wire(bullet.pos.x - 8, bullet.pos.y - 16, 3);

	glColor3f(0.219, 0.074, 0.019);
	circle(bullet.pos.x + 8, bullet.pos.y - 16, 3);
	glColor3f(0, 0, 0);
	circle_wire(bullet.pos.x + 8, bullet.pos.y - 16, 3);

}

void apple()
{

	glColor3f(0.184, 0.407, 0.070);
	glBegin(GL_QUADS);
	glVertex2f(light.pos.x, light.pos.y + 35);
	glVertex2f(light.pos.x + 10, light.pos.y + 45);
	glVertex2f(light.pos.x + 30, light.pos.y + 50);
	glVertex2f(light.pos.x + 10, light.pos.y + 35);
	glEnd();
	glColor3f(0, 0, 0);
	glBegin(GL_LINE_LOOP);
	glVertex2f(light.pos.x, light.pos.y + 35);
	glVertex2f(light.pos.x + 10, light.pos.y + 45);
	glVertex2f(light.pos.x + 30, light.pos.y + 50);
	glVertex2f(light.pos.x + 10, light.pos.y + 35);
	glEnd();

	glColor3f(0.407, 0.137, 0.070);
	glBegin(GL_TRIANGLES);
	glVertex2f(light.pos.x, light.pos.y);
	glVertex2f(light.pos.x - 5, light.pos.y + 50);
	glVertex2f(light.pos.x + 5, light.pos.y + 60);
	glEnd();
	glColor3f(0, 0, 0);
	glBegin(GL_LINE_LOOP);
	glVertex2f(light.pos.x, light.pos.y);
	glVertex2f(light.pos.x - 5, light.pos.y + 50);
	glVertex2f(light.pos.x + 5, light.pos.y + 60);
	glEnd();

	glColor3f(0.764, 0.588, 0.094);
	circle(light.pos.x - 20, light.pos.y, 30);
	glColor3f(0, 0, 0);
	circle_wire(light.pos.x - 20, light.pos.y, 30);

	glColor3f(0.764, 0.588, 0.094);
	circle(light.pos.x + 20, light.pos.y, 30);
	glColor3f(0, 0, 0);
	circle_wire(light.pos.x + 20, light.pos.y, 30);

	glColor3f(0.764, 0.588, 0.094);
	glBegin(GL_QUADS);
	glVertex2f(light.pos.x - 20, light.pos.y + 20);
	glVertex2f(light.pos.x + 20, light.pos.y + 20);
	glVertex2f(light.pos.x + 20, light.pos.y - 20);
	glVertex2f(light.pos.x - 20, light.pos.y - 20);
	glEnd();
}

void dBullet()
{

	glColor3f(0.313, 0.309, 0.349);
	circle(bullet.pos.x, bullet.pos.y, 10);
	glLineWidth(1.9);
	glColor3f(0, 0, 0);
	circle_wire(bullet.pos.x, bullet.pos.y, 10);
	glLineWidth(1);
}

void sun()
{
	glColor3f(0.980, 0.862, 0.058);
	circle(light.pos.x, light.pos.y, 50);
	glColor3f(0.584, 0.505, 0.058);
	circle_wire(light.pos.x, light.pos.y, 50);
}

void rocket()
{

	glColor3f(0.898, 0.454, 0.023);
	glBegin(GL_QUADS);
	glVertex2f(bullet.pos.x - 10, bullet.pos.y + 10);
	glVertex2f(bullet.pos.x + 10, bullet.pos.y + 10);
	glVertex2f(bullet.pos.x + 10, bullet.pos.y - 20);
	glVertex2f(bullet.pos.x - 10, bullet.pos.y - 20);
	glEnd();
	glColor3f(0, 0, 0);
	glBegin(GL_LINE_LOOP);
	glVertex2f(bullet.pos.x - 10, bullet.pos.y + 10);
	glVertex2f(bullet.pos.x + 10, bullet.pos.y + 10);
	glVertex2f(bullet.pos.x + 10, bullet.pos.y - 20);
	glVertex2f(bullet.pos.x - 10, bullet.pos.y - 20);
	glEnd();


	glColor3f(0.898, 0.137, 0.023);
	glBegin(GL_TRIANGLES);
	glVertex2f(bullet.pos.x - 10, bullet.pos.y + 10);
	glVertex2f(bullet.pos.x + 10, bullet.pos.y + 10);
	glVertex2f(bullet.pos.x, bullet.pos.y + 20);
	glEnd();
	glColor3f(0, 0, 0);
	glBegin(GL_LINE_LOOP);
	glVertex2f(bullet.pos.x - 10, bullet.pos.y + 10);
	glVertex2f(bullet.pos.x + 10, bullet.pos.y + 10);
	glVertex2f(bullet.pos.x, bullet.pos.y + 20);
	glEnd();

	glColor3f(0.482, 0.078, 0.086);
	glBegin(GL_TRIANGLES);
	glVertex2f(bullet.pos.x - 10, bullet.pos.y + 10);
	glVertex2f(bullet.pos.x - 20, bullet.pos.y - 10);
	glVertex2f(bullet.pos.x - 10, bullet.pos.y - 10);
	glEnd();
	glColor3f(0, 0, 0);
	glBegin(GL_LINE_LOOP);
	glVertex2f(bullet.pos.x - 10, bullet.pos.y + 10);
	glVertex2f(bullet.pos.x - 20, bullet.pos.y - 10);
	glVertex2f(bullet.pos.x - 10, bullet.pos.y - 10);
	glEnd();

	glColor3f(0.482, 0.078, 0.086);
	glBegin(GL_TRIANGLES);
	glVertex2f(bullet.pos.x + 10, bullet.pos.y + 10);
	glVertex2f(bullet.pos.x + 20, bullet.pos.y - 10);
	glVertex2f(bullet.pos.x + 10, bullet.pos.y - 10);
	glEnd();
	glColor3f(0, 0, 0);
	glBegin(GL_LINE_LOOP);
	glVertex2f(bullet.pos.x + 10, bullet.pos.y + 10);
	glVertex2f(bullet.pos.x + 20, bullet.pos.y - 10);
	glVertex2f(bullet.pos.x + 10, bullet.pos.y - 10);
	glEnd();

	glColor3f(0.462, 0.317, 0.317);
	glBegin(GL_QUADS);
	glVertex2f(bullet.pos.x - 7, bullet.pos.y - 20);
	glVertex2f(bullet.pos.x + 7, bullet.pos.y - 20);
	glVertex2f(bullet.pos.x + 7, bullet.pos.y - 26);
	glVertex2f(bullet.pos.x - 7, bullet.pos.y - 26);
	glEnd();
	glColor3f(0, 0, 0);
	glBegin(GL_LINE_LOOP);
	glVertex2f(bullet.pos.x - 7, bullet.pos.y - 20);
	glVertex2f(bullet.pos.x + 7, bullet.pos.y - 20);
	glVertex2f(bullet.pos.x + 7, bullet.pos.y - 26);
	glVertex2f(bullet.pos.x - 7, bullet.pos.y - 26);
	glEnd();

	glColor3f(0.505, 0.933, 0.917);
	circle(bullet.pos.x, bullet.pos.y, 5);
	glLineWidth(1.3);
	glColor3f(0, 0, 0);
	circle_wire(bullet.pos.x, bullet.pos.y, 5);
	glLineWidth(1);
}



void drawBackground()
{
	glColor3f(0.054, 0.031, 0.188);
	glBegin(GL_QUADS);
	glVertex2f(-1000, 1000);
	glVertex2f(1000, 1000);
	glVertex2f(1000, -1000);
	glVertex2f(-1000, -1000);
	glEnd();

	glColor3f(0.901, 0.839, 0.874);
	circle(-30, 230, 2);

	glColor3f(0.901, 0.839, 0.874);
	circle(0, 200, 2);

	glColor3f(0.901, 0.839, 0.874);
	circle(-10, 300, 2);

	glColor3f(0.901, 0.839, 0.874);
	circle(100, 310, 2);

	glColor3f(0.901, 0.839, 0.874);
	circle(-60, 340, 2);

	glColor3f(0.901, 0.839, 0.874);
	circle(170, 190, 2);

	glColor3f(0.901, 0.839, 0.874);
	circle(130, 290, 2);

	glColor3f(0.901, 0.839, 0.874);
	circle(-130, 290, 2);

	glColor3f(0.901, 0.839, 0.874);
	circle(-230, 270, 2);

	glColor3f(0.901, 0.839, 0.874);
	circle(-330, 300, 2);

	glColor3f(0.901, 0.839, 0.874);
	circle(-430, 320, 2);

	glColor3f(0.901, 0.839, 0.874);
	circle(-450, 280, 2);

	glColor3f(0.901, 0.839, 0.874);
	circle(450, 280, 2);

	glColor3f(0.901, 0.839, 0.874);
	circle(430, 320, 2);

	glColor3f(0.901, 0.839, 0.874);
	circle(440, 120, 2);

	glColor3f(0.901, 0.839, 0.874);
	circle(310, 100, 2);

	glColor3f(0.901, 0.839, 0.874);
	circle(218, 90, 2);

	glColor3f(0.882, 0.878, 0.901);
	circle(-360, 240, 50);
	glColor3f(0, 0, 0);
	circle_wire(-360, 240, 50);

	glColor3f(0.694, 0.682, 0.780);
	circle(-350, 220, 15);

	glColor3f(0.694, 0.682, 0.780);
	circle(-340, 250, 10);

	glColor3f(0.694, 0.682, 0.780);
	circle(-380, 230, 7);

	bulb();
	if (showBullet == true)
		moth();

}

void drawBackground2()
{
	//Ceiling + outline - Quad & line loop
	glColor3f(0.431, 0.368, 0.027);
	glBegin(GL_QUADS);
	glVertex2f(-700, 400);
	glVertex2f(700, 400);
	glVertex2f(252, 275);
	glVertex2f(-252, 275);
	glEnd();
	glColor3f(0, 0, 0);
	glBegin(GL_LINE_LOOP);
	glVertex2f(-700, 400);
	glVertex2f(700, 400);
	glVertex2f(252, 275);
	glVertex2f(-252, 275);
	glEnd();

	//middle wall
	glColor3f(0.062, 0.078, 0.235);
	glBegin(GL_QUADS);
	glVertex2f(-252, 275);
	glVertex2f(252, 275);
	glVertex2f(252, 25);
	glVertex2f(-252, 25);
	glEnd();
	glColor3f(0, 0, 0);
	glBegin(GL_LINE_LOOP);
	glVertex2f(-252, 275);
	glVertex2f(252, 275);
	glVertex2f(252, 25);
	glVertex2f(-252, 25);
	glEnd();

	//left wall
	glColor3f(0.180, 0.188, 0.258);
	glBegin(GL_QUADS);
	glVertex2f(-700, 400);
	glVertex2f(-252, 275);
	glVertex2f(-252, 25);
	glVertex2f(-700, -400);
	glEnd();
	glColor3f(0, 0, 0);
	glBegin(GL_LINE_LOOP);
	glVertex2f(-700, 400);
	glVertex2f(-252, 275);
	glVertex2f(-252, 25);
	glVertex2f(-700, -400);
	glEnd();

	//right wall
	glColor3f(0.180, 0.188, 0.258);
	glBegin(GL_QUADS);
	glVertex2f(700, 400);
	glVertex2f(252, 275);
	glVertex2f(252, 25);
	glVertex2f(700, -400);
	glEnd();
	glColor3f(0, 0, 0);
	glBegin(GL_LINE_LOOP);
	glVertex2f(700, 400);
	glVertex2f(252, 275);
	glVertex2f(252, 25);
	glVertex2f(700, -400);
	glEnd();

	//left door
	glColor3f(0.219, 0.113, 0.078);
	glBegin(GL_QUADS);
	glVertex2f(-420, 50);
	glVertex2f(-336, 125);
	glVertex2f(-336, -175);
	glVertex2f(-420, -175);
	glEnd();
	glColor3f(0, 0, 0);
	glBegin(GL_LINE_LOOP);
	glVertex2f(-420, 50);
	glVertex2f(-336, 125);
	glVertex2f(-336, -175);
	glVertex2f(-420, -175);
	glEnd();

	//left door knob - CIRCLE
	glColor3f(0.815, 0.690, 0.305);
	circle(-364, 0, 12);
	glColor3f(0, 0, 0);
	circle_wire(-364, 0, 12);


	//left window rim
	glColor3f(0.047, 0.043, 0.196);
	glBegin(GL_QUADS);
	glVertex2f(-514, 112);
	glVertex2f(-514, -108);
	glVertex2f(-686, -262);
	glVertex2f(-686, -38);
	glEnd();
	glColor3f(0, 0, 0);
	glBegin(GL_LINE_LOOP);
	glVertex2f(-514, 112);
	glVertex2f(-514, -108);
	glVertex2f(-686, -262);
	glVertex2f(-686, -38);
	glEnd();

	//left window glass
	glColor3f(0.729, 0.823, 0.870);
	glBegin(GL_QUADS);
	glVertex2f(-532, 75);
	glVertex2f(-532, -100);
	glVertex2f(-672, -225);
	glVertex2f(-672, -50);
	glEnd();
	glColor3f(0, 0, 0);
	glBegin(GL_LINE_LOOP);
	glVertex2f(-532, 75);
	glVertex2f(-532, -100);
	glVertex2f(-672, -225);
	glVertex2f(-672, -50);
	glEnd();

	//right door1 - QUAD
	glColor3f(0.219, 0.113, 0.078);
	glBegin(GL_QUADS);
	glVertex2f(308, 150);
	glVertex2f(420, 50);
	glVertex2f(420, -150);
	glVertex2f(308, -150);
	glEnd();
	glColor3f(0, 0, 0);
	glBegin(GL_LINE_LOOP);
	glVertex2f(308, 150);
	glVertex2f(420, 50);
	glVertex2f(420, -150);
	glVertex2f(308, -150);
	glEnd();

	//right door1 knob - CIRCLE
	glColor3f(0.815, 0.690, 0.305);
	circle(392, -25, 12);
	glColor3f(0, 0, 0);
	circle_wire(392, -25, 12);

	//right door2 - QUAD
	glColor3f(0.219, 0.113, 0.078);
	glBegin(GL_QUADS);
	glVertex2f(532, -50);
	glVertex2f(616, -125);
	glVertex2f(616, -325);
	glVertex2f(532, -325);
	glEnd();
	glColor3f(0, 0, 0);
	glBegin(GL_LINE_LOOP);
	glVertex2f(532, -50);
	glVertex2f(616, -125);
	glVertex2f(616, -325);
	glVertex2f(532, -325);
	glEnd();

	//right door2 knob - CIRCLE
	glColor3f(0.815, 0.690, 0.305);
	circle(588, -200, 12);
	glColor3f(0, 0, 0);
	circle_wire(588, -200, 12);

	//floor
	glColor3f(0.509, 0.419, 0.250);
	glBegin(GL_QUADS);
	glVertex2f(-252, 25);
	glVertex2f(252, 25);
	glVertex2f(700, -400);
	glVertex2f(-700, -400);
	glEnd();
	glColor3f(0, 0, 0);
	glBegin(GL_LINE_LOOP);
	glVertex2f(-252, 25);
	glVertex2f(252, 25);
	glVertex2f(700, -400);
	glVertex2f(-700, -400);
	glEnd();


	//Middle door Rim
	glColor3f(0.168, 0.090, 0.070);
	glBegin(GL_QUADS);
	glVertex2f(-168, 200);
	glVertex2f(168, 200);
	glVertex2f(168, 25);
	glVertex2f(-168, 25);
	glEnd();
	glColor3f(0, 0, 0);
	glBegin(GL_LINE_LOOP);
	glVertex2f(-168, 200);
	glVertex2f(168, 200);
	glVertex2f(168, 25);
	glVertex2f(-168, 25);
	glEnd();

	//Middle door
	glColor3f(0.074, 0.070, 0.168);
	glBegin(GL_QUADS);
	glVertex2f(-140, 175);
	glVertex2f(140, 175);
	glVertex2f(140, 25);
	glVertex2f(-140, 25);
	glEnd();
	glColor3f(0, 0, 0);
	glBegin(GL_LINE_LOOP);
	glVertex2f(-140, 175);
	glVertex2f(140, 175);
	glVertex2f(140, 25);
	glVertex2f(-140, 25);
	glEnd();

	//Middle door line
	glColor3f(0, 0, 0);
	glBegin(GL_LINE_STRIP);
	glVertex2f(0, 175);
	glVertex2f(0, 25);
	glEnd();

	//Middle door left knob
	glColor3f(0.815, 0.690, 0.305);
	circle(-28, 100, 10);
	glColor3f(0, 0, 0);
	circle_wire(-28, 100, 10);

	//Middle door right knob
	glColor3f(0.815, 0.690, 0.305);
	circle(28, 100, 10);
	glColor3f(0, 0, 0);
	circle_wire(28, 100, 10);

	//"Emergency room" sign
	glColor3f(0.913, 0.537, 0.505);
	glBegin(GL_QUADS);
	glVertex2f(-196, 212);
	glVertex2f(-196, 262);
	glVertex2f(196, 262);
	glVertex2f(196, 212);
	glEnd();
	glColor3f(0, 0, 0);
	glBegin(GL_LINE_LOOP);
	glVertex2f(-196, 212);
	glVertex2f(-196, 262);
	glVertex2f(196, 262);
	glVertex2f(196, 212);
	glEnd();
	glColor3f(0.2, 0.1, 0.2);
	vprint(-120, 230, GLUT_BITMAP_TIMES_ROMAN_24, "EMERGENCY ROOM");

	//Floor line1
	glColor3f(0, 0, 0);
	glBegin(GL_LINE_STRIP);
	glVertex2f(-168, 25);
	glVertex2f(-448, -400);
	glEnd();

	//Floor line2
	glColor3f(0, 0, 0);
	glBegin(GL_LINE_STRIP);
	glVertex2f(-84, 25);
	glVertex2f(-224, -400);
	glEnd();

	//Floor line3
	glColor3f(0, 0, 0);
	glBegin(GL_LINE_STRIP);
	glVertex2f(0, 25);
	glVertex2f(0, -400);
	glEnd();

	//Floor line4
	glColor3f(0, 0, 0);
	glBegin(GL_LINE_STRIP);
	glVertex2f(84, 25);
	glVertex2f(224, -400);
	glEnd();

	//Floor line5
	glColor3f(0, 0, 0);
	glBegin(GL_LINE_STRIP);
	glVertex2f(168, 25);
	glVertex2f(448, -400);
	glEnd();

	//floor horizontal line1
	glColor3f(0, 0, 0);
	glBegin(GL_LINE_STRIP);
	glVertex2f(-672, -375);
	glVertex2f(672, -375);
	glEnd();

	//floor horizontal line2
	glColor3f(0, 0, 0);
	glBegin(GL_LINE_STRIP);
	glVertex2f(-278, 0);
	glVertex2f(278, 0);
	glEnd();

	//floor horizontal line3
	glColor3f(0, 0, 0);
	glBegin(GL_LINE_STRIP);
	glVertex2f(-357, -75);
	glVertex2f(357, -75);
	glEnd();

	//floor horizontal line4
	glColor3f(0, 0, 0);
	glBegin(GL_LINE_STRIP);
	glVertex2f(-462, -175);
	glVertex2f(462, -175);
	glEnd();

	//floor horizontal line5
	glColor3f(0, 0, 0);
	glBegin(GL_LINE_STRIP);
	glVertex2f(-568, -275);
	glVertex2f(568, -275);
	glEnd();

	apple();
	if (showBullet == true)
		dBullet();

}

void drawBackground3()   //function to draw background 3

{
	//Sky
	glColor3f(0.749, 0.105, 0.031);
	glBegin(GL_QUADS);
	glVertex2f(-800, 40);
	glVertex2f(800, 40);
	glVertex2f(800, 800);
	glVertex2f(-800, 800);
	glEnd();
	glColor3f(0, 0, 0);


	//Light green strip of land
	glColor3f(0.407, 0.862, 0.337);
	glBegin(GL_QUADS);
	glVertex2f(-800, -110);
	glVertex2f(800, -110);
	glVertex2f(800, -320);
	glVertex2f(-800, -320);
	glEnd();


	//darker green strip of land
	glColor3f(0.274, 0.643, 0.215);
	glBegin(GL_QUADS);
	glVertex2f(-800, -110);
	glVertex2f(800, -110);
	glVertex2f(800, 0);
	glVertex2f(-800, 0);
	glEnd();

	//darkest green strip of land
	glColor3f(0.219, 0.411, 0.188);
	glBegin(GL_QUADS);
	glVertex2f(-800, 40);
	glVertex2f(800, 40);
	glVertex2f(800, 0);
	glVertex2f(-800, 0);
	glEnd();
	glColor3f(0, 0, 0);
	/*glBegin(GL_LINE_STRIP);
	glVertex2f(-800, 40);
	glVertex2f(800, 40);
	glEnd();*/

	//trees
	//1
	glColor3f(0.607, 0.231, 0.109);
	glBegin(GL_QUADS);
	glVertex2f(-100, 80);
	glVertex2f(-80, 80);
	glVertex2f(-80, 0);
	glVertex2f(-100, 0);
	glEnd();
	glColor3f(0, 0, 0);
	glBegin(GL_LINE_LOOP);
	glVertex2f(-100, 80);
	glVertex2f(-80, 80);
	glVertex2f(-80, 0);
	glVertex2f(-100, 0);
	glEnd();
	glColor3f(0.325, 0.721, 0.098);
	glBegin(GL_TRIANGLES);
	glVertex2f(-120, 60);
	glVertex2f(-60, 60);
	glVertex2f(-90, 160);
	glEnd();
	glBegin(GL_LINE_LOOP);
	glColor3f(0, 0, 0);
	glVertex2f(-120, 60);
	glVertex2f(-60, 60);
	glVertex2f(-90, 160);
	glEnd();

	//2
	glColor3f(0.352, 0.137, 0.047);
	glBegin(GL_QUADS);
	glVertex2f(-360, 80);
	glVertex2f(-340, 80);
	glVertex2f(-340, 0);
	glVertex2f(-360, 0);
	glEnd();
	glColor3f(0, 0, 0);
	glBegin(GL_LINE_LOOP);
	glVertex2f(-360, 80);
	glVertex2f(-340, 80);
	glVertex2f(-340, 0);
	glVertex2f(-360, 0);
	glEnd();
	glColor3f(0.243, 0.505, 0.094);
	glBegin(GL_TRIANGLES);
	glVertex2f(-380, 60);
	glVertex2f(-320, 60);
	glVertex2f(-350, 160);
	glEnd();
	glBegin(GL_LINE_LOOP);
	glColor3f(0, 0, 0);
	glVertex2f(-380, 60);
	glVertex2f(-320, 60);
	glVertex2f(-350, 160);
	glEnd();

	//3
	glColor3f(0.396, 0.219, 0.141);
	glBegin(GL_QUADS);
	glVertex2f(-180, 60);
	glVertex2f(-160, 60);
	glVertex2f(-160, -20);
	glVertex2f(-180, -20);
	glEnd();
	glColor3f(0, 0, 0);
	glBegin(GL_LINE_LOOP);
	glVertex2f(-180, 60);
	glVertex2f(-160, 60);
	glVertex2f(-160, -20);
	glVertex2f(-180, -20);
	glEnd();
	glColor3f(0.207, 0.309, 0.152);
	glBegin(GL_TRIANGLES);
	glVertex2f(-210, 40);
	glVertex2f(-130, 40);
	glVertex2f(-170, 180);
	glEnd();
	glBegin(GL_LINE_LOOP);
	glColor3f(0, 0, 0);
	glVertex2f(-210, 40);
	glVertex2f(-130, 40);
	glVertex2f(-170, 180);
	glEnd();

	//4
	glColor3f(0.647, 0.333, 0.152);
	glBegin(GL_QUADS);
	glVertex2f(-280, 20);
	glVertex2f(-260, 20);
	glVertex2f(-260, -60);
	glVertex2f(-280, -60);
	glEnd();
	glColor3f(0, 0, 0);
	glBegin(GL_LINE_LOOP);
	glVertex2f(-280, 20);
	glVertex2f(-260, 20);
	glVertex2f(-260, -60);
	glVertex2f(-280, -60);
	glEnd();
	glColor3f(0.345, 0.607, 0.192);
	glBegin(GL_TRIANGLES);
	glVertex2f(-310, 0);
	glVertex2f(-230, 0);
	glVertex2f(-270, 120);
	glEnd();
	glBegin(GL_LINE_LOOP);
	glColor3f(0, 0, 0);
	glVertex2f(-310, 0);
	glVertex2f(-230, 0);
	glVertex2f(-270, 120);
	glEnd();


	//5
	glColor3f(0.564, 0.250, 0.074);
	glBegin(GL_QUADS);
	glVertex2f(-220, -20);
	glVertex2f(-200, -20);
	glVertex2f(-200, -100);
	glVertex2f(-220, -100);
	glEnd();
	glColor3f(0, 0, 0);
	glBegin(GL_LINE_LOOP);
	glVertex2f(-220, -20);
	glVertex2f(-200, -20);
	glVertex2f(-200, -100);
	glVertex2f(-220, -100);
	glEnd();
	glColor3f(0.184, 0.623, 0.078);
	glBegin(GL_TRIANGLES);
	glVertex2f(-250, -40);
	glVertex2f(-170, -40);
	glVertex2f(-210, 80);
	glEnd();
	glBegin(GL_LINE_LOOP);
	glColor3f(0, 0, 0);
	glVertex2f(-250, -40);
	glVertex2f(-170, -40);
	glVertex2f(-210, 80);
	glEnd();

	//6
	glColor3f(0.662, 0.313, 0.117);
	glBegin(GL_QUADS);
	glVertex2f(-400, -40);
	glVertex2f(-380, -40);
	glVertex2f(-380, -120);
	glVertex2f(-400, -120);
	glEnd();
	glColor3f(0, 0, 0);
	glBegin(GL_LINE_LOOP);
	glVertex2f(-400, -40);
	glVertex2f(-380, -40);
	glVertex2f(-380, -120);
	glVertex2f(-400, -120);
	glEnd();
	glColor3f(0.203, 0.674, 0.086);
	glBegin(GL_TRIANGLES);
	glVertex2f(-350, -60);
	glVertex2f(-430, -60);
	glVertex2f(-390, 40);
	glEnd();
	glBegin(GL_LINE_LOOP);
	glColor3f(0, 0, 0);
	glVertex2f(-350, -60);
	glVertex2f(-430, -60);
	glVertex2f(-390, 40);
	glEnd();

	//7
	glColor3f(0.768, 0.266, 0.090);
	glBegin(GL_QUADS);
	glVertex2f(-340, -60);
	glVertex2f(-320, -60);
	glVertex2f(-320, -140);
	glVertex2f(-340, -140);
	glEnd();
	glColor3f(0, 0, 0);
	glBegin(GL_LINE_LOOP);
	glVertex2f(-340, -60);
	glVertex2f(-320, -60);
	glVertex2f(-320, -140);
	glVertex2f(-340, -140);
	glEnd();
	glColor3f(0.192, 0.741, 0.156);
	glBegin(GL_TRIANGLES);
	glVertex2f(-380, -80);
	glVertex2f(-280, -80);
	glVertex2f(-330, 60);
	glEnd();
	glBegin(GL_LINE_LOOP);
	glColor3f(0, 0, 0);
	glVertex2f(-380, -80);
	glVertex2f(-280, -80);
	glVertex2f(-330, 60);
	glEnd();

	//8
	glColor3f(0.768, 0.266, 0.090);
	glBegin(GL_QUADS);
	glVertex2f(-140, -80);
	glVertex2f(-120, -80);
	glVertex2f(-120, -160);
	glVertex2f(-140, -160);
	glEnd();
	glColor3f(0, 0, 0);
	glBegin(GL_LINE_LOOP);
	glVertex2f(-140, -80);
	glVertex2f(-120, -80);
	glVertex2f(-120, -160);
	glVertex2f(-140, -160);
	glEnd();
	glColor3f(0.215, 0.690, 0.188);
	glBegin(GL_TRIANGLES);
	glVertex2f(-170, -100);
	glVertex2f(-90, -100);
	glVertex2f(-130, 20);
	glEnd();
	glBegin(GL_LINE_LOOP);
	glColor3f(0, 0, 0);
	glVertex2f(-170, -100);
	glVertex2f(-90, -100);
	glVertex2f(-130, 20);
	glEnd();


	//clouds
	glColor3f(0.95, 0.95, 0.95);
	circle(-200, 260, 30);
	glColor3f(0.95, 0.95, 0.95);
	circle(-250, 260, 30);
	glColor3f(0.95, 0.95, 0.95);
	circle(-220, 300, 30);
	glColor3f(0.95, 0.95, 0.95);
	circle(-170, 290, 30);
	glColor3f(0.95, 0.95, 0.95);
	circle(-180, 240, 40);
	glColor3f(0.95, 0.95, 0.95);
	circle(-140, 270, 20);
	glColor3f(0.95, 0.95, 0.95);
	circle(-60, 260, 40);
	glColor3f(0.95, 0.95, 0.95);
	circle(-110, 260, 30);

	glColor3f(0.95, 0.95, 0.95);
	circle(300, 280, 30);
	glColor3f(0.95, 0.95, 0.95);
	circle(260, 260, 40);
	glColor3f(0.95, 0.95, 0.95);
	circle(240, 300, 40);
	glColor3f(0.95, 0.95, 0.95);
	circle(220, 320, 20);
	glColor3f(0.95, 0.95, 0.95);
	circle(200, 280, 40);
	glColor3f(0.95, 0.95, 0.95);
	circle(180, 300, 20);
	glColor3f(0.95, 0.95, 0.95);
	circle(160, 260, 30);
	glColor3f(0.95, 0.95, 0.95);
	circle(140, 300, 40);
	glColor3f(0.95, 0.95, 0.95);
	circle(120, 280, 40);


	//House
	//main
	glColor3f(0.968, 0.772, 0.631);
	glBegin(GL_QUADS);
	glVertex2f(140, 100);
	glVertex2f(220, 100);
	glVertex2f(220, 0);
	glVertex2f(140, 0);
	glEnd();
	glColor3f(0, 0, 0);
	glBegin(GL_LINE_LOOP);
	glVertex2f(140, 100);
	glVertex2f(220, 100);
	glVertex2f(220, 0);
	glVertex2f(140, 0);
	glEnd();

	glColor3f(1, 0.776, 0.619);
	glBegin(GL_QUADS);
	glVertex2f(380, 100);
	glVertex2f(220, 100);
	glVertex2f(220, 0);
	glVertex2f(380, 0);
	glEnd();
	glColor3f(0, 0, 0);
	glBegin(GL_LINE_LOOP);
	glVertex2f(380, 100);
	glVertex2f(220, 100);
	glVertex2f(220, 0);
	glVertex2f(380, 0);
	glEnd();

	glColor3f(0.792, 0.145, 0.086);
	glBegin(GL_QUADS);
	glVertex2f(180, 160);
	glVertex2f(340, 160);
	glVertex2f(380, 100);
	glVertex2f(220, 100);
	glEnd();
	glColor3f(0, 0, 0);
	glBegin(GL_LINE_LOOP);
	glVertex2f(180, 160);
	glVertex2f(340, 160);
	glVertex2f(380, 100);
	glVertex2f(220, 100);
	glEnd();

	glColor3f(0.925, 0.592, 0.333);
	glBegin(GL_TRIANGLES);
	glVertex2f(180, 160);
	glVertex2f(220, 100);
	glVertex2f(140, 100);
	glEnd();
	glColor3f(0, 0, 0);
	glBegin(GL_LINE_LOOP);
	glVertex2f(180, 160);
	glVertex2f(220, 100);
	glVertex2f(140, 100);
	glEnd();
	//windows
	//1
	glColor3f(0.729, 0.968, 0.952);
	glBegin(GL_QUADS);
	glVertex2f(240, 80);
	glVertex2f(280, 80);
	glVertex2f(280, 60);
	glVertex2f(240, 60);
	glEnd();
	glColor3f(0, 0, 0);
	glBegin(GL_LINE_LOOP);
	glVertex2f(240, 80);
	glVertex2f(280, 80);
	glVertex2f(280, 60);
	glVertex2f(240, 60);
	glEnd();
	glColor3f(0, 0, 0);
	glBegin(GL_LINE_STRIP);
	glVertex2f(240, 70);
	glVertex2f(280, 70);
	glEnd();
	glBegin(GL_LINE_STRIP);
	glVertex2f(260, 60);
	glVertex2f(260, 80);
	glEnd();

	//2
	glColor3f(0.729, 0.968, 0.952);
	glBegin(GL_QUADS);
	glVertex2f(320, 80);
	glVertex2f(360, 80);
	glVertex2f(360, 60);
	glVertex2f(320, 60);
	glEnd();
	glColor3f(0, 0, 0);
	glBegin(GL_LINE_LOOP);
	glVertex2f(320, 80);
	glVertex2f(360, 80);
	glVertex2f(360, 60);
	glVertex2f(320, 60);
	glEnd();
	glColor3f(0, 0, 0);
	glBegin(GL_LINE_STRIP);
	glVertex2f(320, 70);
	glVertex2f(360, 70);
	glEnd();
	glBegin(GL_LINE_STRIP);
	glVertex2f(340, 60);
	glVertex2f(340, 80);
	glEnd();

	//round window
	glColor3f(0.729, 0.968, 0.952);
	circle(180, 120, 10);
	glColor3f(0, 0, 0);
	circle_wire(180, 120, 10);

	//door
	glColor3f(0.533, 0.219, 0.086);
	glBegin(GL_QUADS);
	glVertex2f(165, 0);
	glVertex2f(165, 60);
	glVertex2f(195, 60);
	glVertex2f(195, 0);
	glEnd();
	glColor3f(0, 0, 0);
	glBegin(GL_LINE_LOOP);
	glVertex2f(165, 0);
	glVertex2f(165, 60);
	glVertex2f(195, 60);
	glVertex2f(195, 0);
	glEnd();

	//house fence
	glColor3f(0.921, 0.803, 0.517);
	glBegin(GL_QUADS);
	glVertex2f(60, 40);
	glVertex2f(800, 40);
	glVertex2f(800, 25);
	glVertex2f(60, 25);
	glEnd();
	glColor3f(0, 0, 0);
	glBegin(GL_LINE_LOOP);
	glVertex2f(60, 40);
	glVertex2f(800, 40);
	glVertex2f(800, 25);
	glVertex2f(60, 25);
	glEnd();

	glColor3f(0.921, 0.803, 0.517);
	glBegin(GL_POLYGON);
	glVertex2f(60, 0);
	glVertex2f(60, 40);
	glVertex2f(70, 60);
	glVertex2f(80, 40);
	glVertex2f(80, 0);
	glEnd();
	glColor3f(0, 0, 0);
	glBegin(GL_LINE_LOOP);
	glVertex2f(60, 0);
	glVertex2f(60, 40);
	glVertex2f(70, 60);
	glVertex2f(80, 40);
	glVertex2f(80, 0);
	glEnd();

	glColor3f(0.921, 0.803, 0.517);
	glBegin(GL_POLYGON);
	glVertex2f(100, 0);
	glVertex2f(100, 40);
	glVertex2f(110, 60);
	glVertex2f(120, 40);
	glVertex2f(120, 0);
	glEnd();
	glColor3f(0, 0, 0);
	glBegin(GL_LINE_LOOP);
	glVertex2f(100, 0);
	glVertex2f(100, 40);
	glVertex2f(110, 60);
	glVertex2f(120, 40);
	glVertex2f(120, 0);
	glEnd();

	glColor3f(0.921, 0.803, 0.517);
	glBegin(GL_POLYGON);
	glVertex2f(140, 0);
	glVertex2f(140, 40);
	glVertex2f(150, 60);
	glVertex2f(160, 40);
	glVertex2f(160, 0);
	glEnd();
	glColor3f(0, 0, 0);
	glBegin(GL_LINE_LOOP);
	glVertex2f(140, 0);
	glVertex2f(140, 40);
	glVertex2f(150, 60);
	glVertex2f(160, 40);
	glVertex2f(160, 0);
	glEnd();

	glColor3f(0.921, 0.803, 0.517);
	glBegin(GL_POLYGON);
	glVertex2f(180, 0);
	glVertex2f(180, 40);
	glVertex2f(190, 60);
	glVertex2f(200, 40);
	glVertex2f(200, 0);
	glEnd();
	glColor3f(0, 0, 0);
	glBegin(GL_LINE_LOOP);
	glVertex2f(180, 0);
	glVertex2f(180, 40);
	glVertex2f(190, 60);
	glVertex2f(200, 40);
	glVertex2f(200, 0);
	glEnd();

	glColor3f(0.921, 0.803, 0.517);
	glBegin(GL_POLYGON);
	glVertex2f(220, 0);
	glVertex2f(220, 40);
	glVertex2f(230, 60);
	glVertex2f(240, 40);
	glVertex2f(240, 0);
	glEnd();
	glColor3f(0, 0, 0);
	glBegin(GL_LINE_LOOP);
	glVertex2f(220, 0);
	glVertex2f(220, 40);
	glVertex2f(230, 60);
	glVertex2f(240, 40);
	glVertex2f(240, 0);
	glEnd();

	glColor3f(0.921, 0.803, 0.517);
	glBegin(GL_POLYGON);
	glVertex2f(260, 0);
	glVertex2f(260, 40);
	glVertex2f(270, 60);
	glVertex2f(280, 40);
	glVertex2f(280, 0);
	glEnd();
	glColor3f(0, 0, 0);
	glBegin(GL_LINE_LOOP);
	glVertex2f(260, 0);
	glVertex2f(260, 40);
	glVertex2f(270, 60);
	glVertex2f(280, 40);
	glVertex2f(280, 0);
	glEnd();

	glColor3f(0.921, 0.803, 0.517);
	glBegin(GL_POLYGON);
	glVertex2f(300, 0);
	glVertex2f(300, 40);
	glVertex2f(310, 60);
	glVertex2f(320, 40);
	glVertex2f(320, 0);
	glEnd();
	glColor3f(0, 0, 0);
	glBegin(GL_LINE_LOOP);
	glVertex2f(300, 0);
	glVertex2f(300, 40);
	glVertex2f(310, 60);
	glVertex2f(320, 40);
	glVertex2f(320, 0);
	glEnd();

	glColor3f(0.921, 0.803, 0.517);
	glBegin(GL_POLYGON);
	glVertex2f(340, 0);
	glVertex2f(340, 40);
	glVertex2f(350, 60);
	glVertex2f(360, 40);
	glVertex2f(360, 0);
	glEnd();
	glColor3f(0, 0, 0);
	glBegin(GL_LINE_LOOP);
	glVertex2f(340, 0);
	glVertex2f(340, 40);
	glVertex2f(350, 60);
	glVertex2f(360, 40);
	glVertex2f(360, 0);
	glEnd();

	glColor3f(0.921, 0.803, 0.517);
	glBegin(GL_POLYGON);
	glVertex2f(380, 0);
	glVertex2f(380, 40);
	glVertex2f(390, 60);
	glVertex2f(400, 40);
	glVertex2f(400, 0);
	glEnd();
	glColor3f(0, 0, 0);
	glBegin(GL_LINE_LOOP);
	glVertex2f(380, 0);
	glVertex2f(380, 40);
	glVertex2f(390, 60);
	glVertex2f(400, 40);
	glVertex2f(400, 0);
	glEnd();


	glColor3f(0.921, 0.803, 0.517);
	glBegin(GL_POLYGON);
	glVertex2f(420, 0);
	glVertex2f(420, 40);
	glVertex2f(430, 60);
	glVertex2f(440, 40);
	glVertex2f(440, 0);
	glEnd();
	glColor3f(0, 0, 0);
	glBegin(GL_LINE_LOOP);
	glVertex2f(420, 0);
	glVertex2f(420, 40);
	glVertex2f(430, 60);
	glVertex2f(440, 40);
	glVertex2f(440, 0);
	glEnd();

	glColor3f(0.921, 0.803, 0.517);
	glBegin(GL_POLYGON);
	glVertex2f(460, 0);
	glVertex2f(460, 40);
	glVertex2f(470, 60);
	glVertex2f(480, 40);
	glVertex2f(480, 0);
	glEnd();
	glColor3f(0, 0, 0);
	glBegin(GL_LINE_LOOP);
	glVertex2f(460, 0);
	glVertex2f(460, 40);
	glVertex2f(470, 60);
	glVertex2f(480, 40);
	glVertex2f(480, 0);
	glEnd();

	glColor3f(0.921, 0.803, 0.517);
	glBegin(GL_POLYGON);
	glVertex2f(420, 0);
	glVertex2f(420, 40);
	glVertex2f(430, 60);
	glVertex2f(440, 40);
	glVertex2f(440, 0);
	glEnd();
	glColor3f(0, 0, 0);
	glBegin(GL_LINE_LOOP);
	glVertex2f(420, 0);
	glVertex2f(420, 40);
	glVertex2f(430, 60);
	glVertex2f(440, 40);
	glVertex2f(440, 0);
	glEnd();

	glColor3f(0.921, 0.803, 0.517);
	glBegin(GL_POLYGON);
	glVertex2f(500, 0);
	glVertex2f(500, 40);
	glVertex2f(510, 60);
	glVertex2f(520, 40);
	glVertex2f(520, 0);
	glEnd();
	glColor3f(0, 0, 0);
	glBegin(GL_LINE_LOOP);
	glVertex2f(500, 0);
	glVertex2f(500, 40);
	glVertex2f(510, 60);
	glVertex2f(520, 40);
	glVertex2f(520, 0);
	glEnd();


	sun();
	if (showBullet == true)
		rocket();
}



void HowToPlay()   //function that draws how to play gamestate
{
	glColor3f(0.4, 0.239, 0.286);
	glRectf(-800, 500, 800, -500);

	glColor3f(0.713, 0.580, 0.447);
	glRectf(-650, 350, 600, -250);

	glColor3f(0.713, 0.490, 0.447);
	glRectf(-600, 300, 550, -200);

	glLineWidth(5);
	glColor3f(0, 0, 0);
	vprint2(-190, 100, 0.4, "HOW TO PLAY");
	glLineWidth(1);
	vprint(-520, 50, GLUT_BITMAP_HELVETICA_18, "Click anywhere on the screen to add and object that will move towards the light source and try to attack it. Click again to add ");
	vprint(-350, 0, GLUT_BITMAP_HELVETICA_18, "the object on screen after it disappears. Press p to pause the game, u to unpause.");
	vprint(-410, -50, GLUT_BITMAP_HELVETICA_18, "Use space bar to toggle between backgrounds. if you get tired, simply press x to end the game :)");
	vprint(-150, -150, GLUT_BITMAP_HELVETICA_18, "PRESS S TO START THE GAME!");


}

void PauseScreen()    //function that makes pausecreen gamestate
{

	if (gameState == PAUSESCREEN || gameState == PAUSESCREEN2 || gameState == PAUSESCREEN3)
	{
		glColor3f(0, 0, 0);
		glLineWidth(10);
		vprint2(-550, 5, 1.2, "GAME PAUSED");
		glLineWidth(1);
	}
}

void GameoverScreen()  //function that makes gameover gamestate
{
	if (gameState == GAMEOVER)
	{
		glColor3f(0.713, 0.447, 0.592);
		glRectf(1000, -1000, -1000, 1000);
		glColor3ub(0, 0, 0);

		glColor3f(0, 0, 0);
		glLineWidth(7);
		vprint2(-510, 20, 0.63, "THANKYOU FOR PLAYING");
		glLineWidth(5);
		vprint2(-310, -80, 0.63, ":) !");
		glLineWidth(1);

	}
}

void nametag()
{
	//function to draw Name and ID tag
	glColor3f(0.8, 0.741, 0.741);
	glRectf(-((winWidth / 2) - 10), (winHeight / 2) - 8, -((winWidth / 2) - 235), (winHeight / 2) - 55);
	glColor3ub(0, 0, 0);
	glBegin(GL_LINE_LOOP);
	glVertex2f(-((winWidth / 2) - 10), (winHeight / 2) - 8);
	glVertex2f(-((winWidth / 2) - 10), (winHeight / 2) - 55);
	glVertex2f(-((winWidth / 2) - 235), (winHeight / 2) - 55);
	glVertex2f(-((winWidth / 2) - 235), (winHeight / 2) - 8);
	glEnd();
	glColor3f(0.2, 0.1, 0.2);
	vprint(-((winWidth / 2) - 20), (winHeight / 2) - 32.5, GLUT_BITMAP_TIMES_ROMAN_24, "SUHEERA TANVIR");
	vprint(-((winWidth / 2) - 20), (winHeight / 2) - 50, GLUT_BITMAP_HELVETICA_18, "            21801211");
}


void display() {

	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	switch (gameState) {
	case HOWTOPLAY: HowToPlay(); break;
	case BACKGROUND: drawBackground();
		break;
	case BACKGROUND2: drawBackground2();
		break;
	case BACKGROUND3: drawBackground3();
		break;
	case PAUSESCREEN: drawBackground(),
		PauseScreen();
		break;
	case PAUSESCREEN2: drawBackground2(),
		PauseScreen();
		break;
	case PAUSESCREEN3: drawBackground3(),
		PauseScreen();
		break;
	case GAMEOVER: GameoverScreen();
		break;
	}

	nametag();

	if (gameState == BACKGROUND || gameState == BACKGROUND2 || gameState == BACKGROUND3)
	{
		glColor3f(light.color.r, light.color.g, light.color.b);

		double y = -450;
		for (int x = -550; x <= 550; x++)
		{


			vertex_t P = { { x, -100}, { 0, 1 } };

			color_t res = { 0, 0, 0 };

			res = addColor(res, calculateColor(light, P));


			glBegin(GL_LINES);
			glColor3f(res.r, res.g, res.b);
			//glVertex2f(x, -375);
			glVertex2f(x, y + 300);

			glColor3f(0.109, 0.062, 0.050);
			//glVertex2f(x, -375);
			glVertex2f(x, y);
			glEnd();
			y -= 0.1;
		}
	}


	glutSwapBuffers();
}


void onKeyDown(unsigned char key, int x, int y)
{
	// exit when ESC is pressed.
	if (key == 27)
		exit(0);

	if (key == 's' || key == 'S' && gameState == HOWTOPLAY)
		gameState = BACKGROUND;

	//toggling between backgrounds
	if ((key == ' ') && gameState == BACKGROUND)
		gameState = BACKGROUND2;

	else if ((key == ' ') && gameState == BACKGROUND2)
		gameState = BACKGROUND3;

	else if ((key == ' ') && gameState == BACKGROUND3)
		gameState = BACKGROUND;


	//pause and unpause gamestates
	if ((key == 'p' || key == 'P') && gameState == BACKGROUND)
		gameState = PAUSESCREEN;

	if ((key == 'u' || key == 'U') && gameState == PAUSESCREEN)
		gameState = BACKGROUND;

	if ((key == 'p' || key == 'P') && gameState == BACKGROUND2)
		gameState = PAUSESCREEN2;

	if ((key == 'u' || key == 'U') && gameState == PAUSESCREEN2)
		gameState = BACKGROUND2;

	if ((key == 'p' || key == 'P') && gameState == BACKGROUND3)
		gameState = PAUSESCREEN3;

	if ((key == 'u' || key == 'U') && gameState == PAUSESCREEN3)
		gameState = BACKGROUND3;


	//gameover gamestate
	if ((key == 'x' || key == 'X') && gameState != HOWTOPLAY)
		gameState = GAMEOVER;

	glutPostRedisplay();
}

void onKeyUp(unsigned char key, int x, int y)
{
	if (key == 27)
		exit(0);
	glutPostRedisplay();
}


void onSpecialKeyDown(int key, int x, int y)
{

	switch (key) {
	case GLUT_KEY_UP: up = true;
		break;
	case GLUT_KEY_DOWN: down = true;
		break;
	case GLUT_KEY_LEFT: left = true;
		break;
	case GLUT_KEY_RIGHT: right = true;
		break;
	}

	glutPostRedisplay();
}


void onSpecialKeyUp(int key, int x, int y)
{

	switch (key) {
	case GLUT_KEY_UP: up = false; break;
	case GLUT_KEY_DOWN: down = false; break;
	case GLUT_KEY_LEFT: left = false; break;
	case GLUT_KEY_RIGHT: right = false; break;
	}


	glutPostRedisplay();
}


void onClick(int button, int stat, int x, int y)
{
	//add bullet to the screen at the place where you click IF it is not already on the screen
	if (button == GLUT_LEFT_BUTTON && stat == GLUT_DOWN && showBullet == false)
	{

		bullet.pos.x = x - winWidth / 2;
		bullet.pos.y = winHeight / 2 - y;
		showBullet = true;
		return;
	}
	glutPostRedisplay();
}

void onResize(int w, int h)
{
	winWidth = w;
	winHeight = h;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-w / 2, w / 2, -h / 2, h / 2, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	display();
}

void onMoveDown(int x, int y) {

	glutPostRedisplay();
}

void onMove(int x, int y) {

	glutPostRedisplay();
}

#if TIMER_ON == 1
void onTimer(int v) {

	glutTimerFunc(TIMER_PERIOD, onTimer, 0);

	//only show obejects and animation if the game is NOT paused
	if (gameState != PAUSESCREEN && gameState != PAUSESCREEN2 && gameState != PAUSESCREEN3)
	{

		//checks if light source is within screen boundaries
		if (light.pos.x < winWidth / 2 + 60 && light.pos.x > -(winWidth / 2 + 60) && upT == true)
		{

			//light source movement
			light.pos.x += 1.5;
			light.pos.y -= 0.5;

			//if light source goes out of boundaries
			if (light.pos.x > winWidth / 2 + 30)
			{
				//shift light source coordinates to the beginning of the screen, at the bottom
				light.pos.x = -470;
				light.pos.y = -110;
				upT = false;
			}


		}
		//same thing but instead of moving from up to down, the light source will now move from down to up
		else if (light.pos.x < winWidth / 2 + 60 && light.pos.x > -(winWidth / 2 + 60) && upT == false)
		{

			light.pos.x += 1.5;
			light.pos.y += 0.5;
			if (light.pos.x > winWidth / 2 + 30)
			{
				//shift light source coordinates to the beginning of the screen, at the top
				light.pos.x = -470;
				light.pos.y = 300;
				upT = true;
			}

		}

		//bullet will chase the light source
		bullet.vel = mulV(1.8, unitV(subV(light.pos, bullet.pos)));
		bullet.pos = addV(bullet.pos, bullet.vel);


		//if bullet reaches center of light source, it will disappear
		if (bullet.pos.x >= light.pos.x - 1 && bullet.pos.x <= light.pos.x + 1 && bullet.pos.y >= light.pos.y - 1 && bullet.pos.y <= light.pos.y + 1)
			showBullet = false;

	}

	glutPostRedisplay();

}
#endif

void Init() {


	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

void main(int argc, char *argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("Realistic(ish) light source chased by an aggressive object - SUHEERA TANVIR");

	glutDisplayFunc(display);
	glutReshapeFunc(onResize);


	glutKeyboardFunc(onKeyDown);
	glutSpecialFunc(onSpecialKeyDown);

	glutKeyboardUpFunc(onKeyUp);
	glutSpecialUpFunc(onSpecialKeyUp);


	glutMouseFunc(onClick);
	glutMotionFunc(onMoveDown);
	glutPassiveMotionFunc(onMove);

#if  TIMER_ON == 1

	glutTimerFunc(TIMER_PERIOD, onTimer, 0);
#endif

	Init();

	glutMainLoop();
}