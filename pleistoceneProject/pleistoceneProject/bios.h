#pragma once
#include "globals.h"

namespace pleistocene {

namespace graphics { class Graphics; }
namespace options { class GameOptions; }

namespace simulation { class Tile; }

namespace user_interface {


class Bios {
public:
	Bios() noexcept;

	Bios(graphics::Graphics &graphics)  noexcept;

	void clear()  noexcept;
	void update() noexcept;
	void draw(graphics::Graphics &graphics) noexcept;

	void selectTile(simulation::Tile * const tile) noexcept;

private:

	bool _exists = false;

	bool _display = false;

	simulation::Tile *_selectedTile;
	std::vector<std::string> _messages;
	SDL_Rect _displayRect;
	int _textMargin;
	int _textHeight;

	std::string blackPath = "../../content/simpleTerrain/blackOutline.png";

	SDL_Rect _selectionDrawPos;

};


class InfoBar {
public:
	InfoBar() noexcept;

	InfoBar(graphics::Graphics &graphics) noexcept;

	void draw(graphics::Graphics &graphics, const options::GameOptions &options) noexcept;
	void update() noexcept;

private:

	SDL_Rect _displayRect;
	int _textMargin;
	int _textHeight;

	std::vector<std::string> _messages;


};

}//user_interface

}//namespace pleistocene