/*========================================================================
* COSC 363 Assignment 2
* Ray Tracing
* Matthew Kenny
* mjk141
* 11031880
*=========================================================================
*/
#include <iostream>
#include <cmath>
#include <algorithm>
#include <vector>
#include <stdbool.h>
#include <glm/glm.hpp>
#include <math.h>
#include "scene-objects/Sphere.h"
#include "scene-objects/SceneObject.h"
#include "rays/Ray.h"
#include "scene-objects/Plane.h"
#include <GL/glut.h>
using namespace std;

const float WIDTH = 20.0;
const float HEIGHT = 20.0;
const float EDIST = 40.0;
const int NUMDIV = 800;
const int MAX_STEPS = 5;
const float XMIN = -WIDTH * 0.5;
const float XMAX =  WIDTH * 0.5;
const float YMIN = -HEIGHT * 0.5;
const float YMAX =  HEIGHT * 0.5;

vector<SceneObject*> sceneObjects;  //A global list containing pointers to objects in the scene

vector<int> reflectiveObjects;
vector<int> transparentObjects;
vector<int> refractiveObjects;


vector<glm::vec3> lightSources; // List of all light sources in the scene

bool superSampling = false;
//int superSamplingAmount = 2;

glm::vec3 calculateSpecularColour(float rDotV) {
    glm::vec3 specularColour;

    if (rDotV < 0) { // If
        specularColour = glm::vec3(0.0);
    } else {
        specularColour =  glm::vec3(1.0, 1.0, 1.0) * (float) pow(rDotV, 50);
    }

    return specularColour;
}




//---The most important function in a ray tracer! ----------------------------------
//   Computes the colour value obtained by tracing a ray and finding its
//     closest point of intersection with objects in the scene.
//----------------------------------------------------------------------------------
glm::vec3 trace(Ray ray, int step)
{
    glm::vec3 backgroundColour = glm::vec3(159.0, 238.0, 252.0) / 255.0f;
    glm::vec3 ambientColour(0.2);   //Ambient color of light

    ray.closestPt(sceneObjects);		//Compute the closest point of intersection of objects with the ray
    int objectId = ray.intersectIndex;
    if(objectId == -1) return backgroundColour;      //If there is no intersection return background colour

    glm::vec3 materialCol;

    if (sceneObjects[objectId]->isProcedural()) {



        float scale = 0.1;
        float a = floor(ray.intersectPoint.x * scale);
        float b = floor(ray.intersectPoint.z * scale);
        if (glm::mod(a+b, 2.0f) < 0.5) {  // a+b is odd
            materialCol = glm::vec3(0.0, 0.0, 0.0); // pink
        }
        else {  // a+b is even
            materialCol = glm::vec3(0.7, 0.7, 0.7); // light blue
        }



    } else {
        materialCol = sceneObjects[objectId]->getColor(); //else return object's colour
    }
    glm::vec3 normalVector = sceneObjects[objectId]->normal(ray.intersectPoint); // Get object that ray intersects with and get normal of object at point of intersection


    // Total colour of the quad
    glm::vec3 colourSum = glm::vec3(0.0);

    for (int lightSourceId = 0; lightSourceId < lightSources.size(); lightSourceId++) {
        glm::vec3 currentLight = lightSources[lightSourceId]; // Get current light


        glm::vec3 lightVector = glm::normalize(currentLight - ray.intersectPoint); // Get the vector to the light source from intersection

        Ray shadow(ray.intersectPoint, lightVector); // Create a shadow ray from the point of intersection with a direction to the light vector.
        shadow.closestPt(sceneObjects); // Compute the closest point of intersection between the shadow ray and the scene objects.

        glm::vec3 reflectionVector = glm::reflect(-lightVector, normalVector); // Get the reflection vector from point of intersection (comes from light source)

        float lDotn = glm::dot(lightVector, normalVector); // Dot product between -lightVector and normal
        float rDotV = glm::dot(reflectionVector, -ray.direction); // Dot product between reflection vector and vector to camera

        glm::vec3 specularColour = calculateSpecularColour(rDotV);


        if (lDotn <= 0) {   // If dot product between normal and light is 0 or less -> In shadow
            colourSum += ambientColour * materialCol;
        } else if (shadow.intersectIndex > -1 && shadow.intersectDistance < distance(currentLight, shadow.intersectPoint)) {  // If shadow secondary ray hits an object before reading light source
            colourSum += ambientColour * materialCol;
        }
        else {
            colourSum += (ambientColour * materialCol) + (lDotn * materialCol) + specularColour;
        }
    }
    if (sceneObjects[objectId]->isReflective()) {
        glm::vec3 reflectedDirection = glm::reflect(ray.direction, normalVector);     // Get the direction of the reflected ray
        Ray reflectedRay(ray.intersectPoint, reflectedDirection);  // Create the reflected ray
        glm::vec3 reflectedColour = trace(reflectedRay, step+1); // Recursively trace the reflected ray
        colourSum += (0.4f * reflectedColour);  // Add the reflected ray's colour values to to rest of the display
    }
    return colourSum;
}

/**
 * Colours a quad from the display plane using a single ray traced from the eye positon, through the quad.
 * Returns the colour of the quad.
 */
glm::vec3 getStandardColour(float cellPosX, float cellPosY, int step, glm::vec3 eyePos) {


    float cellWidth = (XMAX-XMIN)/NUMDIV;  //cell width
    float cellHeight = (YMAX-YMIN)/NUMDIV;  //cell height

    glm::vec3 cellColour = glm::vec3(0.0);

    glm::vec3 dir(cellPosX + (cellWidth / 2), cellPosY + (cellHeight / 2), -EDIST);	//direction of the primary ray
    Ray ray = Ray(eyePos, dir);		//Create a ray originating from the camera in the direction 'dir'
    ray.normalize();				//Normalize the direction of the ray to a unit vector
    cellColour = trace(ray, step);
    return cellColour;
}

/**
 * Performs super-sampling on the display plane. Will send as many rays as numDivision^2 through each quad.
 * Returns the average colour attained from each ray.
 */
glm::vec3 superSampleColour(float cellPosX, float cellPosY, int step, float numDivisions, glm::vec3 eyePos) {

    float cellWidth = (XMAX-XMIN)/NUMDIV;  //cell width
    float cellHeight = (YMAX-YMIN)/NUMDIV;  //cell height

    glm::vec3 totalColour(0.0);

    for(int xSubDiv = 1; xSubDiv < numDivisions + 1; xSubDiv++) {
        float divisionHorizontal = 1.0 / numDivisions / 0.5 * (float) xSubDiv;
        for (int ySubDiv = 1; ySubDiv < numDivisions + 1; ySubDiv++) {
            float divisionVertical = 1.0 / numDivisions / ((float) ySubDiv / 2);

            glm::vec3 dir(cellPosX + (divisionHorizontal * cellWidth), cellPosY + (divisionVertical * cellHeight), -EDIST);	//direction of the primary ray
            Ray ray = Ray(eyePos, dir);		//Create a ray originating from the camera in the direction 'dir'
            ray.normalize();				//Normalize the direction of the ray to a unit vector
            totalColour += trace (ray, step); //Trace the primary ray and get the colour value
        }
    }
    return totalColour / (float) (numDivisions * numDivisions);
}

//---The main display module -----------------------------------------------------------
// In a ray tracing application, it just displays the ray traced image by drawing
// each cell as a quad.
//---------------------------------------------------------------------------------------
void display()
{
    float xp, yp;  //grid point
    float cellX = (XMAX-XMIN)/NUMDIV;  //cell width
    float cellY = (YMAX-YMIN)/NUMDIV;  //cell height

    glm::vec3 eye(0., 0., 0.);  //The eye position (source of primary rays) is the origin

    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glBegin(GL_QUADS);  //Each cell is a quad.

    for(int i = 0; i < NUMDIV; i++)  	//For each grid point xp, yp
    {
        xp = XMIN + i*cellX;
        for(int j = 0; j < NUMDIV; j++)
        {
            yp = YMIN + j*cellY;
            glm::vec3 totalColour = glm::vec3(0.0);
            int step = 1;
            float superSamplingAmount = 2.0;
            glm::vec3 cellColour;

            if (superSampling) {
                cellColour = superSampleColour(xp, yp, step, superSamplingAmount, eye); // Perform super sampling on the quads
            } else {
                cellColour = getStandardColour(xp, yp, step, eye); // Perform basic ray tracing
            }

            /* Draw the Quad */
            glColor3f(cellColour.r, cellColour.g, cellColour.b);
            glVertex2f(xp, yp);				//Draw each cell with its color value
            glVertex2f(xp+cellX, yp);
            glVertex2f(xp+cellX, yp+cellY);
            glVertex2f(xp, yp+cellY);
        }
    }

    glEnd();
    glutSwapBuffers();
}

/**
 * Populates the lightSources vector with light source points
 */
void insertLightSources() {
    glm::vec3 light1(5.0, 15.0, 5.0); // Behind camera
    glm::vec3 light2(-100, 5, -300); // In front to left

    lightSources.push_back(light1);
    lightSources.push_back(light2);
}

/**
 * Populates the sceneObjects vector with scene objects.
 */
void insertObjects() {

    //-- Create a pointer to a sphere object
    Sphere *sphere1 = new Sphere(glm::vec3(-10.0, 0.0, 50.0), 10.0, glm::vec3(0, 0, 1));
    Sphere *sphere2 = new Sphere(glm::vec3(-10.0, 0.0, -50.0), 5.0, glm::vec3(0, 1, 0));
    Sphere *sphere3 = new Sphere(glm::vec3(0.0, -5.0, -70.0), 3.0, glm::vec3(1, 0, 0));
    Plane *plane = new Plane (glm::vec3(-1000.0, -10.0, -40.0),    //Point A
                              glm::vec3(1000.0, -10.0, -40.0),     //Point B
                              glm::vec3(1000.0, -10.0, -200.0),    //Point C
                              glm::vec3(-1000.0, -10.0, -200.0),   //Point D
                              glm::vec3(0.5, 0.5, 0.5));      //Colour

    sphere3->setReflective();
    plane->setReflective();
    plane->setProcedural();

    //--Add the above to the list of scene objects. (.push_back is the same as append)
    sceneObjects.push_back(sphere1);
    sceneObjects.push_back(sphere2);
    sceneObjects.push_back(sphere3);
    sceneObjects.push_back(plane);
}


//---This function initializes the scene ------------------------------------------- 
//   Specifically, it creates scene objects (spheres, planes, cones, cylinders etc)
//     and add them to the list of scene objects.
//   It also initializes the OpenGL orthographc projection matrix for drawing the
//     the ray traced image.
//----------------------------------------------------------------------------------
void initialize()
{
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(XMIN, XMAX, YMIN, YMAX);
    glClearColor(0, 0, 0, 1);

    insertLightSources();

    insertObjects();

}

void keyboardListener(unsigned char key, int x, int y) {
    bool keyPressed = false;
    if (key == 'a') {
        superSampling = !superSampling;
        keyPressed = true;
    } /*else if (key == '2') {
        superSamplingAmount = 2;
        keyPressed = true;
    } else if (key == '3') {
        superSamplingAmount = 3;
        keyPressed = true;
    } else if (key == '4') {
        superSamplingAmount = 4;
        keyPressed = true;
    }*/


    if (keyPressed) {
        glutPostRedisplay();
    }
}



int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB );
    glutInitWindowSize(800, 800);
    glutInitWindowPosition(400, 50);
    glutCreateWindow("Assignment 2");

    glutDisplayFunc(display);
    initialize();

    glutKeyboardFunc(keyboardListener);
    glutMainLoop();
    return 0;
}
