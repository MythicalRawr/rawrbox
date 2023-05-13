
#ifdef _WIN32
	#include <windows.h>
#endif

#include <ui_test/game.hpp>

int main(int argc, char* argv[]) {
#ifdef _WIN32
	SetConsoleTitle("UI test");
#endif

	ui_test::Game engine;
	engine.setFPS(60);
	engine.setTPS(66);

	engine.init();
	engine.run();

	return 0;
}
