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
	glm::vec3 light(10, 40, -3);
    glm::vec3 ambientCol(0.2);   //Ambient color of light

    ray.closestPt(sceneObjects);		//Compute the closest point of intersetion of objects with the ray

    if(ray.xindex == -1) return backgroundCol;      //If there is no intersection return background colour
    glm::vec3 materialCol = sceneObjects[ray.xindex]->getColor(); //else return object's colour


    glm::vec3 normalVector = sceneObjects[ray.xindex]->normal(ray.xpt); // Get object that ray intersects with and get normal of object at point of intersection
    glm::vec3 lightVector = light - ray.xpt; // Get the vector to the light source from intersection
    lightVector = glm::normalize(lightVector); // and normalise it

    glm::vec3 reflVector = glm::reflect(-lightVector, normalVector); // Get the reflection vector from point of intersection (comes from light source)
    glm::vec3 specularCol;
    glm::vec3 colour;


    float lDotn = glm::dot(lightVector, normalVector);
    float rDotv = glm::dot(reflVector, -ray.dir);

    if (rDotv < 0) {
        specularCol = glm::vec3(0.0);
    } else {
        specularCol =  glm::vec3(1.0, 1.0, 1.0) * (float) pow(rDotv, 10);
    }

    if (lDotn < 0) {
        colour = ambientCol * materialCol + specularCol;
    } else {
        colour = (ambientCol * materialCol) + (lDotn * materialCol) + specularCol;
    }
    return colour;
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
    Sphere *sphere1 = new Sphere(glm::vec3(-5.0, -5.0, -90.0), 15.0, glm::vec3(0, 0, 1));
    Sphere *sphere2 = new Sphere(glm::vec3(17.0, 17.0, -100.0), 3.0, glm::vec3(0, 1, 0));
    Sphere *sphere3 = new Sphere(glm::vec3(0.0, 5.0, -100.0), 10.0, glm::vec3(1, 0, 0));

	//--Add the above to the list of scene objects.
    sceneObjects.push_back(sphere1);
    sceneObjects.push_back(sphere2);
    sceneObjects.push_back(sphere3);
}



int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB );
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(20, 20);
    glutCreateWindow("Raytracer");

    glutDisplayFunc(display);
    initialize();

    glutMainLoop();
    return 0;
}
