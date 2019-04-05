# READ ME!!
# Assumes that your project has the following structure:
# -lab5
#     -Tower
#         -Tower.cpp etc
#     -Vase
#         -Vase.cpp etc

cmake_minimum_required(VERSION 2.8)

project(lab5)

add_executable(Tower.out Tower/Tower.cpp)
add_executable(Vase.out Vase/Vase.cpp)


find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
include_directories( ${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS} )

target_link_libraries( Tower.out ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} )
target_link_libraries( Vase.out ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} )

