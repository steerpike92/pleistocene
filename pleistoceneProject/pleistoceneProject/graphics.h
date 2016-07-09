#pragma once
#include "globals.h"

class Camera;
class Game;
class Input;

class Graphics {
public:
	Graphics();
	~Graphics();
	SDL_Renderer *getRenderer() const;

	void loadImage(const std::string pathName);
	MyVector2 imageDimensions(const std::string pathName);
				
	void freeImage(const std::string pathName);
	void freeAll();

	void setCamera(Camera &camera);
	Camera *_cameraPtr;

	void setInput(Input &input);
	Input *_inputPtr;
	
	//preps/clears screen
	void clear();
	
	void darkenTexture(const std::string &texturePathName, double filter);

	void colorFilter(const std::string &texturePathName, double redFilter, 
		double greenFilter, double blueFilter);

	//flag for mouse selection
	bool _selecting;

	std::vector<SDL_Rect> getOnScreenPositions(const SDL_Rect * constgameRectangle, bool screenLocked=false);

	//draws to renderer and uses draw position to detect selection
	bool blitSurface(std::string pathName, const SDL_Rect * const sourceRect, std::vector<SDL_Rect> onScreenPositions, 
		double degreesRotated=0.0, bool mirrorH=false, bool mirrorV=false);

	void blitRectangle(const SDL_Rect *const rectangle, const SDL_Color color, bool screenLocked = false);

	MyVector2 blitText(std::string text, MyVector2 messageLoc, SDL_Color color, bool screenLocked = true);

	//Colors
	SDL_Color Black = { 0,0,0,255 };
	SDL_Color Grey= { 100,100,100,255 };
	SDL_Color White={ 255,255,255,255 };

	//renders to screen
	void flip();

	MyVector2 getSurfaceSize(std::string pathName) const;

private:
	SDL_Window *_window;
	SDL_Rect _windowRect;
	SDL_Renderer *_renderer;


	//Fonts
	TTF_Font *_font;


	std::map <std::string, SDL_Surface*> _spriteSheets;
	std::map <std::string, SDL_Texture*> _textures;
	std::vector <std::string> _pathNames;
};
