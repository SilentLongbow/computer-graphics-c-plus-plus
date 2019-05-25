cmake_minimum_required(VERSION 2.8)

project(lab8)

add_executable(RayTracer.out
        lab7-files/Ray.cpp
        lab7-files/RayTracer.cpp
        lab7-files/SceneObject.cpp
        lab7-files/Sphere.cpp
        lab8-files/Plane.cpp)



find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
include_directories( ${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS} )

target_link_libraries( RayTracer.out ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} )

