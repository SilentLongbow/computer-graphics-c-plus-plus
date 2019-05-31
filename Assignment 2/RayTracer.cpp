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
#include "texture-loading/TextureBMP.h"
#include <GL/glut.h>
using namespace std;

const float WIDTH = 20.0;
const float HEIGHT = 20.0;
const float EDIST = 40.0;
const int NUMDIV = 1000;
const int MAX_STEPS = 5;
const float XMIN = -WIDTH * 0.5;
const float XMAX =  WIDTH * 0.5;
const float YMIN = -HEIGHT * 0.5;
const float YMAX =  HEIGHT * 0.5;

float cameraHeight = 0;

vector<SceneObject*> sceneObjects;  //A global list containing pointers to objects in the scene

vector<glm::vec3> lightSources; // List of all light sources in the scene

bool superSampling = false;

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
    glm::vec3 backgroundColour = glm::vec3(191.0, 249.0, 254.0) / 255.0f;
    glm::vec3 ambientColour(0.2);   //Ambient color of light

    ray.closestPt(sceneObjects);		//Compute the closest point of intersection of objects with the ray
    int objectId = ray.intersectIndex;
    if(objectId == -1) return backgroundColour;      //If there is no intersection return background colour

    glm::vec3 materialCol;

    glm::vec3 normalVector = sceneObjects[objectId]->normal(ray.intersectPoint); // Get object that ray intersects with and get normal of object at point of intersection

    /* If the scene object has a procedural property, perform procedural texture generation */
    if (sceneObjects[objectId]->isProceduralPlane() && step < MAX_STEPS) {

        /* Following code was based off the code given on  http://math.hws.edu/ */
        float scale = 0.1;
        float a = floor(ray.intersectPoint.x * scale);
        float b = floor(ray.intersectPoint.z * scale);
        if (glm::mod(a+b, 2.0f) > 0.5) {  // a+b is odd
            materialCol = glm::vec3(.2, .2, .2);// / 255.0f; // Black
        }
        else {  // a+b is even
            materialCol = glm::vec3(0.5, 0.5, 0.5); // Grey
        }



    } else if (sceneObjects[objectId]->isProceduralSphere() && step < MAX_STEPS) {

        /* Following code was based off the code given on  http://math.hws.edu/ */

        float scale = 50;
        float a = floor((0.5 + atan2(normalVector.z, normalVector.x) / (2.0 * M_PI)) * scale);
        float b = floor((0.5 - asin(normalVector.y) / M_PI) * scale);

        if (glm::mod(b, 2.0f) > 0.5) {
            materialCol = glm::vec3(255.0, 223.0, 0.0) / 255.0f; // Black
        } else {
            materialCol = glm::vec3(.2);
        }
    } else {
        materialCol = sceneObjects[objectId]->getColor(); //else return object's colour
    }


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


        if (lDotn < 0) {   // If dot product between normal and light is 0 or less -> In shadow
            colourSum += ambientColour * materialCol;
        } else if (shadow.intersectIndex > -1 && shadow.intersectDistance < distance(currentLight, shadow.intersectPoint)) {  // If shadow secondary ray hits an object before reading light source
            colourSum += ambientColour * materialCol;
        }
        else {
            colourSum += (ambientColour * materialCol) + (lDotn * materialCol) + specularColour;
        }
    }



    if (sceneObjects[objectId]->isTextured()) {
        float horizontalUnit = 0.5 + atan2(normalVector.z, normalVector.x) / (2 * M_PI);
        float verticalUnit = 0.5 - asin(normalVector.y) / M_PI;
        colourSum += sceneObjects[objectId]->getTexture().getColorAt(horizontalUnit, verticalUnit);
    }

    if (sceneObjects[objectId]->isReflective() && step < MAX_STEPS) {
        float factor = sceneObjects[objectId]->getReflectiveFactor();
        glm::vec3 reflectedDirection = glm::reflect(ray.direction, normalVector);     // Get the direction of the reflected ray
        Ray reflectedRay(ray.intersectPoint, reflectedDirection);  // Create the reflected ray
        glm::vec3 reflectedColour = trace(reflectedRay, step+1); // Recursively trace the reflected ray
        colourSum += (factor * reflectedColour);  // Add the reflected ray's colour values to to rest of the display
    }

    if (sceneObjects[objectId]->isRefractive() && step < MAX_STEPS) {

        float refractiveIndex = sceneObjects[objectId]->getRefractiveIndex();   // Get refractive index of the object
        glm::vec3 insideRayDirection = glm::normalize(glm::refract(ray.direction, normalVector, refractiveIndex));    // Get the direction of the internal ray
        Ray refractedRay(ray.intersectPoint, insideRayDirection);   // Create the internal ray
        refractedRay.closestPt(sceneObjects);   // Find the point where the internal ray hits the object wall

        glm::vec3 otherSideNormal = sceneObjects[ray.intersectIndex]->normal(refractedRay.intersectPoint);
        glm::vec3 outsideRayDirection = glm::refract(insideRayDirection, -otherSideNormal, 1.0f/refractiveIndex);

        Ray outsideRay = Ray(refractedRay.intersectPoint, outsideRayDirection);

        colourSum += trace(outsideRay, step+1);

    }

    if (sceneObjects[objectId]->isTransparent() && step < MAX_STEPS) {
        Ray refractedRay(ray.intersectPoint, ray.direction);
        refractedRay.closestPt(sceneObjects);


        if (ray.intersectIndex == refractedRay.intersectIndex) {
            Ray outsideRay = Ray(refractedRay.intersectPoint, refractedRay.direction);

            colourSum += trace(outsideRay, step + 1);
        } else {
            // Edge case where no inside ray
            colourSum += trace(refractedRay, step + 1);
        }
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

    glm::vec3 eye(0., cameraHeight, 0.);  //The eye position (source of primary rays) is the origin

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

            /** Draw the Quad */
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
    glm::vec3 light1(50, 100, 200); // Behind camera
    glm::vec3 light2(-150, 100, -100); // In front to left

    lightSources.push_back(light1);
    lightSources.push_back(light2);
}

vector<Plane*> createBox(glm::vec3 centre, float height, float width, float depth, glm::vec3 colour) {
    vector<Plane*> boxPlanes;

    glm::vec3 backLeftTop = glm::vec3(centre.x - (width / 2), centre.y + (height / 2), centre.z - (depth / 2));
    glm::vec3 backRightTop = glm::vec3(centre.x + (width / 2), centre.y + (height / 2), centre.z - (depth / 2));
    glm::vec3 frontLeftTop = glm::vec3(centre.x - (width / 2), centre.y + (height / 2), centre.z + (depth / 2));
    glm::vec3 frontRightTop = glm::vec3(centre.x + (width / 2), centre.y + (height / 2), centre.z + (depth / 2));

    glm::vec3 backLeftBottom = glm::vec3(centre.x - (width / 2), centre.y - (height / 2), centre.z - (depth / 2));
    glm::vec3 backRightBottom = glm::vec3(centre.x + (width / 2), centre.y - (height / 2), centre.z - (depth / 2));
    glm::vec3 frontLeftBottom = glm::vec3(centre.x - (width / 2), centre.y - (height / 2), centre.z + (depth / 2));
    glm::vec3 frontRightBottom = glm::vec3(centre.x + (width / 2), centre.y - (height / 2), centre.z + (depth / 2));

    Plane *leftHandSide = new Plane(backLeftTop,
                                    backLeftBottom,
                                    frontLeftBottom,
                                    frontLeftTop,
                                    colour);

    Plane *rightHandSide = new Plane(frontRightTop,
                                     frontRightBottom,
                                     backRightBottom,
                                     backRightTop,
                                     colour);

    Plane *front = new Plane(frontLeftTop,
                             frontLeftBottom,
                             frontRightBottom,
                             frontRightTop,
                             colour);

    Plane *topSide = new Plane(backLeftTop,
                               frontLeftTop,
                               frontRightTop,
                               backRightTop,
                               colour);

    Plane *bottomSide = new Plane(backLeftBottom,
                                  frontLeftBottom,
                                  frontRightBottom,
                                  backRightBottom,
                                  colour);

    Plane *backSide = new Plane(backRightTop,
                                backRightBottom,
                                backLeftBottom,
                                backLeftTop,
                                colour);


    boxPlanes.push_back(front);
    boxPlanes.push_back(topSide);
    boxPlanes.push_back(rightHandSide);
    boxPlanes.push_back(leftHandSide);
    boxPlanes.push_back(bottomSide);
    boxPlanes.push_back(backSide);

    return boxPlanes;
}

TextureBMP setTexture(char pathname[100]) {
    char* pathLocation = pathname;
    TextureBMP texture = TextureBMP(pathLocation);
    return texture;
}

/**
 * Populates the sceneObjects vector with scene objects.
 */
void insertObjects() {

    //-- Create a pointer to a sphere object
    Sphere *sphere1 = new Sphere(glm::vec3(30.0, 0.0, 50.0), 10.0, glm::vec3(0, 0, 1));

    Sphere *refractiveSphere = new Sphere(glm::vec3(-3.0, -2.0, -30.0), 1.0, glm::vec3(0.01, 0.01, 0.01));

    Sphere *reflectiveSphere = new Sphere(glm::vec3(0.5, -3.0, -100.0), 7, (glm::vec3(20.0, 67.0, 55.0) / 255.0f));

    Sphere *transparentSphere = new Sphere(glm::vec3(-3.0, 3.0, -40.0), 2.0, glm::vec3(0.0, 0.0, 0.0));

    Sphere *reflectAndRefractSphere = new Sphere(glm::vec3(-6.0, -4.0, -40), 2.0, glm::vec3(0.01, 0.01, 0.01));

    Sphere *proceduralSphere = new Sphere(glm::vec3(-2.0, 0.0, -80), 3.0, glm::vec3(.5));

    Plane *plane = new Plane (glm::vec3(-1000.0, -10.0, 100.0),    //Point A
                              glm::vec3(1000.0, -10.0, 100.0),     //Point B
                              glm::vec3(1000.0, -10.0, -200.0),    //Point C
                              glm::vec3(-1000.0, -10.0, -200.0),   //Point D
                              glm::vec3(1.0, 1.0, 1.0));      //Colour

    Plane *backDrop = new Plane (glm::vec3(-500.0, 500.0, -200),
                                 glm::vec3(-500.0, -10.0, -200),
                                 glm::vec3(500.0, -10.0, -200),
                                 glm::vec3(500.0, 500.0, -200),
                                 glm::vec3(0.0, 0.0, 0.0));


    vector<Plane*> tallBox = createBox(glm::vec3(10, -10, -50), 1000000.0, 3.0, 7.0, glm::vec3(1, 0.5, 0));

    Sphere *gasGiant = new Sphere(glm::vec3(10.0, 14.0, -100.0), 3, glm::vec3(0.));

    char pathName[100] = "texture-storage/gas-giant.bmp";
    gasGiant->setTextured(setTexture(pathName));




    refractiveSphere->setRefractive(0.8);
    //refractiveSphere->setTransparent();
    reflectiveSphere->setReflective(0.6);
    transparentSphere->setTransparent();
    reflectAndRefractSphere->setReflective(0.8);
    //reflectAndRefractSphere->setTransparent();

    plane->setReflective(0.4);
    plane->setProceduralPlane();

    proceduralSphere->setProceduralSphere();


    //--Add the above to the list of scene objects. (.push_back is the same as append)
    sceneObjects.push_back(sphere1);
    sceneObjects.push_back(refractiveSphere);
    sceneObjects.push_back(reflectiveSphere);
    sceneObjects.push_back(transparentSphere);
    // sceneObjects.push_back(reflectAndRefractSphere);
    sceneObjects.push_back(plane);
    //sceneObjects.push_back(backDrop);

    sceneObjects.push_back(gasGiant);
    sceneObjects.push_back(proceduralSphere);

    for (int i = 0; i < tallBox.size(); i++) {
        tallBox[i]->setReflective(0.2);
        sceneObjects.push_back(tallBox[i]);
    }





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

    if (key ==' ') {
        cameraHeight += 5;
        keyPressed = true;
    }
    if (key == 'c') {
        cameraHeight -= 5;
        keyPressed = true;
    }


    if (keyPressed) {
        glutPostRedisplay();
    }
}



int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB );
    glutInitWindowSize(1000, 1000);
    glutInitWindowPosition(400, 50);
    glutCreateWindow("Assignment 2");

    glutDisplayFunc(display);
    initialize();

    glutKeyboardFunc(keyboardListener);
    glutMainLoop();
    return 0;
}
