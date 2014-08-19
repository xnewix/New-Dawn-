#ifndef SDL_FONTTEXTURE_H
#define SDL_FONTTEXTURE_H

#include "SDL_Video.h"

class SDLFontTexture {
private:
    std::string textstring;
    SDL_Color   colour;
public:
    SDLFontTexture() {
      
    }
    ~SDLFontTexture() {};
    
    // considering SDL_Surface *surface type
    void convertSurface() {
        SDL_Surface *surface;  
        TTF_Font    *font;
        
        // colour 
        
        // Not sure if this is a good idea here, probably should move this
        // so multipul instances can make use of the same call
        font = TTF_OpenFont(filename, ptsize); 
        
        surface = TTF_RenderText_Blended(font, message, colour);
        
        TTF_CloseFont(font);
        
        // Use the CTexture class to convert surface into a Opengl Texture.
        CTexture texttexture;
    }
    void draw() {
        // draw the texture.
    }
}




#endif
