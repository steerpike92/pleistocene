#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <tuple>
#include <map>
#include <algorithm>//map functions
#include "math.h"//pow, trig functions
#include <stdlib.h>//srand, rand
#include <time.h>//time
#include "SDL.h"
#include "SDL_ttf.h"
#include <sstream>
#include <fstream>
#include <stdlib.h>
#include <Eigen/Dense>//linear algebra
#include <memory>

#define DEBUG 1
#define EXCEPTION_HANDLING 1



#if DEBUG
#define LOG(x) std::cout<<x<<std::endl
#else
#define LOG(x)
#endif

namespace globals {
	const int SCREEN_WIDTH = 1000;
	const int SCREEN_HEIGHT = 600;

	const int TILE_WIDTH = 256;

	const int TILE_HEIGHT = 256;

	const int EFFECTIVE_HEIGHT = 190;
	
	const int FPS = 50;//target FPS (20 MS)
	const int MAX_FRAME_TIME = 5*1000/FPS;//cutoff frame time after 100 MS 

}


namespace climate {

	namespace planetary {
		const double tileArea = 100000000.0; //m^2

		const double g = 9.81;		//acceleration of gravity (m/s/s)
		const int maxLatitude = 70;
		const int solarYear_d = 40;	//Length of a solar year in days
		const int solarDay_h = 24;	//length of a solar day in hours
		const int hour_s = 3600;
		//const int hour_s = 14400;
		const double siderealDay_h = double(solarDay_h*solarYear_d) / double(solarYear_d + 1);//hours it takes earth to rotate through 2 pi radians
		const double tilt_rad = 0.4101524;//radians of axial tilt
		//const double tilt_rad = (M_PI / 2)*.6;

		const double solarIntensity = 1.360;//kilo-watts per m2
		const double solarEnergyPerHour = solarIntensity * hour_s;// Kilo-Joules per hour per m2

		const double stephanBoltzmanConstant = 5.67*pow(10, -11);//kw per m2 per K4
		const double emmisionConstantPerHour = stephanBoltzmanConstant * hour_s;

		const double initialTemperatureK = 320.0;
	}

	
	enum DrawType {
		STANDARD_DRAW,
		SURFACE_TEMPERATURE_DRAW,
		SURFACE_AIR_TEMPERATURE_DRAW
	};

	namespace land {
		
		enum elevationType {
			SUBMERGED,
			LOW_LAND,
			MID_LAND,
			HIGH_LAND
		};

		const double gaps = 1000;

		const double landCutoff = 0;
		const double midCutoff = gaps;
		const double highCutoff = 2 * gaps;

		//if unititilized, -3000 meter depth (average sea depth)
		const double defaultDepth = -3 * gaps;

		//high point of 5000 meters above and below sea level
		//rarely realized due to peaking adjustment to noise function
		const double amplitude = 5 * gaps;
	}


	namespace air {
		


	}
}

class GameOptions;

namespace my {



	const double FakeDouble = -6666.0;
	const int FakeInt = -6666;
	const int FakeIndex = -6666;

	double uniformRandom();

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
		Vector2(Vector2d v2);
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
		Rectangle();
		~Rectangle();
		Rectangle(int x, int y, int w, int h);
		Rectangle(SDL_Rect rect);

		const SDL_Rect cameraTransform(const double SCALE, const Vector2 _C) const;

		const Vector2 getCenter() const;

		//takes new position
		void moveRect(const Vector2 &S);

		const int getLeft() const;
		const int getRight() const;
		const int getTop() const;
		const int getBottom() const;

		const int getWidth() const;
		const int getHeight() const;

		void print() const;

		int x, y, w, h;


	};



	class Address {
		
		static int Rows;
		static int Cols;

	public:
		static void getOptions(GameOptions &options);

		int r;//row
		int c;//column
		int i;//tile index (-1 if doesn't exist)
		bool exists = false;//existance or validity flag
		bool odd = false;//odd row flag

		//default constructor
		Address();

		//Normal constructor
		Address(int R, int C);

		//call normal constructor
		Address(Vector2 v);

		//call spurious constructor, for sort of made up Address positions that don't correspond to a tile
		Address(int R, int C, bool Spurious);

		//gets game position at an Address
		Vector2 getGamePos() const;

		Vector2d getLatLonDeg() const;

		Address adjacent(Direction direction) const;

		Address adjacent(int i) const;

		static int GetRows();
		static int GetCols();
	};


	class SimulationTime {
	public:
		//default constructor for 
		SimulationTime();

		static SimulationTime _globalTime;

		static void updateGlobalTime();

		static std::vector<std::string> readGlobalTime();

		double getTotalYears() const;
		double getTotalDays() const;
		double getTotalHours() const;

		int getYear() const;
		int getDay() const;
		int getHour() const;

	private:
		int _year;
		int _day;
		int _hour;

		static bool _globalTimeExists;

	};


	double degToRad(double deg);
	double radToDeg(double rad);
}

