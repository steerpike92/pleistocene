#pragma once
#include "globals.h"

class Graphics;
class Tile;

class Bios {
public:
	Bios() noexcept;

	Bios(Graphics &graphics)  noexcept;

	void clear()  noexcept;
	void update() noexcept;
	void draw(Graphics &graphics) noexcept;

	void selectTile(Tile * const tile) noexcept;

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
	InfoBar() noexcept;

	InfoBar(Graphics &graphics) noexcept;

	void draw(Graphics &graphics) noexcept;
	void update() noexcept;

private:

	SDL_Rect _displayRect;
	int _textMargin;
	int _textHeight;

	std::vector<std::string> _messages;


};
