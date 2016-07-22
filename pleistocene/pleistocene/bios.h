#pragma once
#include "globals.h"

namespace pleistocene {

namespace graphics { class Graphics; }
namespace options { class GameOptions; }

namespace user_interface {

const int kTextMargin = 2;
const int kTextHeight = 7;

const int kInfoBarHeight = 20;
const int kDateMargin = 65;

const int kScreenMargin = 0;
const int kBiosWidth = 300;
const int kBiosHeight = 600;


class Bios {
public:
	Bios() noexcept;

	Bios(graphics::Graphics &graphics)  noexcept;

	void update(std::vector<std::string> messages) noexcept;
	void draw(graphics::Graphics &graphics) noexcept;
private:

	bool _exists = false;

	bool _display = false;

	std::vector<std::string> _messages;
	SDL_Rect _displayRect;
};


class InfoBar {
public:
	InfoBar() noexcept;

	InfoBar(graphics::Graphics &graphics) noexcept;

	void draw(graphics::Graphics &graphics) noexcept;
	void update(std::vector<std::string> messages) noexcept;

private:

	SDL_Rect _displayRect;

	std::vector<std::string> _timeReadout;
	std::vector<std::string> _worldReadout;

};

}//user_interface

}//namespace pleistocene