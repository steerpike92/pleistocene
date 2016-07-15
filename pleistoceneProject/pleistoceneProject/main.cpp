#include "game.h"

int main(int argc, char* args[]) {

	pleistocene::Game game = pleistocene::Game();

	//if (EXCEPTION_HANDLING) {//Closes SDL at exception and prints catch message
	//	try
	//	{
	//		pleistocene::Game game = pleistocene::Game();
	//	}
	//	catch (int exception_num)
	//	{
	//		LOG("Handled Error");
	//		switch (exception_num) {
	//		case(0) : LOG("Startup error"); break;
	//		case(1) : LOG("Image loading error"); break;
	//		case(2) : LOG("Game logic error"); break;
	//		default: LOG("Unkown exception number"); break;
	//		}
	//		system("pause");
	//	}
	//	catch (...) {
	//		LOG("Unhandled Error");
	//		system("pause");
	//	}
	//}

	//else {//Breaks at exception for debugging
	//	pleistocene::Game game = pleistocene::Game();
	//}

	return 0;
}
