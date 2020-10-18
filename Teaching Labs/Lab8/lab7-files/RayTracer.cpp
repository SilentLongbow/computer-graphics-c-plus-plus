/*========================================================================
* COSC 363  Computer Graphics (2018)
* Ray tracer 
* See Lab07.pdf for details.
*=========================================================================
*/
#include <iostream>
#include <cmath>
#include <vector>
#include <glm/glm.hpp>
#include "Sphere.h"
#include "SceneObject.h"
#include "Ray.h"
#include "../lab8-files/Plane.h"
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

vector<SceneObject*> sceneObjects;  //A global list containing pointers to objects in the scene


//---The most important function in a ray tracer! ---------------------------------- 
//   Computes the colour value obtained by tracing a ray and finding its 
//     closest point of intersection with objects in the scene.
//----------------------------------------------------------------------------------
glm::vec3 trace(Ray ray, int step)
{
	glm::vec3 backgroundCol(0);
	glm::vec3 light(50, 100, -50);
    glm::vec3 ambientCol(0.2);   //Ambient color of light

    ray.closestPt(sceneObjects);		//Compute the closest point of intersection of objects with the ray

    if(ray.xindex == -1) return backgroundCol;      //If there is no intersection return background colour
    glm::vec3 materialCol = sceneObjects[ray.xindex]->getColor(); //else return object's colour


    glm::vec3 normalVector = sceneObjects[ray.xindex]->normal(ray.xpt); // Get object that ray intersects with and get normal of object at point of intersection
    glm::vec3 lightVector = light - ray.xpt; // Get the vector to the light source from intersection
    lightVector = glm::normalize(lightVector); // and normalise it

    Ray shadow(ray.xpt, lightVector); // Create a shadow ray from the point of intersection with a direction to the light vector.
    shadow.closestPt(sceneObjects); // Compute the closest point of intersection between the shadow ray and the scene objects.

    glm::vec3 reflVector = glm::reflect(-lightVector, normalVector); // Get the reflection vector from point of intersection (comes from light source)
    glm::vec3 specularCol;

    glm::vec3 colourSum;



    float lDotn = glm::dot(lightVector, normalVector);
    float rDotv = glm::dot(reflVector, -ray.dir);

    if (rDotv < 0) {
        specularCol = glm::vec3(0.0);
    } else {
        specularCol =  glm::vec3(1.0, 1.0, 1.0) * (float) pow(rDotv, 50);
    }

    if (lDotn <= 0) {   // If dot product between normal and light is 0 or less
        colourSum = ambientCol * materialCol;
    } else if (shadow.xindex > -1 && shadow.xdist < distance(light, shadow.xpt)) {      // If shadow secondary ray hits an object before reading light source
        colourSum = ambientCol * materialCol;
    }
    else {
        colourSum = (ambientCol * materialCol) + (lDotn * materialCol) + specularCol;
        if (ray.xindex == 0 && step < MAX_STEPS) { // If we're hitting the reflective object && not at max steps
            glm::vec3 reflectedDirection = glm::reflect(ray.dir, normalVector);     // Get the direction of the reflected ray
            Ray reflectedRay(ray.xpt, reflectedDirection);  // Create the reflected ray
            glm::vec3 reflectedColour = trace(reflectedRay, step+1); // Recursively trace the reflected ray
            colourSum += (0.8f * reflectedColour);  // Add the reflected ray's colour values to to rest of the display
        }
    }
    return colourSum;
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

		    glm::vec3 dir(xp+0.5*cellX, yp+0.5*cellY, -EDIST);	//direction of the primary ray

		    Ray ray = Ray(eye, dir);		//Create a ray originating from the camera in the direction 'dir'
			ray.normalize();				//Normalize the direction of the ray to a unit vector
		    glm::vec3 col = trace (ray, 1); //Trace the primary ray and get the colour value

			glColor3f(col.r, col.g, col.b);
			glVertex2f(xp, yp);				//Draw each cell with its color value
			glVertex2f(xp+cellX, yp);
			glVertex2f(xp+cellX, yp+cellY);
			glVertex2f(xp, yp+cellY);
        }
    }

    glEnd();
    glutSwapBuffers();
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

	//-- Create a pointer to a sphere object
    Sphere *sphere1 = new Sphere(glm::vec3(0.0, 0.0, -100.0), 5.0, glm::vec3(0, 0, 1));
    Sphere *sphere2 = new Sphere(glm::vec3(-50.0, 0.0, -50.0), 10, glm::vec3(0, 1, 0));
    Sphere *sphere3 = new Sphere(glm::vec3(20.0, 0.0, -90.0), 10.0, glm::vec3(1, 0, 0));
    Plane *plane = new Plane (glm::vec3(-20., -20, -40),    //Point A
                              glm::vec3(20., -20, -40),     //Point B
                              glm::vec3(20., -20, -200),    //Point C
                              glm::vec3(-20., -20, -200),   //Point D
                              glm::vec3(0.5, 0.5, 0.0));      //Colour

	//--Add the above to the list of scene objects. (.push_back is the same as append)
    sceneObjects.push_back(sphere1);
    sceneObjects.push_back(sphere2);
    sceneObjects.push_back(sphere3);
    sceneObjects.push_back(plane);
}



int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB );
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(20, 20);
    glutCreateWindow("Lab 8");

    glutDisplayFunc(display);
    initialize();

    glutMainLoop();
    return 0;
}
