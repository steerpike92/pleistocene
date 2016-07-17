#include "bios.h"
#include "graphics.h"

#include "tile.h"
#include "world.h"
#include "game-options.h"

namespace pleistocene {
namespace user_interface {

Bios::Bios() noexcept {}

Bios::Bios(graphics::Graphics &graphics) noexcept {

	int screenMargin = 0;
	int width = 250;
	int height = 600;
	this->_displayRect = { globals::kScreenWidth - (width + screenMargin),screenMargin,width,height };
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

void Bios::selectTile(simulation::Tile * const tile) noexcept {
	_selectedTile = tile;

	_selectionDrawPos = _selectedTile->getGameRect();

	this->_display = true;
}

void Bios::update() noexcept {
	if (_selectedTile) {
		//_messages = _selectedTile->sendMessages();
	}
}

void Bios::draw(graphics::Graphics &graphics) noexcept {


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

InfoBar::InfoBar(graphics::Graphics &graphics) noexcept {
	int width = globals::kScreenWidth;
	int height = 18;
	_displayRect = { 0 ,0 ,width,height };
	_textMargin = 2;
}



void InfoBar::update() noexcept {
	_messages.clear();
	_messages = my::SimulationTime::readGlobalTime();
}

void InfoBar::draw(graphics::Graphics &graphics, const options::GameOptions &options) noexcept {
	//bar across top of screen
	graphics.blitRectangle(&_displayRect, graphics.Grey, true);

	my::Vector2 textPosition(_displayRect.x + _textMargin, _displayRect.y);

	my::Vector2 textDimensions;

	int dateMargin = 40; //otherwise it'll jostle stupidly with changing dates. Should have chosen a monospace font

	for (std::string &message : _messages) {
		textDimensions = graphics.blitText(message, textPosition, graphics.White, true);
		textPosition.x += dateMargin;
	}


	std::vector<std::string> draw_messages;
	draw_messages.push_back("   ");

	switch (options._drawSection) {
	case(options::SURFACE) : draw_messages.push_back("SURFACE "); break;
	case(options::HORIZON) : draw_messages.push_back("HORIZON "); break;
	case(options::EARTH) : draw_messages.push_back("EARTH "); break;
	case(options::SEA) : draw_messages.push_back("SEA ");  break;
	case(options::AIR) : draw_messages.push_back("AIR "); break;
	}

	switch (options._statistic) {
	case(options::ELEVATION) : draw_messages.push_back("ELEVATION "); break;
	case(options::TEMPERATURE) : draw_messages.push_back("TEMPERATURE "); break;
	case(options::MATERIAL_PROPERTIES) : draw_messages.push_back("MATERIAL PROPERTIES "); break;
	case(options::FLOW) : draw_messages.push_back("FLOW "); break;
	case(options::MOISTURE) : draw_messages.push_back("MOISTURE "); break;
	}

	for (std::string &message : draw_messages) {
		textDimensions = graphics.blitText(message, textPosition, graphics.White, true);
		textPosition.x += textDimensions.x;
	}

}
}//namespace user_interface
}//namespace pleistocene