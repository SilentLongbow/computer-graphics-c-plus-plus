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
void SceneObject::setReflective(float factor) {
    reflective = true;
    reflectiveFactor = factor;
}

float SceneObject::getReflectiveFactor() {
    return reflectiveFactor;
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
void SceneObject::setRefractive(float index) {
    refractive = true;
    refractiveIndex = index;
}

float SceneObject::getRefractiveIndex() {
    return refractiveIndex;
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

/**
 * Returns the procedural property of the object.
 */
bool SceneObject::isProceduralPlane() {
    return proceduralPlane;
}

/**
 * Sets the procedural property of the object to true.
 */
void SceneObject::setProceduralPlane(){
    proceduralPlane = true;
    proceduralSphere = false;
}

bool SceneObject::isProceduralSphere() {
    return proceduralSphere;
}

void SceneObject::setProceduralSphere() {
    proceduralSphere = true;
    proceduralPlane = false;
}

bool SceneObject::isTextured() {
    return textured;
}

void SceneObject::setTextured(TextureBMP newTexture) {
    textured = true;
    texture = newTexture;
}

TextureBMP SceneObject::getTexture() {
    return texture;
}