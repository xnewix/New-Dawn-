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
    void openFonts(std::string fontfile) {
        // open fonts and store them in a vector.
        
        // this should only be called once, but if not we
        // need to ensure that any existing data is removed.
        fonts.clear();
        // open file with font data
        // formated as;
        //              fontfilename,  npt
        //              "example.ttf", 12
        
        // N.B colours and text strings are not needed as they
        // are assigned later.
        std::string   tmpfontname;
        unsigned int  tmppt;
        
        // use the filename supplied by the settings class.
        std::ifstream file(fontfile.c_str());
        
        while(file >> tmpfontname >> tmppt) {
            addFont(tmpfontname, tmppt);
        }
        file.close();
    }
    void addFont(std::string fontname, unsigned int npt) {
        // add a font to TTF_Font fonts
        TTF_Font *tmpfont = TTF_OpenFont(fontname.c_str(), npt);
        fonts.push_back(tmpfont);
    }
    TTF_Font *returnfont(unsigned int uid) {
        return fonts[uid];
    }
    void amendFont() {
        // amendFont
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
