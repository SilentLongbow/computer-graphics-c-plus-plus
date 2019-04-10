//  ========================================================================
//  COSC 363 Assignment 1: Alien Invasion
//
//  FILE NAME: AlienInvasion.cpp
//  Author: Matthew Kenny
//  ========================================================================

#include <iostream>
#include <stdio.h>
#include <cmath>
#include <GL/glut.h>
#include "textures/loadTGA.h"
using namespace std;

#define GL_CLAMP_TO_EDGE 0x812F // To remove seams between skybox textures

GLuint txId[9];   //Texture ids
float look_x, look_y, look_z; // Point that camera looks at. Values calcualed in special
float eye_x = 0, eye_y = 0, eye_z = 0;  // Position of camera

float angle = M_PI;
float strutAngle = 6.0; // The angle that the undercarriage strut is at against the y-axix
float footAngle = 0.0; // The angle of the undercarriage foot against the z-axis
float undercarriageAngle = 0.0; // Angle of the total undercarriage, used to fold it into the ship
bool renderFoot = true;
bool renderUndercarriage = true;

bool takeOff = false;
float shipHeight = 0.0;
float ship_z = 0.0;
float shipAngle = 0.0; // Angle of rotation of the ship around the y-axis
bool renderShip = true;

float radarAngle = 0.0; // Angle of rotation of the radar system around the y-axis

float bulbAngle = 0.0; // The angle of revolution of the bulbs on the ship around the y-axis

int bulbTick = 0; // Counter to determine when the switch the bulb positions

float black[4] = {0.0, 0.0, 0.0, 1.0};
float grey[4] = {0.2, 0.2, 0.2, 1.0};   // Ambient value
float white[4]  = {1.0, 1.0, 1.0, 1.0}; // Specular and diffuse value

float sunPos[4];

GLUquadricObj* q;

//--------------------------------------------------------------------------------

void loadTexture()
{
    glGenTextures(9, txId);

    // --------- LEFT SKYBOX ---------
    glBindTexture(GL_TEXTURE_2D, txId[0]);
    loadTGA("textures/skybox/left.tga");
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // --------- BACK SKYBOX ---------
    glBindTexture(GL_TEXTURE_2D, txId[1]);
    loadTGA("textures/skybox/back.tga");
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // --------- RIGHT SKYBOX ---------
    glBindTexture(GL_TEXTURE_2D, txId[2]);
    loadTGA("textures/skybox/right.tga");
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // --------- FRONT SKYBOX ---------
    glBindTexture(GL_TEXTURE_2D, txId[3]);
    loadTGA("textures/skybox/front.tga");
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // --------- TOP SKYBOX ---------
    glBindTexture(GL_TEXTURE_2D, txId[4]);
    loadTGA("textures/skybox/top.tga");
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // --------- BOTTOM SKYBOX ---------
    glBindTexture(GL_TEXTURE_2D, txId[5]);
    loadTGA("textures/skybox/bottom.tga");
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // --------- CASTLE WALL ---------
    glBindTexture(GL_TEXTURE_2D, txId[6]);
    loadTGA("textures/castle/wall.tga");
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

    // --------- COBBLESTONES ---------
    glBindTexture(GL_TEXTURE_2D, txId[7]);  //Use this texture
    loadTGA("textures/castle/floor.tga");
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, txId[8]);
    loadTGA("textures/outside/snow.tga");
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

    // Set texture environment - Use modulate which multiplies texture colour with fragment's colour
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}

//--------------------------------------------------------------------------------

void normal(float x1, float y1, float z1,
            float x2, float y2, float z2,
              float x3, float y3, float z3 )
{
      float nx, ny, nz;
      nx = y1*(z2-z3)+ y2*(z3-z1)+ y3*(z1-z2);
      ny = z1*(x2-x3)+ z2*(x3-x1)+ z3*(x1-x2);
      nz = x1*(y2-y3)+ x2*(y3-y1)+ x3*(y1-y2);

      glNormal3f(nx, ny, nz);
}

//--------------------------------------------------------------------------------

void initialise()
{

    loadTexture();
    q =  gluNewQuadric();

    glEnable(GL_TEXTURE_2D);

    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE); // Use the colour values given for each object
    glEnable(GL_COLOR_MATERIAL); // Enable the ambient and diffuse line above

    glMaterialfv(GL_FRONT, GL_SPECULAR, white); // SPECULAR colour
    glMaterialf(GL_FRONT, GL_SHININESS, 50); // SHININESS value

    glEnable(GL_DEPTH_TEST); // Only show objects if view is not obstructed by others
    glEnable(GL_NORMALIZE); // Normalise normal values given

    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_AMBIENT, grey);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
    glLightfv(GL_LIGHT0, GL_SPECULAR, white);

    glClearColor(0.0, 0.0, 0.0, 0.0); // Set background colour as white with 0 opacity

    // Set up camera projection values (fov, near, far, etc.)
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(80.0, 1, 0.1, 5000.0); // Set aspect ratio of 16:9

    // Set up look values for x & y
    look_x = eye_x + 100 * sin(angle);
    look_z = eye_z - 100 * cos(angle);
}

//--------------------------------------------------------------------------------

void skybox()
{
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    ////////////////////// LEFT WALL ///////////////////////
    glBindTexture(GL_TEXTURE_2D, txId[0]);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);     glVertex3f(-200,  -200, 200);
    glTexCoord2f(1, 0);     glVertex3f(-200, -200, -200);
    glTexCoord2f(1, 1);     glVertex3f(-200, 200, -200);
    glTexCoord2f(0, 1);     glVertex3f(-200, 200, 200);
    glEnd();

    ////////////////////// REAR WALL ////////////////////////
    glBindTexture(GL_TEXTURE_2D, txId[1]);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);     glVertex3f( 200, -200, 200);
    glTexCoord2f(1, 0);     glVertex3f(-200, -200,  200);
    glTexCoord2f(1, 1);     glVertex3f(-200, 200,  200);
    glTexCoord2f(0, 1);     glVertex3f( 200, 200, 200);
    glEnd();

    ////////////////////// RIGHT WALL ///////////////////////
    glBindTexture(GL_TEXTURE_2D, txId[2]);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);     glVertex3f(200,  -200, -200);
    glTexCoord2f(1, 0);     glVertex3f(200, -200, 200);
    glTexCoord2f(1, 1);     glVertex3f(200, 200,  200);
    glTexCoord2f(0, 1);     glVertex3f(200,  200,  -200);
    glEnd();


    ////////////////////// FRONT WALL ///////////////////////
    glBindTexture(GL_TEXTURE_2D, txId[3]);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);     glVertex3f(-200,  -200, -200);
    glTexCoord2f(1, 0);     glVertex3f(200, -200, -200);
    glTexCoord2f(1, 1);     glVertex3f(200, 200, -200);
    glTexCoord2f(0, 1);     glVertex3f(-200,  200, -200);
    glEnd();

    /////////////////////// TOP //////////////////////////
    glBindTexture(GL_TEXTURE_2D, txId[4]);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0);     glVertex3f(-200, 200, -200);
    glTexCoord2f(1.0, 0.0);     glVertex3f(200, 200,  -200);
    glTexCoord2f(1.0, 1.0);     glVertex3f(200, 200,  200);
    glTexCoord2f(0.0, 1.0);     glVertex3f(-200, 200, 200);
    glEnd();

    /////////////////////// BOTTOM //////////////////////////
    glBindTexture(GL_TEXTURE_2D, txId[5]);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);     glVertex3f(-200, -200, 200);
    glTexCoord2f(1, 0);     glVertex3f(200, -200,  200);
    glTexCoord2f(1, 1);     glVertex3f(200, -200, -200);
    glTexCoord2f(0, 1);     glVertex3f(-200, -200, -200);
    glEnd();

    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}

//--------------------------------------------------------------------------------

void floor()
{


    glMaterialfv(GL_FRONT, GL_SPECULAR, black); // Remove the specular color of floor
    glBindTexture(GL_TEXTURE_2D, txId[7]);
    glColor3f(1.0, 1.0, 1.0);  //replace with a texture
    glBegin(GL_QUADS);
    glNormal3f(0.0, 1.0, 0.0);
    for(int i = -16; i < 16; i++)
    {
        for(int j = -16;  j < 16; j++)
        {
            glTexCoord2f(0.0, 0.0);     glVertex3f(i, -1, j);
            glTexCoord2f(0.0, 1.0);     glVertex3f(i, -1, j+1);
            glTexCoord2f(1.0, 1.0);     glVertex3f(i+1, -1, j+1);
            glTexCoord2f(1.0, 0.0);     glVertex3f(i+1, -1, j);
        }
    }
    glEnd();

    glMaterialfv(GL_FRONT, GL_SPECULAR, white); // Restore specular colour for other objects
}

//--------------------------------------------------------------------------------

void ground()
{
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glBindTexture(GL_TEXTURE_2D, txId[8]);
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_QUADS);
    glNormal3f(0.0, 1.0, 0.0);

    for (float x = -150; x < 150; x++)
    {
        for (float z = -150; z < 150; z++)
        {
            bool withinCastle = (x > -15 && x < 15) && (z > -15 && z < 15);
            if (!withinCastle)
            {
                glTexCoord2f(0.0, 0.0);     glVertex3f(x, -1, z);
                glTexCoord2f(0.0, 1.0);     glVertex3f(x, -1, z+1);
                glTexCoord2f(1.0, 1.0);     glVertex3f(x+1, -1, z+1);
                glTexCoord2f(1.0, 0.0);     glVertex3f(x+1, -1, z);
            }
        }
    }
    glEnd();
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}

//--------------------------------------------------------------------------------

void wall(float length)
{
    glBindTexture(GL_TEXTURE_2D, txId[6]);
    glColor3f(1.0, 1.0, 1.0);

    ////////////////////// BACK OF WALL ///////////////////////
    glBegin(GL_QUADS);
    glNormal3f(0.0, 0.0, -1.0);
    for (float x = -length / 2; x < length / 2; x++)
    {
        for (float y = -1; y < 2; y++)
        {
            glTexCoord2f(0.0, 0.0);     glVertex3f(x, y, -0.5);
            glTexCoord2f(0.0, 1.0);     glVertex3f(x, y+1.0, -0.5);
            glTexCoord2f(1.0, 1.0);     glVertex3f(x+1.0, y+1.0, -0.5);
            glTexCoord2f(1.0, 0.0);     glVertex3f(x+1.0, y, -0.5);
        }
    }
    glEnd();

    ////////////////////// FRONT OF WALL ///////////////////////
    glBegin(GL_QUADS);
    glNormal3f(0.0, 0.0, 1.0);
    for (float x = -length / 2; x < length / 2; x++)
    {
        for (float y = -1; y < 2; y++)
        {
            glTexCoord2f(0.0, 0.0);     glVertex3f(x, y, 0.5);
            glTexCoord2f(0.0, 1.0);     glVertex3f(x, y+1.0, 0.5);
            glTexCoord2f(1.0, 1.0);     glVertex3f(x+1.0, y+1.0, 0.5);
            glTexCoord2f(1.0, 0.0);     glVertex3f(x+1.0, y, 0.5);
        }
    }
    glEnd();

    ////////////////////// TOP OF WALL ///////////////////////
    glBegin(GL_QUADS);
    glNormal3f(0.0, 1.0, 0.0);
    for (float x = -length / 2; x < length / 2; x++)
    {
        glTexCoord2f(0.0, 0.0);     glVertex3f(x, 2, -0.5);
        glTexCoord2f(0.0, 1.0);     glVertex3f(x, 2, 0.5);
        glTexCoord2f(1.0, 1.0);     glVertex3f(x+1.0, 2, 0.5);
        glTexCoord2f(1.0, 0.0);     glVertex3f(x+1.0, 2, -0.5);
    }
    glEnd();

    ////////////////////// RIGHT OF WALL ///////////////////////
    glBegin(GL_QUADS);
    glNormal3f(-1.0, 0.0, 0.0);
    for (float y = -1; y < 2; y++)
    {
        glTexCoord2f(0.0, 0.0);     glVertex3f(-length / 2, y, -0.5);
        glTexCoord2f(0.0, 1.0);     glVertex3f(-length / 2, y+1, -0.5);
        glTexCoord2f(1.0, 1.0);     glVertex3f(-length / 2, y+1, 0.5);
        glTexCoord2f(1.0, 0.0);     glVertex3f(-length / 2, y, 0.5);
    }
    glEnd();

    ////////////////////// LEFT OF WALL ///////////////////////
    glBegin(GL_QUADS);
    glNormal3f(1.0, 0.0, 0.0);
    for (float y = -1; y < 2; y++)
    {
        glTexCoord2f(0.0, 0.0);     glVertex3f(length / 2, y, -0.5);
        glTexCoord2f(0.0, 1.0);     glVertex3f(length / 2, y+1, -0.5);
        glTexCoord2f(1.0, 1.0);     glVertex3f(length / 2, y+1, 0.5);
        glTexCoord2f(1.0, 0.0);     glVertex3f(length / 2, y, 0.5);
    }
    glEnd();
}

//--------------------------------------------------------------------------------

void entryOverhead()
{
    glBindTexture(GL_TEXTURE_2D, txId[6]);

    ////////////////////// BACK OF OVERHEAD ///////////////////////
    glBegin(GL_QUADS);
    glNormal3f(0.0, 0.0, -1.0);
    for (float x = -1; x < 1; x++)
    {
        glTexCoord2f(0.0, 0.0);     glVertex3f(x, 0, -0.5);
        glTexCoord2f(0.0, 1.0);     glVertex3f(x, 1.0, -0.5);
        glTexCoord2f(1.0, 1.0);     glVertex3f(x+1, 1.0, -0.5);
        glTexCoord2f(1.0, 0.0);     glVertex3f(x+1, 0, -0.5);
    }
    glEnd();

    ////////////////////// FRONT OF OVERHEAD ///////////////////////
    glBegin(GL_QUADS);
    glNormal3f(0.0, 0.0, 1.0);
    for (float x = -1; x < 1; x++)
    {
        glTexCoord2f(0.0, 0.0);     glVertex3f(x, 0, 0.5);
        glTexCoord2f(0.0, 1.0);     glVertex3f(x, 1.0, 0.5);
        glTexCoord2f(1.0, 1.0);     glVertex3f(x+1, 1.0, 0.5);
        glTexCoord2f(1.0, 0.0);     glVertex3f(x+1, 0, 0.5);
    }
    glEnd();

    ////////////////////// TOP OF OVERHEAD ///////////////////////
    glBegin(GL_QUADS);
    glNormal3f(0.0, 1.0, 0.0);
    for (float x = -1; x < 1; x++)
    {
        glTexCoord2f(0.0, 0.0);     glVertex3f(x, 1.0, -0.5);
        glTexCoord2f(0.0, 1.0);     glVertex3f(x, 1.0, 0.5);
        glTexCoord2f(1.0, 1.0);     glVertex3f(x+1, 1.0, 0.5);
        glTexCoord2f(1.0, 0.0);     glVertex3f(x+1, 1.0, -0.5);
    }
    glEnd();

    ////////////////////// BOTTOM OF OVERHEAD ///////////////////////
    glBegin(GL_QUADS);
    glNormal3f(0.0, 1.0, 0.0);
    for (float x = -1; x < 1; x++)
    {
        glTexCoord2f(0.0, 0.0);     glVertex3f(x, 0, -0.5);
        glTexCoord2f(0.0, 1.0);     glVertex3f(x, 0, 0.5);
        glTexCoord2f(1.0, 1.0);     glVertex3f(x+1, 0, 0.5);
        glTexCoord2f(1.0, 0.0);     glVertex3f(x+1, 0, -0.5);
    }
    glEnd();
}

//--------------------------------------------------------------------------------

void castleWalls()
{
    // Back wall
    glPushMatrix();
        glTranslatef(0.0, 0.0, -15.5);
        wall(28);
    glPopMatrix();

    // Left wall
    glPushMatrix();
        glTranslatef(15.5, 0.0, 0.0);
        glRotatef(90, 0.0, 1.0, 0.0);
        wall(28);
    glPopMatrix();

    // Right wall
    glPushMatrix();
        glTranslatef(-15.5, 0.0, 0.0);
        glRotatef(90, 0.0, 1.0, 0.0);
        wall(28);
    glPopMatrix();

    // Front right wall
    glPushMatrix();
        glTranslatef(-7.5, 0.0, 15.5);
        wall(13);
    glPopMatrix();

    // Front left wall
    glPushMatrix();
        glTranslatef(7.5, 0.0, 15.5);
        wall(13);
    glPopMatrix();

    // Overhead
    glPushMatrix();
        glTranslatef(0.0, 1.0, 15.5);
        entryOverhead();
    glPopMatrix();
}

//--------------------------------------------------------------------------------

void merlon()
{

    glBindTexture(GL_TEXTURE_2D, txId[6]);
    glColor3f(1.0, 1.0, 1.0);

    ////////////////////// BACK MERLON SECTION ///////////////////////
    glBegin(GL_QUADS);
    glNormal3f(0.0, 0.0, -1.0);
    glTexCoord2f(0.0, 0.0);     glVertex3f(-0.5, 0.0, -0.5);
    glTexCoord2f(0.0, 1.0);     glVertex3f(-0.5, 1.0, -0.5);
    glTexCoord2f(1.0, 1.0);     glVertex3f(0.5, 1.0, -0.5);
    glTexCoord2f(1.0, 0.0);     glVertex3f(0.5, 0.0, -0.5);
    glEnd();

    ////////////////////// FRONT MERLON SECTION ///////////////////////
    glBegin(GL_QUADS);
    glNormal3f(0.0, 0.0, 1.0);
    glTexCoord2f(0.0, 0.0);     glVertex3f(-0.5, 0.0, 0.5);
    glTexCoord2f(0.0, 1.0);     glVertex3f(-0.5, 1.0, 0.5);
    glTexCoord2f(1.0, 1.0);     glVertex3f(0.5, 1.0, 0.5);
    glTexCoord2f(1.0, 0.0);     glVertex3f(0.5, 0.0, 0.5);
    glEnd();

    ////////////////////// LEFT MERLON SECTION ///////////////////////
    glBegin(GL_QUADS);
    glNormal3f(1.0, 0.0, 0.0);
    glTexCoord2f(0.0, 0.0);     glVertex3f(0.5, 0.0, -0.5);
    glTexCoord2f(0.0, 1.0);     glVertex3f(0.5, 1.0, -0.5);
    glTexCoord2f(1.0, 1.0);     glVertex3f(0.5, 1.0, 0.5);
    glTexCoord2f(1.0, 0.0);     glVertex3f(0.5, 0.0, 0.5);
    glEnd();

    ////////////////////// RIGHT MERLON SECTION ///////////////////////
    glBegin(GL_QUADS);
    glNormal3f(-1.0, 0.0, 0.0);
    glTexCoord2f(0.0, 0.0);     glVertex3f(-0.5, 0.0, -0.5);
    glTexCoord2f(0.0, 1.0);     glVertex3f(-0.5, 1.0, -0.5);
    glTexCoord2f(1.0, 1.0);     glVertex3f(-0.5, 1.0, 0.5);
    glTexCoord2f(1.0, 0.0);     glVertex3f(-0.5, 0.0, 0.5);
    glEnd();

    ////////////////////// TOP MERLON SECTION ///////////////////////
    glBegin(GL_QUADS);
    glNormal3f(0.0, 1.0, 0.0);
    glTexCoord2f(0.0, 0.0);     glVertex3f(-0.5, 1.0, -0.5);
    glTexCoord2f(0.0, 1.0);     glVertex3f(-0.5, 1.0, 0.5);
    glTexCoord2f(1.0, 1.0);     glVertex3f(0.5, 1.0, 0.5);
    glTexCoord2f(1.0, 0.0);     glVertex3f(0.5, 1.0, -0.5);
    glEnd();
}

//--------------------------------------------------------------------------------

void battlement()
{
    glPushMatrix();
        glTranslatef(1.0, 4.0, 1.0);
        merlon();
    glPopMatrix();

    glPushMatrix();
        glTranslatef(1.0, 4.0, -1.0);
        merlon();
    glPopMatrix();

    glPushMatrix();
        glTranslatef(-1.0, 4.0, 1.0);
        merlon();
    glPopMatrix();

    glPushMatrix();
        glTranslatef(-1.0, 4.0, -1.0);
        merlon();
    glPopMatrix();
}

//--------------------------------------------------------------------------------

void tower()
{
    float x;
    float y;
    float z;

    glColor3f(1.0, 1.0, 1.0);

    glBindTexture(GL_TEXTURE_2D, txId[6]);
    ////////////////////// BACK TOWER SECTION ///////////////////////
    z = -1.5;
    glBegin(GL_QUADS);
    glNormal3f(0.0, 0.0, -1.0);
    for (x = -1.5; x < 1.5; x++) {
        for (y = -1; y < 4; y++) {
            glTexCoord2f(0.0, 0.0);     glVertex3f(x, y, z);
            glTexCoord2f(0.0, 1.0);     glVertex3f(x, y+1, z);
            glTexCoord2f(1.0, 1.0);     glVertex3f(x+1, y+1, z);
            glTexCoord2f(1.0, 0.0);     glVertex3f(x+1, y, z);
        }
    }
    glEnd();

    ////////////////////// RIGHT TOWER SECTION ///////////////////////
    x = 1.5;
    glBegin(GL_QUADS);
    glNormal3f(1.0, 0.0, 0.0);
    for (z = -1.5; z < 1.5; z++) {
        for (y = -1; y < 4; y++) {
            glTexCoord2f(0.0, 0.0);     glVertex3f(x, y, z);
            glTexCoord2f(0.0, 1.0);     glVertex3f(x, y+1, z);
            glTexCoord2f(1.0, 1.0);     glVertex3f(x, y+1, z+1);
            glTexCoord2f(1.0, 0.0);     glVertex3f(x, y, z+1);
        }
    }
    glEnd();

    ////////////////////// LEFT TOWER SECTION ///////////////////////
    x = -1.5;
    glBegin(GL_QUADS);
    glNormal3f(-1.0, 0.0, 0.0);
    for (z = -1.5; z < 1.5; z++) {
        for (y = -1; y < 4; y++) {
            glTexCoord2f(0.0, 0.0);     glVertex3f(x, y, z);
            glTexCoord2f(0.0, 1.0);     glVertex3f(x, y+1, z);
            glTexCoord2f(1.0, 1.0);     glVertex3f(x, y+1, z+1);
            glTexCoord2f(1.0, 0.0);     glVertex3f(x, y, z+1);
        }
    }
    glEnd();

    ////////////////////// FRONT TOWER SECTION ///////////////////////
    z = 1.5;
    glBegin(GL_QUADS);
    glNormal3f(0.0, 0.0, 1.0);
    for (x = -1.5; x < 1.5; x++) {
        for (y = -1; y < 4; y++) {
            glTexCoord2f(0.0, 0.0);     glVertex3f(x, y, z);
            glTexCoord2f(0.0, 1.0);     glVertex3f(x, y+1, z);
            glTexCoord2f(1.0, 1.0);     glVertex3f(x+1, y+1, z);
            glTexCoord2f(1.0, 0.0);     glVertex3f(x+1, y, z);
        }
    }
    glEnd();


    ////////////////////// TOP TOWER SECTION ///////////////////////
    y = 4;
    glBegin(GL_QUADS);
    glNormal3f(0.0, 1.0, 0.0);
    for (x = -1.5; x < 1.5; x++) {
        for (z = -1.5; z < 1.5; z++) {
            glTexCoord2f(0.0, 0.0);     glVertex3f(x, y, z);
            glTexCoord2f(0.0, 1.0);     glVertex3f(x, y, z+1);
            glTexCoord2f(1.0, 1.0);     glVertex3f(x+1, y, z+1);
            glTexCoord2f(1.0, 0.0);     glVertex3f(x+1, y, z);
        }
    }
    glEnd();

    battlement();
}

//--------------------------------------------------------------------------------

void castleTowers()
{

    ////////////////////// FRONT RIGHT TOWER ///////////////////////
    glPushMatrix();
        glTranslatef(15.5, 0.0, 15.5);
        tower();
    glPopMatrix();

    ////////////////////// FRONT LEFT TOWER ///////////////////////
    glPushMatrix();
        glTranslatef(-15.5, 0.0, 15.5);
        tower();
    glPopMatrix();

    ////////////////////// BACK LEFT TOWER ///////////////////////
    glPushMatrix();
        glTranslatef(-15.5, 0.0, -15.5);
        tower();
    glPopMatrix();

    ////////////////////// BACK RIGHT TOWER ///////////////////////
    glPushMatrix();
        glTranslatef(15.5, 0.0, -15.5);
        tower();
    glPopMatrix();


}
//--------------------------------------------------------------------------------

void radarAntenna()
{
    glDisable(GL_TEXTURE_2D);
    glColor3f(35.0/255.0, 64.0/255.0, 152.0/255.0);
    gluCylinder(q, 0.1, 0.1, 1, 10, 10);
    glPushMatrix();

        ////////////////////// TOP SENSOR ///////////////////////
        glScalef(1.0, 4.0, 1.0);

        glColor3f(204.0/255.0, 152.0/255.0, 20.0/255.0);
        glMaterialfv(GL_FRONT, GL_SPECULAR, black); // Make top matte
        glutSolidCube(0.2);
        glMaterialfv(GL_FRONT, GL_SPECULAR, white); // Restore specular colour

    glPopMatrix();
    glEnable(GL_TEXTURE_2D);
}

//--------------------------------------------------------------------------------

void sensors()
{
    glPushMatrix();
        glRotatef(radarAngle, 0.0, 1.0, 0.0);
        glTranslatef(0.0, -0.7, 0.0);
        glRotatef(-90, 1.0, 0.0, 0.0);
        radarAntenna();
    glPopMatrix();
}

//--------------------------------------------------------------------------------

void shipHull()
{
    // Vertices go from bottom to top
    int N = 59;
    float angStep = 2.5 * M_PI / 180.0;  //Rotate in 5 deg steps (converted to radians)

    float vx[N] = {0.0, 0.05, 0.1, 0.175, 0.22, 0.26, 0.297, 0.3, 0.3, 0.3, 0.34, 0.4, 0.45, 0.5, 0.6, 0.7,
                   0.8, 0.92, 1.05, 1.1, 1.22, 1.28, 1.38, 1.5, 1.6, 1.7, 1.76, 1.805, 1.836, 1.836,
                   1.79, 1.7, 1.6, 1.5, 1.4, 1.3, 1.2, 1.1, 1.0, 0.9, 0.8, 0.76, 0.7, 0.6, 0.5, 0.46, 0.46,
                   0.45, 0.44, 0.43, 0.4, 0.35, 0.3, 0.28, 0.2, 0.15 ,0.1 ,0.05, 0.0};
    float vy[N] = {0.0, 0.01, 0.02, 0.04, 0.06, 0.08, 0.11, 0.13, 0.15, 0.15, 0.16, 0.175, 0.178, 0.19, 0.2,
                   0.22, 0.24, 0.25, 0.258, 0.27, 0.28, 0.285, 0.29, 0.3, 0.31, 0.315, 0.32, 0.34, 0.35,
                   0.37, 0.387, 0.395, 0.4, 0.4, 0.4, 0.4, 0.405, 0.41, 0.42, 0.43, 0.445, 0.45, 0.46,
                   0.48, 0.49, 0.5, 0.5, 0.55, 0.6, 0.62, 0.633, 0.675, 0.7, 0.72, 0.75, 0.76, 0.787, 0.795, 0.8};
    float vz[N] = {0};

    float wx[N];
    float wy[N];
    float wz[N];

    float ufoGrey = 45.0/255.0;
    float ufoSilerDisk = 211.0/255.0;
    float ufoSilverBridge = 125.0/255.0;
    glDisable(GL_TEXTURE_2D);
    for (int j = 0; j < 144; j++) {
        glBegin(GL_TRIANGLE_STRIP);
        for(int i = 0; i < N; i++) {
            if (i < 9)
            {
                glMaterialfv(GL_FRONT, GL_SPECULAR, black); // SPECULAR colour
                glColor3f(ufoGrey, ufoGrey, ufoGrey); // Life support grey
            }
            else if (i < 47)
            {
                glMaterialfv(GL_FRONT, GL_SPECULAR, white); // SPECULAR colour
                glColor3f(ufoSilerDisk, ufoSilerDisk, ufoSilerDisk); // Silver for disk
            }
            else glColor3f(ufoSilverBridge, ufoSilverBridge, ufoSilverBridge); // Silver for bridge

            wx[i] = (vx[i] * cos(angStep)) + (vz[i] * sin(angStep));
            wy[i] = vy[i];
            wz[i] = (-vx[i] * sin(angStep)) + (vz[i] * cos(angStep));
            if (i > 0) normal (vx[i], vy[i], vz[i], vx[i-1], vy[i-1], vz[i-1], wx[i-1], wy[i-1], wz[i-1]);
            //if (i > 1) normal (vx[i], vy[i], vz[i], vx[i-2], vy[i-2], vz[i-2], wx[i-1], wy[i-1], wz[i-1]);
            /*glTexCoord2f(j / 36.0, i / (float)N);*/    glVertex3f(vx[i], vy[i], vz[i]);
            if (i > 0) normal (wx[i], wy[i], wz[i], vx[i], vy[i], vz[i], wx[i-1], wy[i-1], wz[i-1]);
            glVertex3f(wx[i], wy[i], wz[i]);
        }
        for (int i = 0; i < N; i++) {
            vx[i] = wx[i];
            vy[i] = wy[i];
            vz[i] = wz[i];
        }
        glEnd();
    }
    glEnable(GL_TEXTURE_2D);
}
//--------------------------------------------------------------------------------

void landingStrut()
{
    float strutLength = 1.45;

    glPushMatrix();
        glTranslatef(0.0, -0.27 + 0.8, -1.5);
        glRotatef(undercarriageAngle, 1.0, 0.0, 0.0);
        glTranslatef(0.0, -0.8, 0.0);

        if (renderFoot)
        {
            // Set up the foot
            glPushMatrix();
            glTranslatef(0.0, -0.68, -0.3);
            glRotatef(-35, 1.0, 0.0, 0.0);
            glScalef(0.1, 0.1, 0.4);
            glutSolidCube(1);
            glPopMatrix();
        }

        // Set up the landing gear strut
        glPushMatrix();
            glTranslatef(0.0, 0.8, 0.0);
            glRotatef(strutAngle + 90, 1.0, 0.0, 0.0);
            glScalef(0.1, 0.1, strutLength);
            gluCylinder(q, 0.5, 0.5, 1.0, 20, 20);
        glPopMatrix();
    glPopMatrix();
}

//--------------------------------------------------------------------------------

void shipUndercarriage()
{
    landingStrut();
    glPushMatrix();
        glRotatef(120, 0.0, 1.0, 0.0);
        landingStrut();
    glPopMatrix();
    glPushMatrix();
        glRotatef(240, 0.0, 1.0, 0.0);
        landingStrut();
    glPopMatrix();
}

//--------------------------------------------------------------------------------

void redBulb()
{

    glColor3f(1.0, 0.0, 0.0);
    glPushMatrix();
        glTranslatef(0.0, 0.0, 1.0);
        gluSphere(q, 0.05, 20, 20);
    glPopMatrix();

}

//--------------------------------------------------------------------------------

void blueBulb()
{

    glColor3f(0.0, 1.0, 1.0);
    glPushMatrix();
        glRotatef(45, 0.0, 1.0, 0.0);
        glTranslatef(0.0, 0.0, 1.0);
        gluSphere(q, 0.05, 20, 20);
    glPopMatrix();


}

//--------------------------------------------------------------------------------

void shipBulbs()
{
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);

    ////////////////////// TOP RED BULB ///////////////////////
    glPushMatrix();
        glRotatef(bulbAngle, 0.0, 1.0, 0.0);
        redBulb();
    glPopMatrix();

    ////////////////////// RIGHT RED BULB ///////////////////////
    glPushMatrix();
        glRotatef(bulbAngle + 90, 0.0, 1.0, 0.0);
        redBulb();
    glPopMatrix();

    ////////////////////// BOTTOM RED BULB ///////////////////////
    glPushMatrix();
        glRotatef(bulbAngle + 180, 0.0, 1.0, 0.0);
        redBulb();
    glPopMatrix();

    ////////////////////// LEFT RED BULB ///////////////////////
    glPushMatrix();
        glRotatef(bulbAngle - 90, 0.0, 1.0, 0.0);
        redBulb();
    glPopMatrix();

    ////////////////////// FRONT LEFT BLUE BULB ///////////////////////
    glPushMatrix();
        glRotatef(bulbAngle, 0.0, 1.0, 0.0);
        blueBulb();
    glPopMatrix();

    ////////////////////// FRONT RIGHT BLUE BULB ///////////////////////
    glPushMatrix();
        glRotatef(bulbAngle + 90, 0.0, 1.0, 0.0);
        blueBulb();
    glPopMatrix();

    ////////////////////// BACK RIGHT BLUE BULB ///////////////////////
    glPushMatrix();
        glRotatef(bulbAngle + 180, 0.0, 1.0, 0.0);
        blueBulb();
    glPopMatrix();

    ////////////////////// BACK LEFT BLUE BULB ///////////////////////
    glPushMatrix();
        glRotatef(bulbAngle - 90, 0.0, 1.0, 0.0);
        blueBulb();
    glPopMatrix();

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
}

//--------------------------------------------------------------------------------


void ship()
{
    glPushMatrix();
        glTranslatef(0.0, shipHeight, ship_z); // Move whole unit
        glRotatef(shipAngle, 0.0, 1.0, 0.0); // Rotate ship

        // Flashing Bulbs on top
        glPushMatrix();
            glTranslatef(0.0, 0.85, 0.0);
            glScalef(2.0, 2.0, 2.0);
            shipBulbs();
        glPopMatrix();

        // Flashing Bulbs on bottom
        glPushMatrix();
            glTranslatef(0.0, 0.378, 0.0);
            shipBulbs();
        glPopMatrix();

        // Generate hull
        glPushMatrix();
            glScalef(2.0, 2.0, 2.0);
            shipHull();
        glPopMatrix();
    if (renderUndercarriage) shipUndercarriage(); // Undercarriage
    sensors(); // Sensors
    glPopMatrix();
}

//--------------------------------------------------------------------------------

void display()
{
    float sunPos[4] = {-4.5, 4.5, -20.0, 0.0}; // Vector from origin towards lightbox sun
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(eye_x, eye_y, eye_z,  look_x, look_y, look_z,  0, 1, 0);

    glEnable(GL_LIGHTING); // Let there be light!
    glLightfv(GL_LIGHT0, GL_POSITION, sunPos); // Create the 'sun' - Position is normalised

    glColor3f(1.0, 1.0, 0.0);

    skybox();
    floor();
    ground();
    castleWalls();
    castleTowers();
    if (renderShip) ship();
    glutPostRedisplay();
    glutSwapBuffers();
}

//--------------------------------------------------------------------------------

void timerFunction(int value)
{
    bulbTick += 1;
    if (bulbTick >= 19) { // Alternates bulbs at approximately 0.5s
        bulbAngle += 45.0;
        if (bulbAngle == 360.0) bulbAngle = 0.0;
        bulbTick = 0;
    }

    radarAngle += 10;
    if (radarAngle > 359.0) radarAngle = 0.0;

    if (renderShip && takeOff) {
        // Slowly rise to 3 units height to raise gear
        if (shipHeight < 4)
        {
            shipHeight += 0.01;
        }

        // Rotate ship once off ground
        if (shipHeight > 0.5)
        {
            shipAngle += 1;
            if (shipAngle > 359.0) shipAngle = 0.0;
        }

        // Rotate landing foot
        if (footAngle > -36)
        {
            footAngle -= 0.5;
        }

        // If foot at edge of movement limits, stop rendering it
        if (undercarriageAngle > 82)
        {
            renderFoot = false;
        }

        // Rotate undercarriage
        if (undercarriageAngle < 90)
        {
            undercarriageAngle += 0.5;
        }
        // Gear up, start moving
        else if (shipHeight < 30.0)
        {
            shipHeight += 0.1;
        }

        if (undercarriageAngle >= 90) {
            renderUndercarriage = false;
        }

        // If at correct elevation, start moving forwards!
        if (shipHeight >= 30.0)
        {
            ship_z += 0.5;
        }

        // When ship past skybox, stop rendering it
        if (ship_z > 210)
        {
            renderShip = false;
        }


    }
    glutPostRedisplay();
    glutTimerFunc(25, timerFunction, 0);
}

//--------------------------------------------------------------------------------

void worldCollision()
{
    if (eye_x > 125.0) {
        eye_x = 125.0;
    }
    if (eye_x < -125.0) {
        eye_x = -125.0;
    }
    if (eye_z > 125.0) {
        eye_z = 125.0;
    }
    if (eye_z < -125.0) {
        eye_z = -125.0;
    }
}

//--------------------------------------------------------------------------------

void wallCollision(float* next_x, float* next_z)
{
    if (*next_x > 14.8 && *next_x < 16.2 && *next_z > -14.2 && *next_z < 14.2)  // Left wall
    {
        *next_x = eye_x; // x-direction is blocked so set to 0
    }
    else if (*next_x > -16.2 && *next_x < -14.8 && *next_z > -14.2 && *next_z < 14.2) // Right wall
    {
        *next_x = eye_x; // x-direction is blocked so set to 0
    }
    else if (*next_x > -14.2 && *next_x < 14.2 && *next_z > 14.8 && *next_z < 16.2) // Front wall
    {
        if (eye_z > 14.8 && eye_z < 16.2 && (*next_x > 0.8 || *next_x < -0.8)) // If inside gate walking into wall
        {
            *next_x = eye_x; // x-direction is blocked so set to 0
        }
        else if (*next_x < -0.8 || *next_x > 0.8) // If not in the gate
        {
            *next_z = eye_z; // z-direction is blocked so set to 0
        }
    }
    else if (*next_x > -14.2 && *next_x < 14.2 && *next_z > -16.2 && *next_z < -14.8) // Rear wall
    {
        *next_z = eye_z; // z-direction is blocked so set to 0
    }
}

//--------------------------------------------------------------------------------

bool towerCollision(float* next_x, float* next_z)
{
    if (*next_x > 13.8 && *next_x < 17.2 && *next_z > 13.8 && *next_z < 17.2) // Front Left tower
    {
        if (eye_x <= 13.8 && *next_x > 13.8)
        {
            *next_x = eye_x;
        }
        else if (eye_x >= 17.2 && *next_x < 17.2)
        {
            *next_x = eye_x;
        }
        if (eye_z <= 13.8 && *next_z > 13.8)
        {
            *next_z = eye_z;
        }
        else if (eye_z >= 17.2 && *next_z < 17.2)
        {
            *next_z = eye_z;
        }

    } else if (*next_x > -17.2 && *next_x < -13.8 && *next_z > 13.8 && *next_z < 17.2) // Front Right wall
    {
        if (eye_x <= -17.2 && *next_x > -17.2)
        {
            *next_x = eye_x;
        }
        else if (eye_x >= -13.8 && *next_x < -13.8)
        {
            *next_x = eye_x;
        }
        if (eye_z <= 13.8 && *next_z > 13.8)
        {
            *next_z = eye_z;
        }
        else if (eye_z >= 17.2 && *next_z < 17.2)
        {
            *next_z = eye_z;
        }
    } else if (*next_x > -17.2 && *next_x < -13.8 && *next_z > -17.2 && *next_z < -13.8) // Back Right wall
    {
        if (eye_x <= -17.2 && *next_x > -17.2)
        {
            *next_x = eye_x;
        }
        else if (eye_x >= -13.8 && *next_x < -13.8)
        {
            *next_x = eye_x;
        }
        if (eye_z <= -17.2 && *next_z > -17.2)
        {
            *next_z = eye_z;
        }
        else if (eye_z >= -13.8 && *next_z < -13.8)
        {
            *next_z = eye_z;
        }
    } else if (*next_x > 13.8 && *next_x < 17.2 && *next_z > -17.2 && *next_z < -13.8) // Back Left wall
    {
        if (eye_x <= 13.8 && *next_x > 13.8)
        {
            *next_x = eye_x;
        }
        else if (eye_x >= 17.2 && *next_x < 17.2)
        {
            *next_x = eye_x;
        }
        if (eye_z <= -17.2 && *next_z > -17.2)
        {
            *next_z = eye_z;
        }
        else if (eye_z >= -13.8 && *next_z < -13.8)
        {
            *next_z = eye_z;
        }
    }
}

//--------------------------------------------------------------------------------

void keyboard(unsigned char key, int x, int y)
{
    if (key == ' ')
    {
        // go up
        eye_y += 0.1;
    } else if (key == 'c')
    {
        // go down
        eye_y -= 0.1;
    } else if (key == 's')
    {
        takeOff = true;
    }
    look_x = eye_x + 100 * sin(angle);
    look_y = eye_y;
    look_z = eye_z - 100 * cos(angle);
    glutPostRedisplay();
}

//--------------------------------------------------------------------------------

void special(int key, int x, int y)
{
    float next_x;
    float next_z;
    if (key == GLUT_KEY_LEFT)
    {
        // Look right
        angle -= 5 * M_PI / 180; // 5 Degrees
    } else if (key == GLUT_KEY_RIGHT)
    {
        // look left
        angle += 5 * M_PI / 180; // 5 Degrees
    }
    else if (key == GLUT_KEY_UP)
    {
        // Move forwards
        next_x = eye_x + 0.2 * sin(angle);
        next_z = eye_z - 0.2 * cos(angle);
        towerCollision(&next_x, &next_z); // Tower collision calculations
        wallCollision(&next_x, &next_z); // Wall collision will change values of next_x and next_z if needed
        eye_x = next_x;
        eye_z = next_z;
    } else if (key == GLUT_KEY_DOWN)
    {
        // Move backwards
        next_x = eye_x - 0.2 * sin(angle);
        next_z = eye_z + 0.2 * cos(angle);
        towerCollision(&next_x, &next_z);
        wallCollision(&next_x, &next_z);
        eye_x = next_x;
        eye_z = next_z;
    }

    // Calculate lookat position
    look_x = eye_x + 100 * sin(angle);
    look_y = eye_y;
    look_z = eye_z - 100 * cos(angle);
    worldCollision();
    glutPostRedisplay();
}

//--------------------------------------------------------------------------------

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);

    glutInitWindowSize(1000, 1000);
    glutInitWindowPosition(50, 50);

    glutCreateWindow("Alien Invasion");
    initialise();
    glutDisplayFunc(display);
    glutSpecialFunc(special);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(25, timerFunction, 0);

    glutMainLoop();
    return 0;
}
