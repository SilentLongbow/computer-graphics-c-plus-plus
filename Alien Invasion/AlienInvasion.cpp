//  ========================================================================
//  COSC 363 Assignment 1: Alien Invasion
//
//  FILE NAME: AlienInvasion.cpp
//  Author: Matthew Kenny
//  ========================================================================

#include <iostream>
#include <fstream>
#include <climits>
#include <stdio.h>
#include <cmath>
#include <GL/glut.h>
#include "textures/loadTGA.h"
using namespace std;

#define GL_CLAMP_TO_EDGE 0x812F // To remove seams between skybox textures

//----------------------------------GLOBAL VARIABLES------------------------------

GLuint txId[9];   //Texture ids
float look_x, look_y, look_z; // Point that camera looks at. Values calcualed in special
float eye_x = 0, eye_y = 0, eye_z = 40.0;  // Position of camera
float firstPersonAngle = 0;

float *mesh_x, *mesh_y, *mesh_z;  //vertex coordinate arrays
int *t1, *t2, *t3; //triangles
int nvrt, ntri;    //total number of vertices and triangles

bool shipCamera = false;
float shipLookAngle = 0;
float shipLook_x, shipLook_y, shipLook_z; // Camera look at positions for the ship PoV
float shipEye_x = 0, shipEye_y = 2.0, shipEye_z = -0.5; // Camera positions for the ship PoV
float horizontalDisplacement = 0;

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

float cannonElevation = 15.0;
float cannonHeading = 180.0;

// Initial position of cannonball
float ballInitial_z = 0.95;
float ballInitial_y = 0.6;

// Position of cannon ball during flight
float cannonBall_z;
float cannonBall_y;

float ballFiredAngle;
float cannonBallTick = 0;
float g = 9.81;
bool fireCannons = false;
bool loaded = true;

float robotPathAngle = 0.0;
float robotArmAngle = 0.0;
float robotBobHeight = 0.0;
bool rightArmForwards = true;

float black[4] = {0.0, 0.0, 0.0, 1.0};
float grey[4] = {0.2, 0.2, 0.2, 1.0};   // Ambient value
float white[4]  = {1.0, 1.0, 1.0, 1.0}; // Specular and diffuse value
float morning[4] = {0.4, 0.4, 0.4, 1.0};

float red[4] = {1.0, 0.0, 0.0}; // Robot light colour

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
    glBindTexture(GL_TEXTURE_2D, txId[7]);
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

void loadMeshFile(const char* fname)  // loadMesh function from Lab2
{
    ifstream fp_in;
    int num, ne;

    fp_in.open(fname, ios::in);
    if(!fp_in.is_open())
    {
        cout << "Error opening mesh file" << endl;
        exit(1);
    }

    fp_in.ignore(INT_MAX, '\n');				//ignore first line
    fp_in >> nvrt >> ntri >> ne;			    // read number of vertices, polygons, edges

    mesh_x = new float[nvrt];                        //create arrays
    mesh_y = new float[nvrt];
    mesh_z = new float[nvrt];

    t1 = new int[ntri];
    t2 = new int[ntri];
    t3 = new int[ntri];

    for(int i=0; i < nvrt; i++)                         //read vertex list
        fp_in >> mesh_x[i] >> mesh_y[i] >> mesh_z[i];

    for(int i=0; i < ntri; i++)                         //read polygon list
    {
        fp_in >> num >> t1[i] >> t2[i] >> t3[i];
        if(num != 3)
        {
            cout << "ERROR: Polygon with index " << i  << " is not a triangle." << endl;  //not a triangle!!
            exit(1);
        }
    }

    fp_in.close();
    cout << " File successfully read." << endl;
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
    loadMeshFile("meshes/Cannon.off");
    q =  gluNewQuadric();

    glEnable(GL_TEXTURE_2D);

    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE); // Use the colour values given for each object
    glEnable(GL_COLOR_MATERIAL); // Enable the ambient and diffuse line above

    glMaterialfv(GL_FRONT, GL_SPECULAR, white); // SPECULAR colour
    glMaterialf(GL_FRONT, GL_SHININESS, 50); // SHININESS value

    glEnable(GL_DEPTH_TEST); // Only show objects if view is not obstructed by others
    glEnable(GL_NORMALIZE); // Normalise normal values given

    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_AMBIENT, morning);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, morning);
    glLightfv(GL_LIGHT0, GL_SPECULAR, morning);



    ////////////////////// ROBOT LIGHT ///////////////////////
    glEnable(GL_LIGHT1);
    glLightfv(GL_LIGHT1, GL_AMBIENT, grey);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, red);
    glLightfv(GL_LIGHT1, GL_SPECULAR, red);

    ////////////////////// ROBOT LIGHT ///////////////////////
    glEnable(GL_LIGHT2);
    glLightfv(GL_LIGHT2, GL_AMBIENT, grey);
    glLightfv(GL_LIGHT2, GL_DIFFUSE, red);
    glLightfv(GL_LIGHT2, GL_SPECULAR, red);

    ////////////////////// ROBOT LIGHT ///////////////////////
    glEnable(GL_LIGHT3);
    glLightfv(GL_LIGHT3, GL_AMBIENT, grey);
    glLightfv(GL_LIGHT3, GL_DIFFUSE, red);
    glLightfv(GL_LIGHT3, GL_SPECULAR, red);

    glClearColor(0.0, 0.0, 0.0, 0.0); // Set background colour as white with 0 opacity

    // Set up camera projection values (fov, near, far, etc.)
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(80.0, 1, 0.1, 5000.0); // Set aspect ratio of 16:9

    // Set up look values for x & y
    look_x = eye_x + 100 * sin(firstPersonAngle);
    look_z = eye_z - 100 * cos(firstPersonAngle);

    // Set up look values for the ship PoV
    shipLook_x = shipEye_x + 100 * sin(shipLookAngle * M_PI / 180);
    shipLook_z = shipEye_z - 100 * cos(shipLookAngle * M_PI / 180);

    // Initial position of cannonball
    cannonBall_y = 0.6 + (1.35 * sin(cannonElevation * M_PI / 180));
    cannonBall_z = (1.35 * cos(cannonElevation * M_PI / 180)) - 0.4;


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

    //glMaterialfv(GL_FRONT, GL_SPECULAR, white); // Restore specular colour for other objects
}

//--------------------------------------------------------------------------------

void ground()
{
    glBindTexture(GL_TEXTURE_2D, txId[8]);
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_QUADS);
    glNormal3f(0.0, 1.0, 0.0);

    for (float x = -150; x < 150; x += 0.5)
    {
        for (float z = -150; z < 150; z += 0.5)
        {
            bool withinCastle = (x >= -15.5 && x <= 15.5) && (z >= -15.5 && z <= 15.5);
            if (!withinCastle)
            {
                glTexCoord2f(0.0, 0.0);     glVertex3f(x, -1, z);
                glTexCoord2f(0.0, 1.0);     glVertex3f(x, -1, z+0.5);
                glTexCoord2f(1.0, 1.0);     glVertex3f(x+0.5, -1, z+0.5);
                glTexCoord2f(1.0, 0.0);     glVertex3f(x+0.5, -1, z);
            }
        }
    }
    glEnd();
}

//--------------------------------------------------------------------------------

void wall(float length)
{
    glBindTexture(GL_TEXTURE_2D, txId[6]);

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
    glNormal3f(0.0, -1.0, 0.0);
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

void radarAntenna(bool coloured)
{
    glDisable(GL_TEXTURE_2D);
    if (coloured) glColor3f(35.0/255.0, 64.0/255.0, 152.0/255.0);
    gluCylinder(q, 0.1, 0.1, 1, 10, 10);
    glPushMatrix();

        ////////////////////// TOP SENSOR ///////////////////////
        glScalef(1.0, 4.0, 1.0);

        if (coloured) glColor3f(204.0/255.0, 152.0/255.0, 20.0/255.0);
        glMaterialfv(GL_FRONT, GL_SPECULAR, black); // Make top matte
        glutSolidCube(0.2);
        glMaterialfv(GL_FRONT, GL_SPECULAR, white); // Restore specular colour

    glPopMatrix();
    glEnable(GL_TEXTURE_2D);
}

//--------------------------------------------------------------------------------

void radarSensors(bool coloured)
{
    glPushMatrix();
        glRotatef(radarAngle, 0.0, 1.0, 0.0);
        glTranslatef(0.0, -0.7, 0.0);
        glRotatef(-90, 1.0, 0.0, 0.0);
        radarAntenna(coloured);
    glPopMatrix();
}

//--------------------------------------------------------------------------------

void opticalAntenna(bool coloured)
{
    glDisable(GL_TEXTURE_2D);
    glMaterialfv(GL_FRONT, GL_SPECULAR, black); // Make whole aparatus matte
    if (coloured) glColor3f(242.0/255.0, 104.0/255.0, 19.0/255.0);
    gluCylinder(q, 0.1, 0.1, 0.5, 10, 10);


    ////////////////////// HORIZONTAL ARM ///////////////////////
    glPushMatrix();
        glTranslatef(0.0, 0.1, 0.0);
        glScalef(1.0, 2.0, 1.0);
        glutSolidCube(0.2);
    glPopMatrix();

    glPushMatrix();
        if (coloured) glColor3f(0.5, 0.5, 0.5);
        glTranslatef(0.0, 0.35, 0.0);
        gluSphere(q, 0.15, 20, 20);
    glPopMatrix();

    glMaterialfv(GL_FRONT, GL_SPECULAR, white); // Restore specular colour
    glEnable(GL_TEXTURE_2D);
}

//--------------------------------------------------------------------------------

void opticalSensors(bool coloured)
{
    glPushMatrix();
        glRotatef(-shipAngle + 180, 0.0, 1.0, 0.0);
        glTranslatef(0.0, 2.0, 0.0);
        glRotatef(90, 1.0, 0.0, 0.0);
        opticalAntenna(coloured);
    glPopMatrix();
}

//--------------------------------------------------------------------------------

void shipHull(bool coloured)
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


    float ufoGrey;
    float ufoSilerDisk;
    float ufoSilverBridge;

    if (coloured) // Check if making ship or shadow
    {
        // Ship so colour appropriately
        ufoGrey = 45.0/255.0;
        ufoSilerDisk = 211.0/255.0;
        ufoSilverBridge = 125.0/255.0;
    }
    else
    {
        // Shadow so make all same colour
        ufoGrey = 0.2;
        ufoSilerDisk = 0.2;
        ufoSilverBridge = 0.2;
    }
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
            glRotatef(footAngle, 1.0, 0.0, 0.0);
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
    glDisable(GL_TEXTURE_2D);
    landingStrut();
    glPushMatrix();
        glRotatef(120, 0.0, 1.0, 0.0);
        landingStrut();
    glPopMatrix();
    glPushMatrix();
        glRotatef(240, 0.0, 1.0, 0.0);
        landingStrut();
    glPopMatrix();
    glEnable(GL_TEXTURE_2D);
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

void shadowMatrix()
{
    float gx = 4.5;
    float gy = 4.5;
    float gz = 20;
    float shadowMatrix[16] = {gy * 200,0,0,0,   -gx * 200,0,-gz * 200,-1,   0,0,gy * 200,0,    0,0,0,gy * 200};
    glMultMatrixf(shadowMatrix);
}

//--------------------------------------------------------------------------------

void castleShadow()
{
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glPushMatrix();
        glTranslatef(-1.0, -0.95, -4.45);
        shadowMatrix();
        glColor4f(0.2, 0.2, 0.2, 0.0);
        castleWalls();
        castleTowers();


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
            glRotatef(45, 0.0, 1.0, 0.0);
            glTranslatef(0.0, 0.378, 0.0);
            shipBulbs();
        glPopMatrix();

        // Generate hull
        glPushMatrix();
            glScalef(2.0, 2.0, 2.0);
            shipHull(true);
        glPopMatrix();
    if (renderUndercarriage) shipUndercarriage(); // Undercarriage
    glPushMatrix();
        glRotatef(shipLookAngle, 0.0, -1.0, 0.0);
        opticalSensors(true); // Optical sensor used for second camera
    glPopMatrix();
    radarSensors(true); // Radar underneath ship
    glPopMatrix();
}

//--------------------------------------------------------------------------------

void shipShadow()
{
    glDisable(GL_LIGHTING);
    glDisable(GL_COLOR_MATERIAL);
    glDisable(GL_TEXTURE_2D);


    glColor4f(0.2, 0.2, 0.2, 1.0);
    glPushMatrix();
        glTranslatef(-1.0, -0.95, -4.45);
        shadowMatrix();
        glTranslatef(0.0, shipHeight, ship_z); // Move whole unit
        glRotatef(shipAngle, 0.0, 1.0, 0.0); // Rotate ship

        // Generate hull
        glPushMatrix();
            glScalef(2.0, 2.0, 2.0);
            shipHull(false);
        glPopMatrix();
    if (renderUndercarriage) shipUndercarriage(); // Undercarriage
    radarSensors(false); // Radar underneath ship
    glPushMatrix();
        glRotatef(shipLookAngle, 0.0, -1.0, 0.0);
        opticalSensors(false); // Optical sensor used for second camera
    glPopMatrix();
    glPopMatrix();

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);
}

//--------------------------------------------------------------------------------

void drawCannon()
{
    glColor3f(110.0/255.0, 102.0/255.0, 113.0/255.0);

    //Construct the object model here using triangles read from OFF file
    glBegin(GL_TRIANGLES);
        for(int tindx = 0; tindx < ntri; tindx++)
        {
            float x1 = mesh_x[t1[tindx]], x2 = mesh_x[t2[tindx]], x3 = mesh_x[t3[tindx]];
            float y1 = mesh_y[t1[tindx]], y2 = mesh_y[t2[tindx]], y3 = mesh_y[t3[tindx]];
            float z1 = mesh_z[t1[tindx]], z2 = mesh_z[t2[tindx]], z3 = mesh_z[t3[tindx]];

           normal(x1, y1, z1, x2, y2, z2, x3, y3, z3);

           glVertex3d(x1, y1, z1);
           glVertex3d(x2, y2, z2);
           glVertex3d(x3, y3, z3);
        }
    glEnd();
}

//--------------------------------------------------------------------------------

void drawCannonFrame()
{
    glDisable(GL_LIGHTING);
    glMaterialfv(GL_FRONT, GL_SPECULAR, black); // SPECULAR colour
    // Right base
    glPushMatrix();
        glTranslatef(-10, 5, 17);
        glScalef(80, 10, 6);
        glColor3f(0.651, 0.502, 0.392);
        glutSolidCube(1);
    glPopMatrix();

    // Right frame
    glPushMatrix();
        glTranslatef(-20, 25, 17);
        glScalef(40, 30, 6);
        glColor3f(0.651, 0.502, 0.392);
        glutSolidCube(1);
    glPopMatrix();

    // Left base
    glPushMatrix();
        glTranslatef(-10, 5, -17);
        glScalef(80, 10, 6);
        glColor3f(0.651, 0.502, 0.392);
        glutSolidCube(1);
    glPopMatrix();

    // Left Frame
    glPushMatrix();
        glTranslatef(-20, 25, -17);
        glScalef(40, 30, 6);
        glColor3f(0.651, 0.502, 0.392);
        glutSolidCube(1);
    glPopMatrix();

    glMaterialfv(GL_FRONT, GL_SPECULAR, white); // SPECULAR colour
    glEnable(GL_LIGHTING);
}

//--------------------------------------------------------------------------------

void cannonBall()
{
    glColor3f(80.0/255.0, 58.0/255.0, 46.0/255.0);
    glPushMatrix();
        glTranslatef(0.0, cannonBall_y, cannonBall_z); // Position in cannon
        glScalef(0.02, 0.02, 0.02);
        gluSphere(q, 5, 36, 18);
    glPopMatrix();
}

//--------------------------------------------------------------------------------

void cannonEmplacement()
{

    glPushMatrix();
        glTranslatef(0.0, -1.0, 0.0);
        cannonBall();
        glScalef(0.02, 0.02, 0.02);
        glRotatef(90 + cannonHeading, 0.0, 1.0, 0.0);
        drawCannonFrame();
        glPushMatrix();
            glTranslatef(-20, 30, 0);
            glRotatef(cannonElevation, 0.0, 0.0, 1.0);
            glTranslatef(20, -30, 0);
            drawCannon();
        glPopMatrix();

    glPopMatrix();
}

//--------------------------------------------------------------------------------

void defences()
{
    glPushMatrix();
        glTranslatef(-4.0, 0.0, 18.0);
        cannonEmplacement();
    glPopMatrix();


    glPushMatrix();
        glTranslatef(4.0, 0.0, 18.0);
        cannonEmplacement();
    glPopMatrix();
}

//--------------------------------------------------------------------------------

void robotBody(bool shadow)
{
    if (shadow) {
        glColor3f(0.2, 0.2, 0.2);
    } else {
        glColor3f(178.0/255.0, 34.0/255.0, 34.0/255.0);
    }

    glPushMatrix();
        glScalef(1.1, 1.2, 1.1);
        glutSolidCube(1);
    glPopMatrix();
}

//--------------------------------------------------------------------------------

void singleArm(bool shadow)
{
    if (shadow) {
        glColor3f(0.2, 0.2, 0.2);
    } else {
        glColor3f(1.0, 165.0/255.0, 0.0);
    }

    /////////////////////// UPPER ARM //////////////////////////
    glPushMatrix();
        glTranslatef(0.55, 0.5, 0.0);
        glRotatef(45, 0.0, 0.0, 1.0);
        glTranslatef(-0.2, 0.0, 0.0);
        glRotatef(90, 1.0, 0.0, 0.0);
        glutSolidCylinder(0.2, 0.8, 36, 18);
    glPopMatrix();

    /////////////////////// ARM JOINT //////////////////////////
    glPushMatrix();
        glTranslatef(0.9, -0.1, 0.0);
        glutSolidSphere(0.25, 36, 18);
    glPopMatrix();

    /////////////////////// FOREARM //////////////////////////
    glPushMatrix();
        glTranslatef(1.1, -0.05, 0.0);
        glTranslatef(-0.2, 0.0, 0.0);
        glRotatef(90, 1.0, 0.0, 0.0);
        glutSolidCylinder(0.2, 1.0, 36, 18);
    glPopMatrix();

    /////////////////////// HAND //////////////////////////
    glPushMatrix();
        glTranslatef(1.1, -0.95, 0.0);
        glTranslatef(-0.2, 0.0, 0.0);
        glRotatef(90, 1.0, 0.0, 0.0);
        glutSolidCone(0.2, 0.5, 36, 18);
    glPopMatrix();
}

//--------------------------------------------------------------------------------

void robotArms(bool shadow)
{
    glPushMatrix();
        glTranslatef(0.0, 0.35, 0.0);
        glRotatef(robotArmAngle, 1.0, 0.0, 0.0);
        glTranslatef(0.0, -0.35, 0.0);
        singleArm(shadow);
    glPopMatrix();

    glPushMatrix();
        glTranslatef(0.0, 0.35, 0.0);
        glRotatef(robotArmAngle, -1.0, 0.0, 0.0);
        glTranslatef(0.0, -0.35, 0.0);
        glRotatef(180, 0.0, 1.0, 0.0);
        singleArm(shadow);
    glPopMatrix();
}

//--------------------------------------------------------------------------------

void robotFacialFeatures(bool shadow)
{
    if (shadow) {
        glColor3f(0.2, 0.2, 0.2);
    } else {
        glColor3f(0.6, 0.6, 0.6);
    }
    glDisable(GL_LIGHTING);

    /////////////////////// ANTENNA SHAFT //////////////////////////
    glPushMatrix();
        glTranslatef(0.0, 0.45, 0.0);
        glRotatef(90, 1.0, 0.0, 0.0);
        gluCylinder(q, 0.01, 0.01, 0.3, 36, 18);
    glPopMatrix();

    /////////////////////// ANTENNA SPHERE //////////////////////////
    glPushMatrix();
        glTranslatef(0.0, 0.45, 0.0);
        gluSphere(q, 0.05, 36, 18);
    glPopMatrix();

    /////////////////////// RIGHT EYE /////////////////////////
    if (shadow) {
        glColor3f(0.2, 0.2, 0.2);
    } else {
        glColor3f(1.0, 0.0, 0.0);
    }

    glPushMatrix();
        glTranslatef(-0.1, 0.1, 0.33);
        glScalef(0.1, 0.05, 0.05);
        glutSolidCube(1);
    glPopMatrix();

    /////////////////////// LEFT EYE //////////////////////////
    glPushMatrix();
        glTranslatef(0.1, 0.1, 0.33);
        glScalef(0.1, 0.05, 0.05);
        glutSolidCube(1);
    glPopMatrix();

    /////////////////////// MOUTH //////////////////////////
    glColor3f(0.9, 0.9, 0.9);
    glPushMatrix();
        glTranslatef(0.0, -0.05, 0.33);
        glScalef(0.1, 0.05, 0.05);
        glutSolidCube(1);
    glPopMatrix();
    glEnable(GL_LIGHTING);
}

//--------------------------------------------------------------------------------

void robotHead(bool shadow)
{
    if (shadow) {
        glColor3f(0.2, 0.2, 0.2);
    } else {
        glColor3f(0.4, 0.4, 0.4);
    }
    glPushMatrix();
        glTranslatef(0.0, 0.75, 0.0);

        glPushMatrix();
            glScalef(0.5, 0.3, 0.7);
            glutSolidCube(1);
        glPopMatrix();

        robotFacialFeatures(shadow);
    glPopMatrix();
}

//--------------------------------------------------------------------------------

void robotLight(GLenum lightNum)
{
    float spotlightPosition[4] = {0.0, 0.75, 0.35, 1.0};
    float spotlightDirection[4] = {0.0, -0.8, 1.0, 0.0};

    glLightf(lightNum, GL_SPOT_CUTOFF, 20.0);
    glLightf(lightNum, GL_SPOT_EXPONENT, 1.0);
    glLightfv(lightNum, GL_POSITION, spotlightPosition);
    glLightfv(lightNum, GL_SPOT_DIRECTION, spotlightDirection);
}

//--------------------------------------------------------------------------------

void robotLower(bool shadow)
{
    if (shadow) {
        glColor3f(0.2, 0.2, 0.2);
    } else {
        glColor3f(0.6, 0.6, 1.0);
    }

    glPushMatrix();
        glTranslatef(0.0, -0.6, 0.0);
        glRotatef(90, 1.0, 0.0, 0.0);
        glutSolidCone(0.55, 0.6, 38, 18);
    glPopMatrix();
}

//--------------------------------------------------------------------------------

void robotSphere(bool shadow)
{
    if (shadow) {
        glColor3f(0.2, 0.2, 0.2);
    } else {
        glColor3f(0.7, 0.7, 0.7);
    }

    glPushMatrix();
        glTranslatef(0.0, -1.4, 0.0);
        gluSphere(q, 0.4, 36, 18);
    glPopMatrix();
}

//--------------------------------------------------------------------------------

void buildRobot(GLenum lightNum, bool clockwise, float patrolRadius, float robotAngle)
{

    bool isShadow = false;
    float rotationConstant;
    float initialHeading;
    if (clockwise) {
        rotationConstant = -1;
        initialHeading = 90;
    } else {
        rotationConstant = 1;
        initialHeading = -90;
    }

    float robot_x = patrolRadius * rotationConstant * sin((robotAngle + robotPathAngle) * M_PI / 180);
    float robot_z = patrolRadius * cos((robotAngle + robotPathAngle) * M_PI / 180);
    float robotBounce = (0.08 * sin(robotPathAngle / 2));
    glPushMatrix();
        glTranslatef(robot_x, 0.8, robot_z);
        glRotatef(initialHeading - robotAngle + (-rotationConstant * robotPathAngle), 0.0, -1.0, 0.0);
        robotSphere(isShadow);
        glTranslatef(0.0, robotBounce, 0.0);
        robotBody(isShadow);
        robotHead(isShadow);
        robotLight(lightNum);
        robotLower(isShadow);
        robotArms(isShadow);
    glPopMatrix();
}

//--------------------------------------------------------------------------------

void robotPatrol()
{

    glDisable(GL_TEXTURE_2D);
    glMaterialfv(GL_FRONT, GL_SPECULAR, grey);

    buildRobot(GL_LIGHT1, true, 30, 0.0);
    //robotShadow(false, 30, 0.0);

    buildRobot(GL_LIGHT2, false, 45, 90);
    //robotShadow(false, 45, 90);

    buildRobot(GL_LIGHT3, true, 10, 180);
    //robotShadow(true, 10, 180);

    glEnable(GL_TEXTURE_2D);
    glMaterialfv(GL_FRONT, GL_SPECULAR, white);

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
    if (!takeOff && key == 's')
    {
        takeOff = true;
        shipCamera = true;
    }
    if (key == 'k' && cannonElevation <= 75)
    {
        cannonElevation += 1;
    }
    else if (key == 'i' && cannonElevation >= -20) {
        cannonElevation -= 1;
    }
    if (loaded) {
        if (key == 'c'){ // Fire guns!
            fireCannons = true;
            loaded = false;
        }
        cannonBall_y = ballInitial_y;
        cannonBall_z = ballInitial_z;
        ballFiredAngle = cannonElevation;
    }
    if (!loaded && !fireCannons && key == 'r') { // Reload guns!
        // Reset to original values
        cannonBall_y = ballInitial_y;
        cannonBall_z = ballInitial_z;
        cannonBallTick = 0;
        loaded = true;
    }

    look_x = eye_x + 100 * sin(firstPersonAngle);
    look_y = eye_y;
    look_z = eye_z - 100 * cos(firstPersonAngle);

    ballInitial_y = 0.6 + (1.35 * sin(cannonElevation * M_PI / 180));
    ballInitial_z = (1.35 * cos(cannonElevation * M_PI / 180)) - 0.4;

    cannonBall_y = ballInitial_y;
    cannonBall_z = ballInitial_z;

    glutPostRedisplay();
}

//--------------------------------------------------------------------------------

void special(int key, int x, int y)
{
    float next_x;
    float next_z;
    if (!shipCamera) // Check we are in first person camera
    {
        if (key == GLUT_KEY_LEFT)
        {
            // Look right
            firstPersonAngle -= 5 * M_PI / 180; // 5 Degrees
        } else if (key == GLUT_KEY_RIGHT)
        {
            // look left
            firstPersonAngle += 5 * M_PI / 180; // 5 Degrees
        }
        else if (key == GLUT_KEY_UP)
        {
            // Move forwards
            next_x = eye_x + 0.2 * sin(firstPersonAngle);
            next_z = eye_z - 0.2 * cos(firstPersonAngle);
            towerCollision(&next_x, &next_z); // Tower collision calculations
            wallCollision(&next_x, &next_z); // Wall collision will change values of next_x and next_z if needed
            eye_x = next_x;
            eye_z = next_z;
        } else if (key == GLUT_KEY_DOWN)
        {
            // Move backwards
            next_x = eye_x - 0.2 * sin(firstPersonAngle);
            next_z = eye_z + 0.2 * cos(firstPersonAngle);
            towerCollision(&next_x, &next_z);
            wallCollision(&next_x, &next_z);
            eye_x = next_x;
            eye_z = next_z;
        }
    } else {
        if (key == GLUT_KEY_LEFT)
        {
            // Look right
            shipLookAngle -= 5; // 5 Degrees
        } else if (key == GLUT_KEY_RIGHT)
        {
            // look left
            shipLookAngle += 5; // 5 Degrees
        }
    }
    if (key == GLUT_KEY_HOME && renderShip)
    {
        // Switch Camera
        shipCamera = !shipCamera;
    }

    // Calculate lookat position
    look_x = eye_x + 100 * sin(firstPersonAngle);
    look_y = eye_y;
    look_z = eye_z - 100 * cos(firstPersonAngle);

    shipEye_x = 0.5 * sin(shipLookAngle * M_PI / 180);
    shipEye_z = horizontalDisplacement + (0.5 * -cos(shipLookAngle * M_PI / 180));

    worldCollision();
    glutPostRedisplay();
}

//--------------------------------------------------------------------------------

void timerFunction(int value)
{
    robotPathAngle += 0.5;

    if (robotPathAngle >= 360.0) {
        robotPathAngle = 0.0;
    }

    // Alternate forward and back swing of arms
    if (robotArmAngle >=45) {
        rightArmForwards = false;
    } else if (robotArmAngle <= -45) {
        rightArmForwards = true;
    }

    // Arm angle increment
    if (rightArmForwards) {
        robotArmAngle += 4.0;
    } else {
        robotArmAngle -= 4.0;
    }

    bulbTick += 1;
    if (bulbTick >= 20) { // Alternates bulbs at approximately 0.5s
        bulbAngle += 45.0;
        if (bulbAngle == 360.0) bulbAngle = 0.0;
        bulbTick = 0;
    }

    radarAngle += 10;
    if (radarAngle >= 360.0) radarAngle = 0.0;

    if (renderShip && takeOff) {
        // Slowly rise to 3 units height to raise gear
        if (shipHeight < 4)
        {
            shipEye_y += 0.01;
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
            shipEye_y += 0.1;
            shipHeight += 0.1;
        }

        if (undercarriageAngle >= 90) {
            renderUndercarriage = false;
        }

        // If at correct elevation, start moving forwards!
        if (shipHeight >= 30.0)
        {
            horizontalDisplacement += 0.5;
            ship_z += 0.5;
        }

        // When ship past skybox, stop rendering it
        if (ship_z > 210)
        {
            renderShip = false;
        } else if (ship_z > 190)
        {
            shipCamera = false;
        }
    }

    if (fireCannons) // Calculate z and y components of cannonball position
    {
        cannonBallTick += 25.0;
        cannonBall_z = ballInitial_z + (25 * cos(ballFiredAngle * M_PI / 180) * (cannonBallTick / 1000));
        cannonBall_y = ballInitial_y + (25 * sin (ballFiredAngle * M_PI / 180) * (cannonBallTick / 1000)) + (0.5 * -9.81 * pow(cannonBallTick / 1000, 2));

        if (cannonBall_y <= -1.0)
        {
            fireCannons = false;
        }
    }

    shipEye_z = horizontalDisplacement + (0.5 * -cos(shipLookAngle * M_PI / 180));

    glutPostRedisplay();
    glutTimerFunc(25, timerFunction, 0);
}

//--------------------------------------------------------------------------------

void display()
{
    float sunPos[4] = {-4.5, 4.5, -20.0, 0.0}; // Vector from origin towards lightbox sun

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);    //GL_LINE = Wireframe;   GL_FILL = Solid
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    shipLook_x = shipEye_x + 100 * sin(shipLookAngle * M_PI / 180);
    shipLook_y = shipEye_y;
    shipLook_z = shipEye_z - 100 * cos(shipLookAngle * M_PI / 180);


    if (shipCamera)
    {
        gluLookAt(shipEye_x, shipEye_y, shipEye_z,      shipLook_x, shipLook_y, shipLook_z,     0, 1, 0);
    } else
    {
        gluLookAt(eye_x, eye_y, eye_z,  look_x, look_y, look_z,  0, 1, 0);
    }

    glEnable(GL_LIGHTING); // Let there be light!

    glColor3f(1.0, 1.0, 0.0);

    // Rotate light and skybox
    glPushMatrix();
        glRotatef(180, 0.0, 1.0, 0.0);
        glLightfv(GL_LIGHT0, GL_POSITION, sunPos); // Create the 'sun' - Position is normalised
        skybox();
    glPopMatrix();
    floor();
    ground();
    castleWalls();
    castleTowers();
    castleShadow();
    if (renderShip) {
        ship();
        shipShadow();
    }
    defences();
    robotPatrol();

    glutPostRedisplay();
    glutSwapBuffers();
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
