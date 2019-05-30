/*----------------------------------------------------------
* COSC363  Ray Tracer
*
*  The object class
*  This is a generic type for storing objects in the scene
*  Sphere, Plane etc. must be defined as subclasses of Object.
*  Being an abstract class, this class cannot be instantiated.
-------------------------------------------------------------*/

#include "SceneObject.h"

glm::vec3 SceneObject::getColor()
{
	return color;
}

void SceneObject::setColor(glm::vec3 col)
{
	color = col;
}

/**
 * Returns the reflective property of the object
 */
bool SceneObject::isReflective() {
    return reflective;
}

/**
 * Sets the reflective property of the object to true
 */
void SceneObject::setReflective() {
    reflective = true;
}

/**
 * Returns the refractive property of the object
 */
bool SceneObject::isRefractive() {
    return refractive;
}

/**
 * Sets the refractive property of the object to true
 */
void SceneObject::setRefractive() {
    refractive = true;
}

/**
 * Returns the transparent property of the object
 */
bool SceneObject::isTransparent() {
    return transparent;
}

/**
 * Sets the transparent property of the object to true
 */
void SceneObject::setTransparent() {
    transparent = true;
}