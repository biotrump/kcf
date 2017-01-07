cmake_minimum_required(VERSION 2.8)

project(kcf_tracker)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra -pedantic -O3 -Wno-long-long -fno-omit-frame-pointer")

#
find_package(OpenMP)
if(OPENMP_FOUND)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
	add_definitions(-DENABLE_OPENMP)
endif()

#QT5
# Tell CMake to run moc when necessary:
set(CMAKE_AUTOMOC ON)
# As moc files are generated in the binary dir, tell CMake
# to always look for includes there:
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Widgets finds its own dependencies (QtGui and QtCore).
find_package(Qt5Widgets)
#  find_package(Qt5Declarative)

# The Qt5Widgets_INCLUDES also includes the include directories for
# dependencies QtCore and QtGui
include_directories(${Qt5Widgets_INCLUDES})

# We need add -DQT_WIDGETS_LIB when using QtWidgets in Qt 5.
add_definitions(${Qt5Widgets_DEFINITIONS})

#openCV
#set(OpenCV_DIR $ENV{OPENCV_OUT})
#set(OpenCV_DIR "/home/thomas/opencv/build_2.4.13/debug")
set(OpenCV_DIR "/home/thomas/build/bcv-x86.gl/out/opencv/2.4.x-static/x86_64")
message("++++OpenCV_DIR:${OpenCV_DIR}")
FIND_PACKAGE( OpenCV REQUIRED)
if(OpenCV_FOUND)
	message("found")
	message( "OpenCV_INCLUDE_DIRS:${OpenCV_INCLUDE_DIRS}" )
	message( "OpenCV_INCLUDES:${OpenCV_INCLUDES}")
	message( "OpenCV_LIBRARIES:${OpenCV_LIBRARIES}")
	message( "OpenCV_LIBS:${OpenCV_LIBS}")
	message( "OpenCV_DEFINITIONS:${OpenCV_DEFINITIONS}")
else()
	message(FATAL_ERROR "openCV not foud")
endif()

message("*** OpenCV_LIBS:${OpenCV_LIBS}")
message("*** OpenCV_LIBRARIES:${OpenCV_LIBRARIES}")
	
#FIND_PACKAGE( OpenCV REQUIRED )
link_directories ( ${OpenCV_LIB_DIR} )
#MESSAGE(STATUS "OpenCV_LIB_DIR: ${OpenCV_LIB_DIR}")

IF ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
ENDIF ()

include_directories ( ${OpenCV_INCLUDE_DIRS} )
MESSAGE(STATUS "OpenCV_INCLUDE_DIRS: ${OpenCV_INCLUDE_DIRS}")

INCLUDE_DIRECTORIES( ${CMAKE_BINARY_DIR}/)
include_directories(${CMAKE_SOURCE_DIR}/src/ ${CMAKE_SOURCE_DIR}/src/piotr_fhog ${CMAKE_SOURCE_DIR}/src/cn)

add_subdirectory(src)

add_executable(kcf_vot main_vot.cpp vot.hpp)
target_link_libraries(kcf_vot ${OpenCV_LIBS} kcf)

add_executable(kcf_cv main.cpp)
qt5_use_modules(kcf_cv Widgets  OpenGL Test Concurrent)
target_link_libraries(kcf_cv ${OpenCV_LIBS} kcf)

# Try to find TraX header and library ...
FIND_FILE(TRAX_HEADER NAMES trax.h)
IF (EXISTS ${TRAX_HEADER})
    GET_FILENAME_COMPONENT(TRAX_DIR ${TRAX_HEADER} DIRECTORY)
    ADD_DEFINITIONS(-DTRAX) # Added to enable TraX protocol support
    LINK_DIRECTORIES(${TRAX_DIR}) # Add library directory
    INCLUDE_DIRECTORIES(${TRAX_DIR}) # Add includes directory

    add_executable(kcf_trax main_trax.cpp)
    target_link_libraries(kcf_trax ${OpenCV_LIBS} kcf trax trax_opencv)
ENDIF()


