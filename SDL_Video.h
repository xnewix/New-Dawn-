#ifndef SDL_VIDEO_H
#define SDL_VIDEO_H

// Include SDL and Opengl headers

#include "SDL.h"

//#include "SDL_ttf.h"
#include "SDL_image.h"
#include "SDL_opengl.h"

//#include <windows.h>							
#include <gl\gl.h>								
#include <gl\glu.h>								
//#include <gl\glaux.h>								
//#include <glut.h>

// C++ headers
#include <cstdlib>
#include <vector>
#include <list>
#include <set>
#include <string>
#include <algorithm>
#include <fstream>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <cmath>

// All SDL/Opengl Video/Display related code should eventualy run from here.

// The following is not currently use in the implementation.

class Opengl_SDL_Video {
private:
	SDL_Window     *window;
	SDL_GLContext  context;
public:
	Opengl_SDL_Video() {
        createWindowandContext();
	}
	~Opengl_SDL_Video() {}

	bool createWindowandContext() {
		window  = SDL_CreateWindow("SDL2/OpenGL Demo", 
			                        SDL_WINDOWPOS_UNDEFINED, 
									SDL_WINDOWPOS_UNDEFINED, 
		                            640, 480, 
		                            SDL_WINDOW_OPENGL|SDL_WINDOW_SHOWN
	    );  

		context = SDL_GL_CreateContext(window);
		return true;
	}
	SDL_Window *returnwindow() {
		return window;
	}
	void UpdateWindow() {
		SDL_GL_SwapWindow(window);
	};
};

// This should probably be merged with Opengl_SDL_Video class

class OpenglVersion {
public:
	void openglversion_output() {
		std::ofstream oglfile("openglversion.txt");

		oglfile << "Vendor:     " << glGetString(GL_VENDOR)                   << std::endl;
	    oglfile << "Renderer:   " << glGetString(GL_RENDERER)                 << std::endl;
		oglfile << "Version:    " << glGetString(GL_VERSION)                  << std::endl;
		
		//oglfile << "Shading:   " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
		oglfile.close();
	}
};
#endif
