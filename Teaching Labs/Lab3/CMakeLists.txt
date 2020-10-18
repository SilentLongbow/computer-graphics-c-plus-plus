cmake_minimum_required(VERSION 2.8)

project(lab3)

add_executable(Train.out Train.cpp)
add_executable(Shadow.out Shadow.cpp)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
include_directories( ${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS} )

target_link_libraries( Train.out ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} )
target_link_libraries( Shadow.out ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} )
