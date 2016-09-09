/*!
 * \brief   libhelium
 * \author  Fabio Dalla Libera
 * \date    2012.06.14
 * \version 0.2
 * Release_flags g
 */

#ifndef HE_GLHEADER_PRIVATE
#define HE_GLHEADER_PRIVATE

#ifdef _WIN32
#include <windows.h>
#endif

#ifdef __APPLE__
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#  include <GLUT/glut.h>
#else
#include <GL/gl.h>	// Header File For The OpenGL32 Library
#include <GL/glut.h>    // Header File For The GLUT Library 
#endif

#ifdef _WIN32
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#else

#ifdef __APPLE__
#  include <OpenGL/glu.h>
#else
#include <GL/glu.h>	// Header File For The GLu32 Library
#endif

#endif

#endif
