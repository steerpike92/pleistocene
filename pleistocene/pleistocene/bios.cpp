#include "bios.h"
#include "graphics.h"

#include "tile.h"
#include "world.h"
#include "game-options.h"

namespace pleistocene {
namespace user_interface {

Bios::Bios() noexcept {}

Bios::Bios(graphics::Graphics &graphics) noexcept {

	
	this->_displayRect = { globals::kScreenWidth - (kBiosWidth + kScreenMargin),kScreenMargin+kInfoBarHeight, kBiosWidth,kBiosHeight };
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

	my::Vector2 textPos(_displayRect.x + kTextMargin, _displayRect.y + kTextMargin);
	my::Vector2 textDimensions;

	textDimensions = graphics.blitText("Bios:", textPos, graphics.White, true);

	textPos.y += textDimensions.y + kTextMargin;

	for (std::string &message : _messages) {
		graphics.blitText(message, textPos, graphics.White, true);
		textPos.y += textDimensions.y + kTextMargin;
	}
}



InfoBar::InfoBar() noexcept {}

InfoBar::InfoBar(graphics::Graphics &graphics) noexcept {
	int width = globals::kScreenWidth;
	int height = 20;
	_displayRect = { 0 ,0 ,width,height };
}



void InfoBar::update(std::vector<std::string> messages) noexcept {
	_timeReadout = my::SimulationTime::readGlobalTime();
	_worldReadout = messages;
}

void InfoBar::draw(graphics::Graphics &graphics) noexcept {
	//bar across top of screen
	graphics.blitRectangle(&_displayRect, graphics.Grey, true);

	my::Vector2 textPosition(_displayRect.x + kTextMargin, _displayRect.y);

	my::Vector2 textDimensions;

	for (std::string &message : _timeReadout) {
		graphics.blitText(message, textPosition, graphics.White, true);
		textPosition.x += kDateMargin;
	}

	for (std::string &message : _worldReadout) {
		textDimensions = graphics.blitText(message, textPosition, graphics.White, true);
		textPosition.x += textDimensions.x+kTextMargin;
	}

}
}//namespace user_interface
}//namespace pleistocene