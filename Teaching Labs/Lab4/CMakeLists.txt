cmake_minimum_required(VERSION 2.8)

project(lab4)

add_executable(RailWagon.out RailWagon/RailWagon.cpp)
add_executable(Yard.out Yard/Yard.cpp)
add_executable(Earth.out Earth/Earth.cpp)
add_executable(Skybox.out SkyBox/Skybox.cpp)


find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
include_directories( ${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS} )

target_link_libraries( RailWagon.out ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} )
target_link_libraries( Yard.out ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} )
target_link_libraries( Earth.out ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} )
target_link_libraries( Skybox.out ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} )

