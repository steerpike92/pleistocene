#pragma once
#include "globals.h"

class Graphics;
class Tile;

class Bios {
public:
	Bios();

	Bios(Graphics &graphics);

	void clear();
	void update();
	void draw(Graphics &graphics);

	void selectTile(Tile * const tile);

private:

	bool _exists = false;

	bool _display = false;

	Tile *_selectedTile;
	std::vector<std::string> _messages;
	SDL_Rect _displayRect;
	int _textMargin;
	int _textHeight;

	std::string blackPath="../../content/simpleTerrain/blackOutline.png";

	SDL_Rect _selectionDrawPos;

};


class InfoBar {
public:
	InfoBar();

	InfoBar(Graphics &graphics);

	void draw(Graphics &graphics);
	void update();

private:

	SDL_Rect _displayRect;
	int _textMargin;
	int _textHeight;

	std::vector<std::string> _messages;


};
