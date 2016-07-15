#include "bios.h"
#include "graphics.h"

#include "tile.h"
#include "map.h"

namespace pleistocene {

Bios::Bios() noexcept {}

Bios::Bios(Graphics &graphics) noexcept {

	int screenMargin = 0;
	int width = 250;
	int height = 600;
	this->_displayRect = { globals::SCREEN_WIDTH - (width + screenMargin),screenMargin,width,height };
	_textMargin = 1;
	_textHeight = 7;

	graphics.loadImage(this->blackPath);

	_exists = true;
	_selectedTile = NULL;
}


void Bios::clear() noexcept {
	if (_selectedTile) {
		_selectedTile = NULL;
	}
	_messages.clear();
	_display = false;
}

void Bios::selectTile(Tile * const tile) noexcept {
	_selectedTile = tile;
	_messages = _selectedTile->sendMessages();

	_selectionDrawPos = _selectedTile->getGameRect();

	this->_display = true;

}

void Bios::update() noexcept {
	if (_selectedTile) {
		_messages = _selectedTile->sendMessages();
	}
}

void Bios::draw(Graphics &graphics) noexcept {


	if (_display == NULL) return;

	graphics.blitRectangle(&_displayRect, graphics.Grey, true);//background

	my::Vector2 textPos(_displayRect.x + _textMargin, _displayRect.y + _textMargin);
	my::Vector2 textDimensions;

	textDimensions = graphics.blitText("Bios:", textPos, graphics.White, true);

	textPos.y += textDimensions.y + _textMargin;

	for (std::string &message : _messages) {
		graphics.blitText(message, textPos, graphics.White, true);
		textPos.y += textDimensions.y + _textMargin;;
	}

	SDL_Rect sourceRect = { 0,0,255,255 };
	SDL_Rect dest;

	if (_selectedTile) {

		dest = _selectionDrawPos;

		std::vector<SDL_Rect> onScreenPositions = graphics.getOnScreenPositions(&dest);

		if (onScreenPositions.empty()) {
			return;
		}

		graphics.blitSurface(blackPath, &sourceRect, onScreenPositions);

	}
}



InfoBar::InfoBar() noexcept {}

InfoBar::InfoBar(Graphics &graphics) noexcept {
	int width = globals::SCREEN_WIDTH;
	int height = 18;
	_displayRect = { 0 ,0 ,width,height };
	_textMargin = 2;
}



void InfoBar::update() noexcept {
	_messages.clear();
	_messages = my::SimulationTime::readGlobalTime();
}

void InfoBar::draw(Graphics &graphics) noexcept {
	graphics.blitRectangle(&_displayRect, graphics.Grey, true);

	my::Vector2 textPos(_displayRect.x + _textMargin, _displayRect.y);

	my::Vector2 textDimensions;

	for (std::string &message : _messages) {
		textDimensions = graphics.blitText(message, textPos, graphics.White, true);
		textPos.x += textDimensions.x + _textMargin;
	}

}
}//namespace pleistocene