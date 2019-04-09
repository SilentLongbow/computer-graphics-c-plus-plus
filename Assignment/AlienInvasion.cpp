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

GLuint txId[8];   //Texture ids
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
    glGenTextures(8, txId);

    // --------- CASTLE WALL ---------
    glBindTexture(GL_TEXTURE_2D, txId[0]);
    loadTGA("textures/wall.tga");
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

    // --------- COBBLESTONES ---------
    glBindTexture(GL_TEXTURE_2D, txId[1]);  //Use this texture
    loadTGA("textures/floor.tga");
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

    // --------- LEFT ---------
    glBindTexture(GL_TEXTURE_2D, txId[2]);
    loadTGA("textures/skybox/left.tga");
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // --------- BACK ---------
    glBindTexture(GL_TEXTURE_2D, txId[3]);
    loadTGA("textures/skybox/back.tga");
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // --------- RIGHT ---------
    glBindTexture(GL_TEXTURE_2D, txId[4]);
    loadTGA("textures/skybox/right.tga");
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // --------- FRONT ---------
    glBindTexture(GL_TEXTURE_2D, txId[5]);
    loadTGA("textures/skybox/front.tga");
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // --------- TOP ---------
    glBindTexture(GL_TEXTURE_2D, txId[6]);
    loadTGA("textures/skybox/top.tga");
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // --------- BOTTOM ---------
    glBindTexture(GL_TEXTURE_2D, txId[7]);
    loadTGA("textures/skybox/bottom.tga");
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Set texture environment - Use modulate which multiplies texture colour with fragment's colour
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
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
    glBindTexture(GL_TEXTURE_2D, txId[2]);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);     glVertex3f(-100,  -100, 100);
    glTexCoord2f(1, 0);     glVertex3f(-100, -100, -100);
    glTexCoord2f(1, 1);     glVertex3f(-100, 100, -100);
    glTexCoord2f(0, 1);     glVertex3f(-100, 100, 100);
    glEnd();

    ////////////////////// REAR WALL ////////////////////////
    glBindTexture(GL_TEXTURE_2D, txId[3]);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);     glVertex3f( 100, -100, 100);
    glTexCoord2f(1, 0);     glVertex3f(-100, -100,  100);
    glTexCoord2f(1, 1);     glVertex3f(-100, 100,  100);
    glTexCoord2f(0, 1);     glVertex3f( 100, 100, 100);
    glEnd();

    ////////////////////// RIGHT WALL ///////////////////////
    glBindTexture(GL_TEXTURE_2D, txId[4]);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);     glVertex3f(100,  -100, -100);
    glTexCoord2f(1, 0);     glVertex3f(100, -100, 100);
    glTexCoord2f(1, 1);     glVertex3f(100, 100,  100);
    glTexCoord2f(0, 1);     glVertex3f(100,  100,  -100);
    glEnd();


    ////////////////////// FRONT WALL ///////////////////////
    glBindTexture(GL_TEXTURE_2D, txId[5]);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);     glVertex3f(-100,  -100, -100);
    glTexCoord2f(1, 0);     glVertex3f(100, -100, -100);
    glTexCoord2f(1, 1);     glVertex3f(100, 100, -100);
    glTexCoord2f(0, 1);     glVertex3f(-100,  100, -100);
    glEnd();

    /////////////////////// TOP //////////////////////////
    glBindTexture(GL_TEXTURE_2D, txId[6]);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0);     glVertex3f(-100, 100, -100);
    glTexCoord2f(1.0, 0.0);     glVertex3f(100, 100,  -100);
    glTexCoord2f(1.0, 1.0);     glVertex3f(100, 100,  100);
    glTexCoord2f(0.0, 1.0);     glVertex3f(-100, 100, 100);
    glEnd();

    /////////////////////// BOTTOM //////////////////////////
    glBindTexture(GL_TEXTURE_2D, txId[7]);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);     glVertex3f(-100, -100, 100);
    glTexCoord2f(1, 0);     glVertex3f(100, -100,  100);
    glTexCoord2f(1, 1);     glVertex3f(100, -100, -100);
    glTexCoord2f(0, 1);     glVertex3f(-100, -100, -100);
    glEnd();

    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}

//--------------------------------------------------------------------------------

void floor()
{


    glMaterialfv(GL_FRONT, GL_SPECULAR, black); // Remove the specular color of floor
    glBindTexture(GL_TEXTURE_2D, txId[1]);
    glColor3d(1.0, 1.0, 1.0);  //replace with a texture
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

}

//--------------------------------------------------------------------------------

void wall(float length)
{
    glBindTexture(GL_TEXTURE_2D, txId[0]);
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

    ////////////////////// RIGHT WALL ///////////////////////
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

    ////////////////////// LEFT WALL ///////////////////////
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
    glBindTexture(GL_TEXTURE_2D, txId[0]);

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

    glPushMatrix();
        glTranslatef(15.5, 0.0, 0.0);
        glRotatef(90, 0.0, 1.0, 0.0);
        wall(28);
    glPopMatrix();

    glPushMatrix();
        glTranslatef(-15.5, 0.0, 0.0);
        glRotatef(90, 0.0, 1.0, 0.0);
        wall(28);
    glPopMatrix();

    glPushMatrix();
        glTranslatef(-7.5, 0.0, 15.5);
        wall(13);
    glPopMatrix();

    glPushMatrix();
        glTranslatef(7.5, 0.0, 15.5);
        wall(13);
    glPopMatrix();

    glPushMatrix();
        glTranslatef(0.0, 1.0, 15.5);
        entryOverhead();
    glPopMatrix();
}

//--------------------------------------------------------------------------------

void merlon()
{

    glBindTexture(GL_TEXTURE_2D, txId[0]);
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

    glBindTexture(GL_TEXTURE_2D, txId[0]);
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
    castleWalls();
    if (render) castleTowers();


    glutPostRedisplay();
    glutSwapBuffers();
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
        eye_x += 0.2 * sin(angle);
        eye_z -= 0.2 * cos (angle);
    } else if (key == GLUT_KEY_DOWN)
    {
        // Move backwards
        eye_x -= 0.2 * sin(angle);
        eye_z += 0.2 * cos(angle);
    }

    // Calculate lookat position
    look_x = eye_x + 100 * sin(angle);
    look_y = eye_y;
    look_z = eye_z - 100 * cos(angle);
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
