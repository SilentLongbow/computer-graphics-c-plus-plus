/*----------------------------------------------------------
* COSC363  Ray Tracer
*
*  The ray class
-------------------------------------------------------------*/

#ifndef H_RAY
#define H_RAY
#include <glm/glm.hpp>
#include <vector>
#include "../scene-objects/SceneObject.h"

class Ray
{

public:
    glm::vec3 source;	//The source point of the ray
	glm::vec3 direction;	//The UNIT direction of the ray
	glm::vec3 intersectPoint;	//The closest point of intersection on the ray
	int intersectIndex;	//The index of the object that gives the closet point of intersection
	float intersectDistance;	//The distance from the source to intersectPoint along the ray.

    Ray()
	{
        source = glm::vec3(0, 0, 0);
        direction = glm::vec3(0, 0, -1);
        intersectPoint = glm::vec3(0, 0, 0);
        intersectIndex = -1;
        intersectDistance = 0;
	}	;
	
    Ray(glm::vec3 point, glm::vec3 direction)
		: source(point), direction(direction)
	{
        intersectPoint = glm::vec3(0, 0, 0);
        intersectIndex = -1;
        intersectDistance = 0;
	} ;

    void normalize();
	void closestPt(std::vector<SceneObject*> &sceneObjects);

};
#endif
