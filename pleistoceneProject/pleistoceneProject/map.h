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
	Map(graphics::Graphics &graphics, user_interface::Bios *bios, options::GameOptions &options) noexcept;

	void generateMap(int seed) noexcept;

	void draw(graphics::Graphics &graphics, bool cameraMovementFlag) noexcept;
	void update(int elapsedTime) noexcept;

	void simulate() noexcept;

	user_interface::Bios* _bioPtr;

	bool _exists = false;

	static climate::DrawType getDrawType() noexcept;
	void setDrawType(int drawNumber) noexcept;
private:

	static climate::DrawType _drawType;

	//tracks sea level rise and fall
	int _totalElevationChange = 0;
};
}//namespace pleistocene