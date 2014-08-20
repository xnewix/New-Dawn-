#ifndef SDL_FONTTEXTURE_H
#define SDL_FONTTEXTURE_H

#include "SDL_Video.h"

class SDLFontManager {
private:
    // multipull font sets?
    // consider std::vector<SDLFontManager> but it is  
    // probably better to index the fontset to use and 
    // then load in those fonts, rather than them all
    
    // in settings;  
    //              unsigned int fontset;
    
    std::vector<TTF_Font*> fonts;
    unsigned int fontset;          // set through settings
public:
    void openFonts() {
        // open fonts and store them in a vector.
    }
};

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
