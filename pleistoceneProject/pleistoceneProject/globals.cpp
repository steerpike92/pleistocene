#include "globals.h"



my::Vector2::Vector2(my::Vector2d v2) {
	x = int(v2.x);
	y = int(v2.y);
}


my::Rectangle::Rectangle() :x(-1), y(-1), w(0), h(0) {}
my::Rectangle::~Rectangle() {}

my::Rectangle::Rectangle(int x, int y, int w, int h) :
	x(x), y(y), w(w), h(h)
{}

my::Rectangle::Rectangle(SDL_Rect rect) :
	x(rect.x), y(rect.y), w(rect.w), h(rect.h)
{}



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

my::SimulationTime::~SimulationTime() {}

my::SimulationTime my::SimulationTime::_globalTime = my::SimulationTime();

void my::SimulationTime::updateGlobalTime() {

	_globalTime._hour++;

	if (_globalTime._hour >= climate::earth::solarDay_h) {
		_globalTime._hour = 0;
		_globalTime._day++;
		if (_globalTime._day >= climate::earth::solarYear_d) {
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
		hours += climate::earth::solarDay_h*(_globalTime._day);
		hours += climate::earth::solarYear_d*climate::earth::solarDay_h*(_globalTime._year);
		return hours;
	}
	else {
		
		hours += _globalTime._hour - this->_hour;
		hours += climate::earth::solarDay_h*(_globalTime._day - this->_day);
		hours += climate::earth::solarYear_d*climate::earth::solarDay_h*(_globalTime._year - this->_year);
		return hours;
	}
}

double my::SimulationTime::getTotalDays() const{
	double days = 0.0;
	if (this == &_globalTime) {
		days += (_globalTime._hour) / climate::earth::solarDay_h;
		days += _globalTime._day;
		days += climate::earth::solarYear_d*(_globalTime._year);
		return days;
	}
	days += (_globalTime._hour - this->_hour)/climate::earth::solarDay_h;
	days += _globalTime._day - this->_day;
	days += climate::earth::solarYear_d*(_globalTime._year - this->_year);
	return days;
}

double my::SimulationTime::getTotalYears() const{
	double years = 0.0;
	if (this == &_globalTime) {
		years += ((_globalTime._hour) / climate::earth::solarDay_h) / climate::earth::solarYear_d;
		years += (_globalTime._day) / climate::earth::solarYear_d;
		years += _globalTime._year;
		return years;
	}
	years += ((_globalTime._hour - this->_hour) / climate::earth::solarDay_h)/climate::earth::solarYear_d;
	years += (_globalTime._day - this->_day)/climate::earth::solarYear_d;
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


double my::degToRad(double deg) {
	return deg*M_PI / 180.0;
}

double my::radToDeg(double rad) {
	return rad*180.0 / M_PI;
}