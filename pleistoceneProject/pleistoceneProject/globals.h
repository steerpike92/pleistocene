#pragma once

//Strings and such
#define NOMINMAX
#include <Windows.h>
#include <iostream>
#include <string>
#include <sstream>	//string stream
#include <fstream>	//file stream

//STL
#include <vector>
#include <tuple>
#include <map>
#include <algorithm>	
#include <memory>

//Math
#include "math.h"	//pow, trig functions
#include <stdlib.h>	//srand, rand
#include <time.h>	//time
#include <Eigen/Dense>	//linear algebra

//Graphics
#include "SDL.h"	//graphics engine
#include "SDL_ttf.h"	//true type font


#define DEBUG 1

//LOG to console

//#if DEBUG
//#define LOG(x) std::cout<<x<<std::endl
//#else
//#define LOG(x)
//#endif

//LOG to output window

#define LOG( s )				\
{						\
	std::ostringstream os_;			\
	os_ << s << "\n";			\
	OutputDebugString( os_.str().c_str() );	\
}



namespace pleistocene {

namespace options { class GameOptions; }


namespace globals {
const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 600;

const int TILE_WIDTH = 256;

const int TILE_HEIGHT = 256;

const int EFFECTIVE_HEIGHT = 190;

const int FPS = 50;//target FPS (20 MS)
const int MAX_FRAME_TIME = 5 * 1000 / FPS;//cutoff frame time after 100 MS 
}



class options::GameOptions;

namespace my {



const double FakeDouble = -6666.0;
const int FakeInt = -6666;
const int FakeIndex = -6666;

double uniformRandom() noexcept;

enum Direction {
	NORTH_EAST,
	EAST,
	SOUTH_EAST,
	SOUTH_WEST,
	WEST,
	NORTH_WEST
};


class Vector2d;

class Vector2 {
public:
	int x; int y;
	Vector2() { x = 0; y = 0; }
	Vector2(int X, int Y) { x = X; y = Y; }
	Vector2(Vector2d v2) noexcept;
	Vector2(SDL_Point P) { x = P.x; y = P.y; }


	//Addition overload
	Vector2 operator + (Vector2 v2) {
		Vector2 v3;
		v3.x = this->x + v2.x;
		v3.y = this->y + v2.y;
		return v3;
	}
	void operator +=(Vector2 v2) {
		x += v2.x;
		y += v2.y;
	}

	//Subtraction overload
	Vector2 operator - (Vector2 v2) {
		Vector2 v3;
		v3.x = this->x - v2.x;
		v3.y = this->y - v2.y;
		return v3;
	}
	void operator -=(Vector2 v2) {
		x -= v2.x;
		y -= v2.y;
	}

	Vector2 operator * (int a) {
		Vector2 v2;
		v2.x = x*a;
		v2.y = y*a;
		return v2;
	}
	Vector2 operator * (double a) {
		Vector2 v2;
		v2.x = int(x*a);
		v2.y = int(y*a);
		return v2;
	}

	double size() {
		return double(pow(pow(x, 2) + pow(y, 2), .5));
	}

	void print() {
		std::cout << "(" << x << "," << y << ")" << std::endl;
	}

};


class Vector2d {
public:
	double x; double y;

	//Default constructor
	Vector2d() { x = 0.0; y = 0.0; }

	//Specified constructor
	Vector2d(double X, double Y) { x = X; y = Y; }

	//Translator constructor
	Vector2d(Vector2 v2) { x = double(v2.x); y = double(v2.y); }

	//Addition overload
	Vector2d operator + (Vector2d v2) {
		Vector2d v3;
		v3.x = this->x + v2.x;
		v3.y = this->y + v2.y;
		return v3;
	}
	void operator +=(Vector2d v2) {
		x += v2.x;
		y += v2.y;
	}
	//Subtraction overload
	Vector2d operator - (Vector2d v2) {
		Vector2d v3;
		v3.x = this->x - v2.x;
		v3.y = this->y - v2.y;
		return v3;
	}
	void operator -=(Vector2d v2) {
		x -= v2.x;
		y -= v2.y;
	}
	Vector2d operator * (int a) {
		Vector2d v2;
		v2.x = x*double(a);
		v2.y = y*double(a);
		return v2;
	}

	Vector2d operator * (double a) {
		Vector2d v2;
		v2.x = x*a;
		v2.y = y*a;
		return v2;
	}

	double size() {
		return double(pow(pow(x, 2) + pow(y, 2), .5));
	}

	void print() {
		std::cout << "(" << int(x) << "," << int(y) << ")" << std::endl;
	}

};

class MyVector3d {
public:
	double x; double y; double z;

	//Default constructor
	MyVector3d() { x = 0.0; y = 0.0; z = 0.0; }

	//Specified constructor
	MyVector3d(double X, double Y, double Z) { x = X; y = Y; z = Z; }

	//Addition overload
	MyVector3d operator + (MyVector3d v2) {
		MyVector3d v3;
		v3.x = this->x + v2.x;
		v3.y = this->y + v2.y;
		v3.z = this->z + v2.z;
		return v3;
	}
	void operator +=(MyVector3d v2) {
		x += v2.x;
		y += v2.y;
		z += v2.z;
	}
	//Subtraction overload
	MyVector3d operator - (MyVector3d v2) {
		MyVector3d v3;
		v3.x = this->x - v2.x;
		v3.y = this->y - v2.y;
		v3.z = this->z - v2.z;
		return v3;
	}
	void operator -=(MyVector3d v2) {
		x -= v2.x;
		y -= v2.y;
		z -= v2.z;
	}
	MyVector3d operator * (int a) {
		MyVector3d v2;
		v2.x = x*double(a);
		v2.y = y*double(a);
		v2.z = z*double(a);
		return v2;
	}

	MyVector3d operator * (double a) {
		MyVector3d v2;
		v2.x = x*a;
		v2.y = y*a;
		v2.z = z*a;
		return v2;
	}

	double size() {
		return double(pow(pow(x, 2) + pow(y, 2) + pow(z, 2), .5));
	}

	void print() {
		std::cout << "(" << int(x) << "," << int(y) << "," << int(z) << ")" << std::endl;
	}

};

class Rectangle {
public:
	Rectangle() noexcept;
	~Rectangle() noexcept;
	Rectangle(int x, int y, int w, int h) noexcept;
	Rectangle(SDL_Rect rect) noexcept;

	const SDL_Rect cameraTransform(const double SCALE, const Vector2 _C) const noexcept;

	const Vector2 getCenter() const noexcept;

	//takes new position
	void moveRect(const Vector2 &S) noexcept;
	
	const int getLeft() const noexcept;
	const int getRight() const noexcept;
	const int getTop() const noexcept;
	const int getBottom() const noexcept;

	const int getWidth() const noexcept;
	const int getHeight() const noexcept;

	void print() const noexcept;

	int x, y, w, h;


};



class Address {

	static int Rows;
	static int Cols;

public:
	static void getOptions(options::GameOptions &options) noexcept;

	int r;//row
	int c;//column
	int i;//tile index (-1 if doesn't exist)
	bool exists = false;//existance or validity flag
	bool odd = false;//odd row flag

	//default constructor
	Address() noexcept;

	//Normal constructor
	Address(int R, int C) noexcept;

	//call normal constructor
	Address(Vector2 v) noexcept;

	//call spurious constructor, for sort of made up Address positions that don't correspond to a tile
	Address(int R, int C, bool Spurious) noexcept;

	//gets game position at an Address
	Vector2 getGamePos() const noexcept;

	Vector2d getLatLonDeg() const noexcept;

	Address adjacent(Direction direction) const noexcept;

	Address adjacent(int i) const noexcept;

	static int GetRows() noexcept;
	static int GetCols() noexcept;
};


class SimulationTime {
public:
	//default constructor for 
	SimulationTime() noexcept;

	static SimulationTime _globalTime;

	static void updateGlobalTime() noexcept;

	static std::vector<std::string> readGlobalTime() noexcept;

	double getTotalYears() const noexcept;
	double getTotalDays() const noexcept;
	double getTotalHours() const noexcept;

	int getYear() const noexcept;
	int getDay() const noexcept;
	int getHour() const noexcept;

private:
	int _year;
	int _day;
	int _hour;

	static bool _globalTimeExists;

};


double degToRad(double deg) noexcept;
double radToDeg(double rad) noexcept;

}//namespace my

}//namespace pleistocene