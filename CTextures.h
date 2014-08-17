#ifndef CTEXTURES_H
#define CTEXTURES_H

//#include "objectloader.h"

class CTexture {
private:
public:
	GLuint texture;			
    SDL_Surface *surface;	
    GLenum texture_format;
    GLint  nOfColors;

	void createtexture(std::string filename) {
		        if ((surface = SDL_LoadBMP(filename.c_str())) ) {  
	        if ( (surface->w & (surface->w - 1)) != 0 ) {
		        printf("warning: image.bmp's width is not a power of 2\n");
	        } 
	        if ( (surface->h & (surface->h - 1)) != 0 ) {
		        printf("warning: image.bmp's height is not a power of 2\n");
	    }
 
        nOfColors = surface->format->BytesPerPixel;
        if (nOfColors == 4)     // contains an alpha channel
        {
                if (surface->format->Rmask == 0x000000ff)
                        texture_format = GL_RGBA;
                else
                        texture_format = GL_BGRA;
        } else if (nOfColors == 3)     // no alpha channel
        {
                if (surface->format->Rmask == 0x000000ff)
                        texture_format = GL_RGB;
                else
                        texture_format = GL_BGR;
        } else {
                printf("warning: the image is not truecolor..  this will probably break\n");
                // this error should not go unhandled
        }
 
		glGenTextures( 1, &texture ); 
		glBindTexture( GL_TEXTURE_2D, texture ); 
	    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	    
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		glTexImage2D( GL_TEXTURE_2D, 0, nOfColors, surface->w, surface->h, 0,
                      texture_format, GL_UNSIGNED_BYTE, surface->pixels );
        } 
else {
	printf("SDL could not load image.bmp: %s\n", SDL_GetError());
	//SDL_Quit();
	
}    
 
// Free the SDL_Surface only if it was successfully created
if ( surface ) { 
	SDL_FreeSurface( surface );
}
    }
	void usetexture() {
		glBindTexture( GL_TEXTURE_2D, texture );
	}
};

#endif
 
