#include "globals.h"
#include "gameOptions.h"


my::Vector2::Vector2(my::Vector2d v2) {
	x = int(v2.x);
	y = int(v2.y);
}

//////////////=======================================
//////////////RECTANGLE
//////////////=======================================

my::Rectangle::Rectangle() :
	x(-1), y(-1), w(0), h(0) {}

my::Rectangle::~Rectangle() {}


my::Rectangle::Rectangle(int x, int y, int w, int h) :
	x(x), y(y), w(w), h(h){}

my::Rectangle::Rectangle(SDL_Rect rect) :
	x(rect.x), y(rect.y), w(rect.w), h(rect.h){}


const SDL_Rect my::Rectangle::cameraTransform(const double SCALE, const my::Vector2 C) const {

	SDL_Rect GameRect;

	GameRect.x = (int)round(x * SCALE);
	GameRect.y = (int)round(y * SCALE);
	GameRect.w = (int)ceil((double)w * SCALE);
	GameRect.h = (int)ceil((double)h * (SCALE)*1.05);

	GameRect.x -= C.x;
	GameRect.y -= C.y;
	return GameRect;
}


void my::Rectangle::moveRect(const my::Vector2 &S) {
	this->x = S.x;
	this->y = S.y;
}

const my::Vector2 my::Rectangle::getCenter() const {
	return my::Vector2(x + w / 2, y + h / 2);
}

const int my::Rectangle::getLeft() const { return x; }
const int my::Rectangle::getRight() const { return x + w; }
const int my::Rectangle::getTop() const { return y; }
const int my::Rectangle::getBottom() const { return y + h; }

const int my::Rectangle::getWidth() const { return w; }
const int my::Rectangle::getHeight() const { return h; }


void my::Rectangle::print() const {

	std::cout << "(" << x << "," << y << "," << w << "," << h << ")\n";

}


//////////////=======================================
//////////////ADDRESS
//////////////=======================================


void my::Address::getOptions(GameOptions &options) {

	Rows = options.getRows();
	Cols = options.getCols();

}

int my::Address::Rows=my::FakeIndex;
int my::Address::Cols=my::FakeIndex;

my::Address::Address() {
	r = FakeInt; c = FakeInt; exists = false; i = -FakeIndex;
}


//Normal constructor
my::Address::Address(int R, int C) {

		//guard against 
		if (R<0 || R >= Rows) {
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
my::Address::Address(Vector2 v) : Address(v.x, v.y) {}

	//call spurious constructor, for sort of made up Address positions that don't correspond to a tile
my::Address::Address(int R, int C, bool Spurious) {
		exists = false;
		r = R;
		odd = (r % 2 != 0);
		c = C;
		i = FakeIndex;
	}

	//gets game position at an Address
my::Vector2 my::Address::getGamePos() const {
	Vector2 v;
	v.x = (globals::TILE_WIDTH / 2) * (r % 2) + globals::TILE_WIDTH * c;
	v.y = globals::EFFECTIVE_HEIGHT*r;
	return v;
}

my::Vector2d my::Address::getLatLonDeg() const {
	Vector2 v = this->getGamePos();

	double _latitude_deg = ((-(double)v.y /
		(globals::EFFECTIVE_HEIGHT*(Rows) / 2)) + 1)*climate::planetary::maxLatitude;
	double _longitude_deg = 360 * v.x /
		(Cols*globals::TILE_WIDTH);

	return Vector2d(_latitude_deg, _longitude_deg);
}

my::Address my::Address::adjacent(Direction direction) const {

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

my::Address my::Address::adjacent(int i) const {
	if (i >= 0 && i < 6) {
		return adjacent(static_cast<my::Direction>(i));
	}
	else {
		LOG("NOT A VALID DIRECTION");
		throw (2);
		return Address(FakeInt, FakeInt, true);
	}
}

int my::Address::GetRows() { return Rows; }

int my::Address::GetCols() { return Cols; }



//////////////=======================================
//////////////SIMULATION TIME
//////////////=======================================

bool my::SimulationTime::_globalTimeExists = false;

my::SimulationTime::SimulationTime() {

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


my::SimulationTime my::SimulationTime::_globalTime = my::SimulationTime();

void my::SimulationTime::updateGlobalTime() {

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

std::vector<std::string> my::SimulationTime::readGlobalTime() {

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


double my::SimulationTime::getTotalHours() const {
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

double my::SimulationTime::getTotalDays() const{
	double days = 0.0;
	if (this == &_globalTime) {
		days += (_globalTime._hour) / climate::planetary::solarDay_h;
		days += _globalTime._day;
		days += climate::planetary::solarYear_d*(_globalTime._year);
		return days;
	}
	days += (_globalTime._hour - this->_hour)/climate::planetary::solarDay_h;
	days += _globalTime._day - this->_day;
	days += climate::planetary::solarYear_d*(_globalTime._year - this->_year);
	return days;
}

double my::SimulationTime::getTotalYears() const{
	double years = 0.0;
	if (this == &_globalTime) {
		years += ((_globalTime._hour) / climate::planetary::solarDay_h) / climate::planetary::solarYear_d;
		years += (_globalTime._day) / climate::planetary::solarYear_d;
		years += _globalTime._year;
		return years;
	}
	years += ((_globalTime._hour - this->_hour) / climate::planetary::solarDay_h)/climate::planetary::solarYear_d;
	years += (_globalTime._day - this->_day)/climate::planetary::solarYear_d;
	years += _globalTime._year - this->_year;

	return years;
}

int my::SimulationTime::getYear()const {
	if (this == &_globalTime) {
		return _globalTime._year;
	}
	return _globalTime._year - this->_year;
}
int my::SimulationTime::getDay()const {
	if (this == &_globalTime) {
		return _globalTime._day;
	}
	return _globalTime._day - this->_day;
}
int my::SimulationTime::getHour()const {
	if (this == &_globalTime) {
		return _globalTime._hour;
	}
	return _globalTime._hour - this->_hour;
}



//////////////=======================================
//////////////UTILITY
//////////////=======================================


double my::degToRad(double deg) {
	return deg*M_PI / 180.0;
}

double my::radToDeg(double rad) {
	return rad*180.0 / M_PI;
}


double my::uniformRandom()
{
	return double(rand()) / double(RAND_MAX + 1.0);
}
