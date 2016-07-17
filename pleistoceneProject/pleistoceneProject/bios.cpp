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

}


void Bios::update(std::vector<std::string> messages) noexcept {
	_messages = messages;
	if (_messages.empty()) {
		_display = false;
	}
	else _display = true;
}

void Bios::draw(graphics::Graphics &graphics) noexcept {


	if (_display == false) return;

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

	
}



InfoBar::InfoBar() noexcept {}

InfoBar::InfoBar(graphics::Graphics &graphics) noexcept {
	int width = globals::kScreenWidth;
	int height = 18;
	_displayRect = { 0 ,0 ,width,height };
	_textMargin = 2;
}



void InfoBar::update(std::vector<std::string> messages) noexcept {
	_timeReadout = my::SimulationTime::readGlobalTime();
	_worldReadout = messages;
}

void InfoBar::draw(graphics::Graphics &graphics, const options::GameOptions &options) noexcept {
	//bar across top of screen
	graphics.blitRectangle(&_displayRect, graphics.Grey, true);

	my::Vector2 textPosition(_displayRect.x + _textMargin, _displayRect.y);

	my::Vector2 textDimensions;

	int dateMargin = 40; //otherwise it'll jostle stupidly with changing dates. Should have chosen a monospace font

	for (std::string &message : _timeReadout) {
		textDimensions = graphics.blitText(message, textPosition, graphics.White, true);
		textPosition.x += dateMargin;
	}

	for (std::string &message : _worldReadout) {
		textDimensions = graphics.blitText(message, textPosition, graphics.White, true);
		textPosition.x += textDimensions.x;
	}

}
}//namespace user_interface
}//namespace pleistocene