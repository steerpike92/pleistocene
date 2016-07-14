#pragma once
#include "globals.h"

class Camera;
class Game;
class Input;

class Graphics {
public:
	Graphics() noexcept;
	~Graphics() noexcept;
	SDL_Renderer *getRenderer() const noexcept;

	void loadImage(const std::string pathName) noexcept;
	my::Vector2 imageDimensions(const std::string pathName) noexcept;
				
	void freeImage(const std::string pathName) noexcept;
	void freeAll() noexcept;

	void setCamera(Camera &camera) noexcept;
	Camera *_cameraPtr;

	void setInput(Input &input) noexcept;
	Input *_inputPtr;
	
	//preps/clears screen
	void clear() noexcept;
	
	void darkenTexture(const std::string &texturePathName, double filter) noexcept;

	void colorFilter(const std::string &texturePathName, double redFilter, 
		double greenFilter, double blueFilter) noexcept;

	//flag for mouse selection
	bool _selecting;

	std::vector<SDL_Rect> getOnScreenPositions(const SDL_Rect * constgameRectangle, bool screenLocked=false) noexcept;

	//draws to renderer and uses draw position to detect selection
	bool blitSurface(std::string pathName, const SDL_Rect * const sourceRect, std::vector<SDL_Rect> onScreenPositions, 
		double degreesRotated=0.0, bool mirrorH=false, bool mirrorV=false) noexcept;

	void blitRectangle(const SDL_Rect *const Rect, const SDL_Color color, bool screenLocked = false) noexcept;

	my::Vector2 blitText(std::string text, my::Vector2 messageLoc, SDL_Color color, bool screenLocked = true) noexcept;

	//Colors
	SDL_Color Black = { 0,0,0,255 };
	SDL_Color Grey= { 100,100,100,255 };
	SDL_Color White={ 255,255,255,255 };

	//renders to screen
	void flip() noexcept;

	my::Vector2 getSurfaceSize(std::string pathName) const noexcept;

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
