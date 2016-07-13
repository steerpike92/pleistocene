#include "graphics.h"
#include "SDL_image.h"

#include "game.h"
#include "camera.h"
#include "bios.h"
#include "input.h"


Graphics::Graphics() {
	if (SDL_Init(SDL_INIT_EVERYTHING)) {
		LOG("ERROR SDL FAILED TO INIT");
		throw(0);
	}
	else LOG("SDL RUNNING");

	if (TTF_Init() == -1) {
		LOG("ERROR: could not initialize TTF");
		throw(0);
	}
	
	_window=SDL_CreateWindow("Tribe", 50, 50, globals::SCREEN_WIDTH, globals::SCREEN_HEIGHT, 0); 
	_renderer = SDL_CreateRenderer(_window, 0, 0);

	_windowRect = { 0,0,globals::SCREEN_WIDTH, globals::SCREEN_HEIGHT };


	this->_font = TTF_OpenFont("../../content/fonts/Roboto-Black.ttf", 10);
}

Graphics::~Graphics() {
	SDL_DestroyWindow(_window);
	SDL_DestroyRenderer(_renderer);
	SDL_Quit();
	freeAll();
	_renderer = NULL;
	_window = NULL;
}


void Graphics::clear() {
	SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 1);
	SDL_RenderClear(_renderer);
}


void Graphics::loadImage(const std::string pathName) {
	//Add surface to _spriteSheets map
	if (_spriteSheets.count(pathName) == 0) {
		_spriteSheets[pathName] = IMG_Load(pathName.c_str());
		if (_spriteSheets[pathName] == NULL) {
			LOG("Error loading image: ");
			LOG(pathName);
			throw(1);
		}
		//Add texture to _textures map
		_textures[pathName] = SDL_CreateTextureFromSurface(_renderer, _spriteSheets[pathName]);
		if (_textures[pathName] == NULL) {
			LOG("Error creating texture: ");
			LOG(pathName);
			throw(1);
		}
		//Add name to list of names
		_pathNames.push_back(pathName);
	}
}

my::Vector2 Graphics::imageDimensions(const std::string pathName) {
	return my::Vector2(_spriteSheets[pathName]->w, _spriteSheets[pathName]->h);
}

void  Graphics::freeImage(const std::string pathName) {
	_spriteSheets.erase(pathName);
	_textures.erase(pathName);
	_pathNames.erase(std::remove(_pathNames.begin(), _pathNames.end(), pathName), _pathNames.end());
}

void Graphics::freeAll() {
	_spriteSheets.clear();
	_textures.clear();
	_pathNames.clear();
}

void Graphics::setCamera(Camera &camera) {
	_cameraPtr = &camera;
}

void Graphics::setInput(Input &input) {
	_inputPtr = &input;
}

void Graphics::darkenTexture(const std::string &texturePathName, double filter) {
	colorFilter(texturePathName, filter, filter, filter);
}

void Graphics::colorFilter(const std::string &texturePathName, double redFilter,
	double greenFilter, double blueFilter) {
	
	//cull stupid values
	redFilter = std::min(redFilter, 1.0); 
	redFilter = std::max(redFilter, 0.0);

	greenFilter = std::min(greenFilter, 1.0);
	greenFilter = std::max(greenFilter, 0.0);

	blueFilter = std::min(blueFilter, 1.0);
	blueFilter = std::max(blueFilter, 0.0);

	int redMod = int(redFilter * 255);
	int greenMod = int(greenFilter * 255);
	int blueMod = int(blueFilter * 255);

	if (SDL_SetTextureColorMod(_textures[texturePathName], redMod, greenMod, blueMod)) {
		LOG("ERROR, COLOR FILTERING NOT SUPPORTED"); throw(2);
	}
	return;
}

std::vector<SDL_Rect> Graphics::getOnScreenPositions(const SDL_Rect * const gameRectangle, bool screenLocked) {

	std::vector<SDL_Rect> onScreenPositions;

	if (screenLocked) {
		onScreenPositions.push_back(*gameRectangle);
		return onScreenPositions;
	}

	SDL_bool onScreen = SDL_FALSE;
	SDL_Rect localDestRect;
	int loopStart = 0, loopEnd = 0;
	if (1) { loopStart = -1; loopEnd = 1; }//map elements //Stub add option

	for (int i = loopStart; i <= loopEnd; i++) {

		//destinationmy::Rectangle comes in game position
		localDestRect = *gameRectangle;

		//add world looping shift
		localDestRect.x += i * globals::TILE_WIDTH * my::Address::GetCols();

		//Adjust for camera position
		my::Rectangle dest(localDestRect);
		localDestRect = dest.cameraTransform(_cameraPtr->getZoomScale(), _cameraPtr->getCameraPosition());

		//Within view rendering guard
		SDL_Rect bullshit;//SDL demands somewhere to store something irrelevant
		onScreen = SDL_IntersectRect(&localDestRect, &_windowRect, &bullshit);

		if (onScreen) { onScreenPositions.push_back(localDestRect); }
	}
	return onScreenPositions;
}

bool Graphics::blitSurface(const std::string pathName, const SDL_Rect * const sourceRect, std::vector<SDL_Rect> onScreenPositions, 
	double degreesRotated, bool mirrorH, bool mirrorV) {

	bool selectionFlag = false;

	//mirroring
	SDL_RendererFlip mirror = SDL_FLIP_NONE;
	if (mirrorH && mirrorV) mirror = static_cast<SDL_RendererFlip>(SDL_FLIP_HORIZONTAL | SDL_FLIP_VERTICAL);
	else if (mirrorH) mirror = SDL_FLIP_HORIZONTAL;
	else if (mirrorV) mirror = SDL_FLIP_VERTICAL;

	for (SDL_Rect localDestRect : onScreenPositions) {
		if (SDL_RenderCopyEx(_renderer, _textures[pathName], sourceRect, &localDestRect, degreesRotated, NULL, mirror)) {
			LOG("Error rendering: "); LOG(pathName); throw(1);
		}
		if (_selecting) {
			SDL_Rect bullshit;
			if (SDL_EnclosePoints(&_inputPtr->getMousePoint(), 1, &localDestRect, &bullshit)) {
				selectionFlag = true;
			}
		}
	}
	return selectionFlag;
}

void Graphics::blitRectangle(const SDL_Rect * const Rectangle, const SDL_Color color, bool screenLocked) {
	SDL_Rect localDestRect = *Rectangle;

	//zoom/camera position transform
	if (!screenLocked) {
		my::Rectangle dest(localDestRect);
		localDestRect = dest.cameraTransform(_cameraPtr->getZoomScale(), _cameraPtr->getCameraPosition());
	}

	//Within view rendering guard
	SDL_Rect bullshit;
	SDL_bool onScreen = SDL_IntersectRect(&localDestRect, &_windowRect, &bullshit);

	SDL_SetRenderDrawColor(_renderer,color.r,color.g,color.b,color.a);

	if (onScreen) {

		if(SDL_RenderFillRect(_renderer, &localDestRect)){
			LOG("Error rendering my::Rectangle");
			throw(1);
		}
	}
}

my::Vector2 Graphics::blitText(std::string text, my::Vector2 Message_loc, SDL_Color color, bool screenLocked) {
	
	SDL_Rect localDestRect;
	SDL_Texture *tempTexture;
	my::Vector2 textDimensions = my::Vector2(0, 0);
	if(text=="") { return textDimensions; }

	SDL_Surface* textSurface = TTF_RenderText_Solid(_font, text.c_str(), color);
	textDimensions = my::Vector2(textSurface->w, textSurface->h);
	tempTexture = SDL_CreateTextureFromSurface(_renderer, textSurface);
	localDestRect = { Message_loc.x,Message_loc.y,textDimensions.x,textDimensions.y };
	SDL_FreeSurface(textSurface);
	
	if (!screenLocked) {
		my::Rectangle dest(localDestRect);
		localDestRect = dest.cameraTransform(_cameraPtr->getZoomScale(), _cameraPtr->getCameraPosition());
	}

	if (SDL_HasIntersection(&_windowRect, &localDestRect)) {
		SDL_RenderCopy(this->_renderer, tempTexture, NULL, &localDestRect);
		SDL_DestroyTexture(tempTexture);
	}

	return textDimensions;
}



void Graphics::flip() {
	SDL_RenderPresent(_renderer);
}

SDL_Renderer* Graphics::getRenderer() const {
	return _renderer;
}





my::Vector2 Graphics::getSurfaceSize( std::string pathName) const {
	SDL_Surface temp;
	my::Vector2 dimensions;
	temp=*_spriteSheets.at(pathName);
	dimensions.x = temp.w;
	dimensions.y = temp.h;
	return dimensions;
}