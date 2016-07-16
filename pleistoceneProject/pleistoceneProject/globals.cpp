#include "globals.h"
#include "gameOptions.h"
#include "tileClimate.h"
namespace pleistocene {
namespace my {
Vector2::Vector2(Vector2d v2) noexcept {
	x = int(v2.x);
	y = int(v2.y);
}

//////////////=======================================
//////////////RECTANGLE
//////////////=======================================

Rectangle::Rectangle() noexcept :
x(-1), y(-1), w(0), h(0) {}

Rectangle::~Rectangle() noexcept {}


Rectangle::Rectangle(int x, int y, int w, int h) noexcept :
x(x), y(y), w(w), h(h) {}

Rectangle::Rectangle(SDL_Rect rect) noexcept :
	x(rect.x), y(rect.y), w(rect.w), h(rect.h) {}


const SDL_Rect Rectangle::cameraTransform(const double SCALE, const Vector2 C) const noexcept {

	SDL_Rect GameRect;

	GameRect.x = (int)round(x * SCALE);
	GameRect.y = (int)round(y * SCALE);
	GameRect.w = (int)ceil((double)w * SCALE);
	GameRect.h = (int)ceil((double)h * (SCALE)*1.05);

	GameRect.x -= C.x;
	GameRect.y -= C.y;
	return GameRect;
}


void Rectangle::moveRect(const Vector2 &S) noexcept {
	this->x = S.x;
	this->y = S.y;
}

const Vector2 Rectangle::getCenter() const noexcept {
	return Vector2(x + w / 2, y + h / 2);
}

const int Rectangle::getLeft() const noexcept { return x; }
const int Rectangle::getRight() const noexcept { return x + w; }
const int Rectangle::getTop() const noexcept { return y; }
const int Rectangle::getBottom() const noexcept { return y + h; }

const int Rectangle::getWidth() const noexcept { return w; }
const int Rectangle::getHeight() const noexcept { return h; }


void Rectangle::print() const noexcept {

	std::cout << "(" << x << "," << y << "," << w << "," << h << ")\n";

}


//////////////=======================================
//////////////ADDRESS
//////////////=======================================


void Address::getOptions(options::GameOptions &options) noexcept {

	Rows = options.getRows();
	Cols = options.getCols();

}

int Address::Rows = kFakeIndex;
int Address::Cols = kFakeIndex;

Address::Address() noexcept {
	r = kFakeInt; c = kFakeInt; exists = false; i = -kFakeIndex;
}


//Normal constructor
Address::Address(int R, int C) noexcept {

	//guard against 
	if (R < 0 || R >= Rows) {
		r = kFakeInt;
		c = kFakeInt;
		exists = false;
		i = kFakeIndex;
		return;
	}

	exists = true;

	r = R;

	odd = (r % 2 != 0);

	if (C < 0) {
		c = C + Cols;
	}
	else if (C >= Cols) {
		c = C - Cols;
	}
	else c = C;

	i = r*Cols + c;
}

//call normal constructor
Address::Address(Vector2 v) noexcept : Address(v.x, v.y) {}

//call spurious constructor, for sort of made up Address positions that don't correspond to a tile
Address::Address(int R, int C, bool Spurious) noexcept {
	exists = false;
	r = R;
	odd = (r % 2 != 0);
	c = C;
	i = kFakeIndex;
}

//gets game position at an Address
Vector2 Address::getGamePos() const noexcept {
	Vector2 v;
	v.x = (globals::kTileWidth / 2) * (r % 2) + globals::kTileWidth * c;
	v.y = globals::kEffectiveTileHeight*r;
	return v;
}

Vector2d Address::getLatLonDeg() const noexcept {
	Vector2 v = this->getGamePos();

	double _latitude_deg = ((-(double)v.y /
		(globals::kEffectiveTileHeight*(Rows) / 2)) + 1)*simulation::climate::kMaxLatitude;
	double _longitude_deg = 360 * v.x /
		(Cols*globals::kTileWidth);

	return Vector2d(_latitude_deg, _longitude_deg);
}

Address Address::adjacent(Direction direction) const noexcept {

	//even/odd changes vertical column shift
	int colMod = 0;

	if (odd) {
		colMod = 1;
	}

	switch (direction) {

	case(NORTH_EAST) :
		return Address(r - 1, c + colMod);
	case(EAST) :
		return Address(r, c + 1);
	case(SOUTH_EAST) :
		return Address(r + 1, c + colMod);
	case(SOUTH_WEST) :
		return Address(r + 1, c + colMod - 1);
	case(WEST) :
		return Address(r, c - 1);
	case(NORTH_WEST) :
		return Address(r - 1, c + colMod - 1);

	//default:
		//NOEXCEPT LOG("NOT A VALID DIRECTION");exit(EXIT_FAILURE);return Address(r, c);
	default:
		return Address();

	}
}

Address Address::adjacent(int i) const noexcept {
	if (i >= 0 && i < 6) {
		return adjacent(static_cast<Direction>(i));
	}
	else {
		//NOEXCEPT LOG("NOT A VALID DIRECTION");throw (2);
		return Address(kFakeInt, kFakeInt, true);
	}
}

int Address::GetRows() noexcept { return Rows; }

int Address::GetCols() noexcept { return Cols; }



//////////////=======================================
//////////////SIMULATION TIME
//////////////=======================================


bool SimulationTime::_globalTimeExists = false;

SimulationTime::SimulationTime() noexcept {

	if (_globalTimeExists) {//set time equal to current global time
		this->_year = _globalTime._year;
		this->_day = _globalTime._day;
		this->_hour = _globalTime._hour;
	}

	else {
		//global time. Its kind of weird that a class is allowed to have a static member which is an object of that class
		this->_year = 0;
		this->_day = 0;
		this->_hour = 0;
		_globalTimeExists = true;
	}

}


SimulationTime SimulationTime::_globalTime = SimulationTime();

void SimulationTime::updateGlobalTime() noexcept 
{

	_globalTime._hour++;

	if (_globalTime._hour >= simulation::climate::kSolarDay_h) {
		_globalTime._hour = 0;
		_globalTime._day++;
		if (_globalTime._day >= simulation::climate::kSolarYear_d) {
			_globalTime._day = 0;
			_globalTime._year++;
		}
	}
}

void SimulationTime::resetGlobalTime() noexcept
{
	_globalTime._day = 0;
	_globalTime._hour = 0;
	_globalTime._year = 0;
	//TODO ensure any created simulation time objects are destroyed when resetGlobalTime is called (at new world creation only?)
}

std::vector<std::string> SimulationTime::readGlobalTime() noexcept 
{

	std::stringstream stream;
	std::vector<std::string> messages;


	stream << "Year: " << _globalTime._year;
	messages.push_back(stream.str());

	stream = std::stringstream();
	stream << "Day: " << _globalTime._day;
	messages.push_back(stream.str());


	stream = std::stringstream();
	stream << "Hour: " << _globalTime._hour;
	messages.push_back(stream.str());

	
	return messages;
}


double SimulationTime::getTotalHours() const noexcept 
{
	double hours = 0.0;
	if (this == &_globalTime) {
		hours += _globalTime._hour;
		hours += simulation::climate::kSolarDay_h*(_globalTime._day);
		hours += simulation::climate::kSolarYear_d*simulation::climate::kSolarDay_h*(_globalTime._year);
		return hours;
	}
	else {

		hours += _globalTime._hour - this->_hour;
		hours += simulation::climate::kSolarDay_h*(_globalTime._day - this->_day);
		hours += simulation::climate::kSolarYear_d*simulation::climate::kSolarDay_h*(_globalTime._year - this->_year);
		return hours;
	}
}

double SimulationTime::getTotalDays() const noexcept 
{
	double days = 0.0;
	if (this == &_globalTime) {
		days += (_globalTime._hour) / simulation::climate::kSolarDay_h;
		days += _globalTime._day;
		days += simulation::climate::kSolarYear_d*(_globalTime._year);
		return days;
	}
	days += (_globalTime._hour - this->_hour) / simulation::climate::kSolarDay_h;
	days += _globalTime._day - this->_day;
	days += simulation::climate::kSolarYear_d*(_globalTime._year - this->_year);
	return days;
}

double SimulationTime::getTotalYears() const noexcept 
{
	double years = 0.0;
	if (this == &_globalTime) {
		years += ((_globalTime._hour) / simulation::climate::kSolarDay_h) / simulation::climate::kSolarYear_d;
		years += (_globalTime._day) / simulation::climate::kSolarYear_d;
		years += _globalTime._year;
		return years;
	}
	years += ((_globalTime._hour - this->_hour) / simulation::climate::kSolarDay_h) / simulation::climate::kSolarYear_d;
	years += (_globalTime._day - this->_day) / simulation::climate::kSolarYear_d;
	years += _globalTime._year - this->_year;

	return years;
}

int SimulationTime::getYear() const noexcept 
{
	if (this == &_globalTime) {
		return _globalTime._year;
	}
	return _globalTime._year - this->_year;
}
int SimulationTime::getDay() const noexcept
{
	if (this == &_globalTime) {
		return _globalTime._day;
	}
	return _globalTime._day - this->_day;
}
int SimulationTime::getHour() const noexcept 
{
	if (this == &_globalTime) {
		return _globalTime._hour;
	}
	return _globalTime._hour - this->_hour;
}



//////////////=======================================
//////////////UTILITY
//////////////=======================================


double degToRad(double deg) noexcept 
{
	return deg*M_PI / 180.0;
}

double radToDeg(double rad) noexcept 
{
	return rad*180.0 / M_PI;
}


double uniformRandom() noexcept
{
	return double(rand()) / double(RAND_MAX + 1.0);
}

}//namespace my

}//namespace pleistocene