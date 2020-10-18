/*----------------------------------------------------------
* COSC363  Ray Tracer
*
*  The Plane class
*  This is a subclass of Object, and hence implements the
*  methods intersect() and normal().
-------------------------------------------------------------*/

#include "Plane.h"
#include <math.h>


/**
* Checks if a point pt is inside the current polygon
* Implement a point inclusion test using 
* member variables a, b, c, d.
*/
bool Plane::isInside(glm::vec3 pt)
{
    // Get all the cross products between vectors which make the edges of the plane and the point from each corner

    glm::vec3 aCross = glm::cross(b - a, pt - a);
    glm::vec3 bCross = glm::cross(c - b, pt - b);
    glm::vec3 cCross = glm::cross(d - c, pt - c);
    glm::vec3 dCross = glm::cross(a - d, pt - d);

    glm::vec3 normal = this->normal(pt);

    // Perform the dot product on each resultant cross product.
    float aDot = glm::dot(aCross, normal);
    float bDot = glm::dot(bCross, normal);
    float cDot = glm::dot(cCross, normal);
    float dDot = glm::dot(dCross, normal);

    // If all the dot products are positive, return true, else return false.
    return (aDot > 0 && bDot > 0 && cDot > 0 && dDot > 0);
}

/**
* Plane's intersection method.  The input is a ray (pos, dir). 
*/
float Plane::intersect(glm::vec3 posn, glm::vec3 dir)
{
	glm::vec3 n = normal(posn);
	glm::vec3 vdif = a - posn;
	float vdotn = glm::dot(dir, n);
	if(fabs(vdotn) < 1.e-4) return -1;
    float t = glm::dot(vdif, n)/vdotn;
	if(fabs(t) < 0.0001) return -1;
	glm::vec3 q = posn + dir*t;
	if(isInside(q)) return t;
    else return -1;
}

/**
* Returns the unit normal vector at a given point.
* Compute the plane's normal vector using 
* member variables a, b, c, d.
* The parameter pt is a dummy variable and is not used.
*/
glm::vec3 Plane::normal(glm::vec3 pt)
{
	glm::vec3 bMinusA = b - a;
	glm::vec3 dMinusA = d - a;

	glm::vec3 normal = glm::cross(bMinusA, dMinusA);   // Get the cross product
    normal = glm::normalize(normal);                    // Normalise it
    return normal;
}