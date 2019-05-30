/*----------------------------------------------------------
* COSC363  Ray Tracer
*
*  The ray class
-------------------------------------------------------------*/
#include "Ray.h"


//Normalizes the direction vector of the current ray to a unit vector
void Ray::normalize()
{
   direction = glm::normalize(direction);
}

//Finds the closest point of intersection of the current ray with scene objects
void Ray::closestPt(std::vector<SceneObject*> &sceneObjects)
{
	glm::vec3 point(0,0,0);
	float min = 1.e+6;
    for(uint i = 0;  i < sceneObjects.size();  i++)
	{
        float t = sceneObjects[i]->intersect(source, direction);
		if(t > 0)        //Intersects the object
		{
			point = source + direction*t;
			if(t < min)
			{
				intersectPoint = point;
				intersectIndex = i;
				intersectDistance = t;
				min = t;
			}
		}
	}
}

