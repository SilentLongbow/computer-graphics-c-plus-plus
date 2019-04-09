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
float black[4] = {0.0, 0.0, 0.0, 1.0};
float grey[4] = {0.2, 0.2, 0.2, 1.0};   // Ambient value
float white[4]  = {1.0, 1.0, 1.0, 1.0}; // Specular and diffuse value
float sunPos[4];
bool render = false;

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
    glPushMatrix();
        glTranslatef(15.5, 0.0, 15.5);
        tower();
    glPopMatrix();

    glPushMatrix();
        glTranslatef(-15.5, 0.0, 15.5);
        tower();
    glPopMatrix();


    glPushMatrix();
        glTranslatef(-15.5, 0.0, -15.5);
        tower();
    glPopMatrix();


    glPushMatrix();
        glTranslatef(15.5, 0.0, -15.5);
        tower();
    glPopMatrix();


}
//--------------------------------------------------------------------------------

void ship()
{
    // Vertices go from bottom to top
    int N = 26;
    float angStep = 10.0*3.1415926/180.0;  //Rotate in 10 deg steps (converted to radians)
    float vx[N] = {0.1, 0.1, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.5, 0.4, 0.4, 0.4, 0.4, 0.4, 0.4,
                   0.4, 0.4, 0.4, 0.38, 0.35, 0.32, 0.3, 0.25, 0.2, 0.1, 0.0};
    float vy[N] = {0.0, 0.1, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9,
                  1.0, 1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7, 1.8, 1.9, 2.0};
    float vz[N] = {0};

    float wx[N];
    float wy[N];
    float wz[N];

    glColor3f(0.9, 0.0, 0.0);
    for (int j = 0; j < 36; j++) {
        glBegin(GL_TRIANGLE_STRIP);
        for(int i = 0; i < N; i++) {
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
    ship();

    glutPostRedisplay();
    glutSwapBuffers();
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
        if (eye_y > 0) eye_y -= 0.1;
    } else if (key == 'r')
    {
        // render towers
        render = !render;
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
        angle -= 0.1;
    } else if (key == GLUT_KEY_RIGHT)
    {
        // look left
        angle += 0.1;
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

    glutMainLoop();
    return 0;
}
