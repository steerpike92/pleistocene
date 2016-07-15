#pragma once
#include "globals.h"

namespace pleistocene {
class Tile;
namespace graphics { class Graphics; }
namespace user_interface { class Bios; }
namespace options { class GameOptions; }

class Map {
public:
	Map() noexcept;
	Map(graphics::Graphics &graphics, user_interface::Bios *bios, const options::GameOptions &options) noexcept;

	void generateMap(int seed, const options::GameOptions &options) noexcept;

	void draw(graphics::Graphics &graphics, bool cameraMovementFlag, const options::GameOptions &options) noexcept;
	void update(int elapsedTime) noexcept;

	void simulate() noexcept;

	user_interface::Bios* _bioPtr;

	bool _exists = false;

private:

};
}//namespace pleistocene