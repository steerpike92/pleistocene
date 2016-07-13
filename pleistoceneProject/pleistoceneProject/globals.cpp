#include "globals.h"
#include "gameOptions.h"

namespace my {
Vector2::Vector2(Vector2d v2) {
	x = int(v2.x);
	y = int(v2.y);
}

//////////////=======================================
//////////////RECTANGLE
//////////////=======================================

Rectangle::Rectangle() :
	x(-1), y(-1), w(0), h(0) {}

Rectangle::~Rectangle() {}


Rectangle::Rectangle(int x, int y, int w, int h) :
	x(x), y(y), w(w), h(h) {}

Rectangle::Rectangle(SDL_Rect rect) :
	x(rect.x), y(rect.y), w(rect.w), h(rect.h) {}


const SDL_Rect Rectangle::cameraTransform(const double SCALE, const Vector2 C) const {

	SDL_Rect GameRect;

	GameRect.x = (int)round(x * SCALE);
	GameRect.y = (int)round(y * SCALE);
	GameRect.w = (int)ceil((double)w * SCALE);
	GameRect.h = (int)ceil((double)h * (SCALE)*1.05);

	GameRect.x -= C.x;
	GameRect.y -= C.y;
	return GameRect;
}


void Rectangle::moveRect(const Vector2 &S) {
	this->x = S.x;
	this->y = S.y;
}

const Vector2 Rectangle::getCenter() const {
	return Vector2(x + w / 2, y + h / 2);
}

const int Rectangle::getLeft() const { return x; }
const int Rectangle::getRight() const { return x + w; }
const int Rectangle::getTop() const { return y; }
const int Rectangle::getBottom() const { return y + h; }

const int Rectangle::getWidth() const { return w; }
const int Rectangle::getHeight() const { return h; }


void Rectangle::print() const {

	std::cout << "(" << x << "," << y << "," << w << "," << h << ")\n";

}


//////////////=======================================
//////////////ADDRESS
//////////////=======================================


void Address::getOptions(GameOptions &options) {

	Rows = options.getRows();
	Cols = options.getCols();

}

int Address::Rows = FakeIndex;
int Address::Cols = FakeIndex;

Address::Address() {
	r = FakeInt; c = FakeInt; exists = false; i = -FakeIndex;
}


//Normal constructor
Address::Address(int R, int C) {

	//guard against 
	if (R < 0 || R >= Rows) {
		r = FakeInt;
		c = FakeInt;
		exists = false;
		i = FakeIndex;
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
Address::Address(Vector2 v) : Address(v.x, v.y) {}

//call spurious constructor, for sort of made up Address positions that don't correspond to a tile
Address::Address(int R, int C, bool Spurious) {
	exists = false;
	r = R;
	odd = (r % 2 != 0);
	c = C;
	i = FakeIndex;
}

//gets game position at an Address
Vector2 Address::getGamePos() const {
	Vector2 v;
	v.x = (globals::TILE_WIDTH / 2) * (r % 2) + globals::TILE_WIDTH * c;
	v.y = globals::EFFECTIVE_HEIGHT*r;
	return v;
}

Vector2d Address::getLatLonDeg() const {
	Vector2 v = this->getGamePos();

	double _latitude_deg = ((-(double)v.y /
		(globals::EFFECTIVE_HEIGHT*(Rows) / 2)) + 1)*climate::planetary::maxLatitude;
	double _longitude_deg = 360 * v.x /
		(Cols*globals::TILE_WIDTH);

	return Vector2d(_latitude_deg, _longitude_deg);
}

Address Address::adjacent(Direction direction) const {

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
	default:
		LOG("NOT A VALID DIRECTION");
		throw(2);
		return Address(r, c);
	}
}

Address Address::adjacent(int i) const {
	if (i >= 0 && i < 6) {
		return adjacent(static_cast<Direction>(i));
	}
	else {
		LOG("NOT A VALID DIRECTION");
		throw (2);
		return Address(FakeInt, FakeInt, true);
	}
}

int Address::GetRows() { return Rows; }

int Address::GetCols() { return Cols; }



//////////////=======================================
//////////////SIMULATION TIME
//////////////=======================================

bool SimulationTime::_globalTimeExists = false;

SimulationTime::SimulationTime() {

	if (_globalTimeExists) {//set time equal to current global time
		this->_year = _globalTime._year;
		this->_day = _globalTime._day;
		this->_hour = _globalTime._hour;
	}
	else {//Hello global time!
		this->_year = 0;
		this->_day = 0;
		this->_hour = 0;
		_globalTimeExists = true;
	}

}


SimulationTime SimulationTime::_globalTime = SimulationTime();

void SimulationTime::updateGlobalTime() {

	_globalTime._hour++;

	if (_globalTime._hour >= climate::planetary::solarDay_h) {
		_globalTime._hour = 0;
		_globalTime._day++;
		if (_globalTime._day >= climate::planetary::solarYear_d) {
			_globalTime._day = 0;
			_globalTime._year++;
		}
	}
}

std::vector<std::string> SimulationTime::readGlobalTime() {

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


double SimulationTime::getTotalHours() const {
	double hours = 0.0;
	if (this == &_globalTime) {
		hours += _globalTime._hour;
		hours += climate::planetary::solarDay_h*(_globalTime._day);
		hours += climate::planetary::solarYear_d*climate::planetary::solarDay_h*(_globalTime._year);
		return hours;
	}
	else {

		hours += _globalTime._hour - this->_hour;
		hours += climate::planetary::solarDay_h*(_globalTime._day - this->_day);
		hours += climate::planetary::solarYear_d*climate::planetary::solarDay_h*(_globalTime._year - this->_year);
		return hours;
	}
}

double SimulationTime::getTotalDays() const {
	double days = 0.0;
	if (this == &_globalTime) {
		days += (_globalTime._hour) / climate::planetary::solarDay_h;
		days += _globalTime._day;
		days += climate::planetary::solarYear_d*(_globalTime._year);
		return days;
	}
	days += (_globalTime._hour - this->_hour) / climate::planetary::solarDay_h;
	days += _globalTime._day - this->_day;
	days += climate::planetary::solarYear_d*(_globalTime._year - this->_year);
	return days;
}

double SimulationTime::getTotalYears() const {
	double years = 0.0;
	if (this == &_globalTime) {
		years += ((_globalTime._hour) / climate::planetary::solarDay_h) / climate::planetary::solarYear_d;
		years += (_globalTime._day) / climate::planetary::solarYear_d;
		years += _globalTime._year;
		return years;
	}
	years += ((_globalTime._hour - this->_hour) / climate::planetary::solarDay_h) / climate::planetary::solarYear_d;
	years += (_globalTime._day - this->_day) / climate::planetary::solarYear_d;
	years += _globalTime._year - this->_year;

	return years;
}

int SimulationTime::getYear()const {
	if (this == &_globalTime) {
		return _globalTime._year;
	}
	return _globalTime._year - this->_year;
}
int SimulationTime::getDay()const {
	if (this == &_globalTime) {
		return _globalTime._day;
	}
	return _globalTime._day - this->_day;
}
int SimulationTime::getHour()const {
	if (this == &_globalTime) {
		return _globalTime._hour;
	}
	return _globalTime._hour - this->_hour;
}



//////////////=======================================
//////////////UTILITY
//////////////=======================================


double degToRad(double deg) {
	return deg*M_PI / 180.0;
}

double radToDeg(double rad) {
	return rad*180.0 / M_PI;
}


double uniformRandom()
{
	return double(rand()) / double(RAND_MAX + 1.0);
}

}//end namespace my