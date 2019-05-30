cmake_minimum_required(VERSION 2.8)

project(assignment2)

add_executable(Assignment2.out
        rays/Ray.cpp
        RayTracer.cpp
        scene-objects/SceneObject.cpp
        scene-objects/Sphere.cpp
        scene-objects/Plane.cpp
        texture-loading/TextureBMP.cpp)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
include_directories( ${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS} )

target_link_libraries( Assignment2.out ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} )