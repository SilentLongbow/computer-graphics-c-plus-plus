//  ========================================================================
//  COSC363: Computer Graphics (2019)
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

GLuint txId[2];   //Texture ids
float angle=0, look_x, look_z=-1., eye_x, eye_z;  //Camera parameters

//--------------------------------------------------------------------------------
void loadTexture()
{
    glGenTextures(2, txId); 	// Create 2 texture ids

    glBindTexture(GL_TEXTURE_2D, txId[0]);  //Use this texture
    loadTGA("textures/castle-wall.tga");
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);	//Set texture parameters
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, txId[1]);  //Use this texture
    loadTGA("textures/castle-floor.tga");
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);	//Set texture parameters
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

    glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
}

//--------------------------------------------------------------------------------

/*void detectCollision() {
    if (eye_x > 14.0 && eye_z > 14.0) {
        eye_x = 14.0;
        eye_z = 14.0;
    } else if (eye_x > 14.0 && eye_z < -14.0) {
        eye_x = 14.0;
        eye_z = -14.0;
    } else if (eye_x < -14.0 && eye_z > 14.0) {
        eye_x = -14.0;
        eye_z = 14.0;
    } else if (eye_x < -14.0 && eye_z < -14.0) {
        eye_x = -14.0;
        eye_z = -14.0;
    }else if (eye_x > 14.0) {
        eye_x = 14.0;
    } else if (eye_x < -14.0) {
        eye_x = -14.0;
    } else if (eye_z > 14.0) {
        eye_z = 14.0;
    } else if (eye_z < -14.0) {
        eye_z = -14.0;
    }
}*/

//--------------------------------------------------------------------------------
void special(int key, int x, int y)
{
    if(key == GLUT_KEY_LEFT) angle -= 0.1;  //Change direction
    else if(key == GLUT_KEY_RIGHT) angle += 0.1;
    else if(key == GLUT_KEY_DOWN)
    {  //Move backward
        eye_x -= (float)0.1*sin(angle);
        eye_z += (float)0.1*cos(angle);
    }
    else if(key == GLUT_KEY_UP)
    { //Move forward
        eye_x += (float)0.1*sin(angle);
        eye_z -= (float)0.1*cos(angle);
    }
    //detectCollision();
    look_x = eye_x + 100*sin(angle);
    look_z = eye_z - 100*cos(angle);
    glutPostRedisplay();
}

//--------------------------------------------------------------------------------

void initialise()
{
    loadTexture();
    glEnable(GL_TEXTURE_2D);
    glClearColor(0., 1., 1., 1.);    //Background colour
}

//--------------------------------------------------------------------------------

void walls()
{
    glColor3f(0.8, 0.7, 0.3);   //replace with a texture
    glBindTexture(GL_TEXTURE_2D, txId[0]);
    glBegin(GL_QUADS);
    ////////////////////// BACK WALL ///////////////////////
    for (int x = -15; x < 15; x++) {
        for (int y = -1; y < 1; y++) {
            glTexCoord2f(0.0, 0.0);     glVertex3f(x, y, -15);
            glTexCoord2f(1.0, 0.0);     glVertex3f(x+1, y, -15);
            glTexCoord2f(1.0, 1.0);    glVertex3f(x+1, y+1, -15);
            glTexCoord2f(0.0, 1.0);    glVertex3f(x, y+1, -15);
        }
    }

    ////////////////////// FRONT WALL ///////////////////////

     glTexCoord2f(0.0, 2.0);     glVertex3f(-15, 1, 15);
     glTexCoord2f(0.0, 0.0);     glVertex3f(-15, -1, 15);
     glTexCoord2f(12.0, 0.0);    glVertex3f(15, -1, 15);
     glTexCoord2f(12.0, 2.0);    glVertex3f(15, 1, 15);

    ////////////////////// LEFT WALL ///////////////////////

     glTexCoord2f(0.0, 2.0);     glVertex3f(-15, 1, -15);
     glTexCoord2f(0.0, 0.0);     glVertex3f(-15, -1, -15);
     glTexCoord2f(12.0, 0.0);    glVertex3f(-15, -1, 15);
     glTexCoord2f(12.0, 2.0);    glVertex3f(-15, 1, 15);

    ////////////////////// RIGHT WALL ///////////////////////

     glTexCoord2f(0.0, 2.0);     glVertex3f(15, 1, -15);
     glTexCoord2f(0.0, 0.0);     glVertex3f(15, -1, -15);
     glTexCoord2f(12.0, 0.0);    glVertex3f(15, -1, 15);
     glTexCoord2f(12.0, 2.0);    glVertex3f(15, 1, 15);

    glEnd();
}
//--------------------------------------------------------------------------------

void floor()
{
    /*float white[4] = {1., 1., 1., 1.};
    float black[4] = {0};
    glColor4f(0.7, 0.7, 0.7, 1.0);  //The floor is gray in colour
    glNormal3f(0.0, 1.0, 0.0);
    glMaterialfv(GL_FRONT, GL_SPECULAR, black); // Set specular for floor to black
    //glColor3d(0.6, 0.25, 0.25);  //replace with a texture*/
    glBindTexture(GL_TEXTURE_2D, txId[1]);
    glBegin(GL_QUADS);
    for(int i = -15; i < 15; i++)
    {
        for(int j = -15;  j < 15; j++)
        {
            glTexCoord2f(0.0, 0.0);     glVertex3f(i, -1, j);
            glTexCoord2f(0.0, 1.0);     glVertex3f(i, -1, j+1);
            glTexCoord2f(1.0, 1.0);     glVertex3f(i+1, -1, j+1);
            glTexCoord2f(1.0, 0.0);     glVertex3f(i+1, -1, j);
        }
    }
    glEnd();
    //glMaterialfv(GL_FRONT, GL_SPECULAR, white); // Reset specular to white
}

//--------------------------------------------------------------------------------

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45., 1., 1., 100.);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(eye_x, 0, eye_z,  look_x, 0, look_z,   0, 1, 0);

    walls();
    floor();

    glutSwapBuffers();
}

//--------------------------------------------------------------------------------

int main(int argc, char** argv)
{
   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB| GLUT_DEPTH);
   glutInitWindowSize (600, 600);
   glutInitWindowPosition (10, 10);
   glutCreateWindow ("The Yard");
   initialise();

   glutDisplayFunc(display);
   glutSpecialFunc(special);
   glutMainLoop();
   return 0;
}
