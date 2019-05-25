cmake_minimum_required(VERSION 2.8)

project(lab7)

add_executable(RayTracer.out ray-tracing-files/RayTracer.cpp ray-tracing-files/Ray.cpp ray-tracing-files/SceneObject.cpp ray-tracing-files/Sphere.cpp)



find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
include_directories( ${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS} )

target_link_libraries( RayTracer.out ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} )

