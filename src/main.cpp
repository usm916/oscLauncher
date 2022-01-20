//#include <windows.h>
//
#include "ofMain.h"
#include "ofApp.h"
#include "ofAppNoWindow.h"
//
////#include "tray_windows.c"
//
////========================================================================
//int main() {
//	ofAppNoWindow window;
//	ofSetupOpenGL(&window, 1024, 768, OF_WINDOW);
//	//ofSetupOpenGL(1280, 720, OF_WINDOW); // <-------- setup the GL context
//	ShowWindow(GetConsoleWindow(), SW_HIDE);
//	// this kicks off the running of my app
//	// can be OF_WINDOW or OF_FULLSCREEN
//	// pass in width and height too:
//	ofRunApp(new ofApp());
//}

#include <stdio.h>
#include <string.h>
#include <windows.h>


#if defined (_WIN32) || defined (_WIN64)
#define TRAY_WINAPI 1
#elif defined (__linux__) || defined (linux) || defined (__linux)
#define TRAY_APPINDICATOR 1
#elif defined (__APPLE__) || defined (__MACH__)
#define TRAY_APPKIT 1
#endif

#include "tray_windows.c"
#include <thread>
#include <chrono>

#if TRAY_APPINDICATOR
#define TRAY_ICON1 "indicator-messages"
#define TRAY_ICON2 "indicator-messages-new"
#elif TRAY_APPKIT
#define TRAY_ICON1 "icon.png"
#define TRAY_ICON2 "icon.png"
#elif TRAY_WINAPI
#define TRAY_ICON1 "icon.ico"
#define TRAY_ICON2 "icon.ico"
#endif

static struct tray ctray;
//static struct 
//class CTray {
//public:
//	CTray() {};
//	~CTray() {};
//	
//	tray ctray;

static void toggle_cb(struct tray_menu* item) {
		printf("toggle cb\n");
		item->checked = !item->checked;
		tray_update(&ctray);
	}

static 	void hello_cb(struct tray_menu* item) {
	(void)item;
	printf("hello cb\n");
	if (strcmp(ctray.icon, TRAY_ICON1) == 0) {
		ctray.icon = TRAY_ICON2;
	}
	else {
		ctray.icon = TRAY_ICON1;
	}
	tray_update(&ctray);
}

static 	void quit_cb(struct tray_menu* item) {
	ofExit();
	(void)item;
	printf("quit cb\n");
	tray_exit();
}

void submenu_cb(struct tray_menu* item) {
	(void)item;
	printf("submenu: clicked on %s\n", item->text);
	tray_update(&ctray);
}
//};

// Test tray init
//static struct tray tray;
//static struct tray tray = {
//    .icon = TRAY_ICON1,
//#if TRAY_WINAPI
//    .tooltip = "Tray",
//#endif
//    .menu =
//        (struct tray_menu[]) {
//            {.text = "Hello", .cb = hello_cb},
//            {.text = "Checked", .checked = 1, .checkbox = 1, .cb = toggle_cb},
//            {.text = "Disabled", .disabled = 1},
//            {.text = "-"},
//            {.text = "SubMenu",
//             .submenu =
//                 (struct tray_menu[]) {
//                     {.text = "FIRST", .checked = 1, .checkbox = 1, .cb = submenu_cb},
//                     {.text = "SECOND",
//                      .submenu =
//                          (struct tray_menu[]) {
//                              {.text = "THIRD",
//                               .submenu =
//                                   (struct tray_menu[]) {
//                                       {.text = "7", .cb = submenu_cb},
//                                       {.text = "-"},
//                                       {.text = "8", .cb = submenu_cb},
//                                       {.text = NULL}}},
//                              {.text = "FOUR",
//                               .submenu =
//                                   (struct tray_menu[]) {
//                                       {.text = "5", .cb = submenu_cb},
//                                       {.text = "6", .cb = submenu_cb},
//                                       {.text = NULL}}},
//                              {.text = NULL}}},
//                     {.text = NULL}}},
//            {.text = "-"},
//            {.text = "Quit", .cb = quit_cb},
//            {.text = NULL}},
//};

int _main() {
	ofAppNoWindow window;
	ofSetupOpenGL(&window, 1024, 768, OF_WINDOW);
	//ShowWindow(GetConsoleWindow(), SW_HIDE);
	//ofSetupOpenGL(1280, 720, OF_WINDOW);

	return ofRunApp(new ofApp());
}

int main() {
	//CTray cTray;
	cout << "strat main()" << endl;

    tray_menu tr[5];

	tr[0].text = "Hello World with task tray!";
	tr[0].cb = hello_cb;
	tr[0].disabled = 0;
	tr[0].checked = 0;
	tr[0].checkbox = 0;
	//tr[0].context;
	tr[0].submenu = NULL;

	tr[1].text = "toggle checking!";
	tr[1].disabled = 0;
	tr[1].checked = 1;
	tr[1].checkbox = 1;
    tr[1].cb = toggle_cb;
	tr[1].submenu = NULL;

	tr[2].text = "-";
	tr[2].disabled = 0;
	tr[2].checked = 0;
	tr[2].checkbox = 0;
	tr[2].submenu = NULL;

	tr[3].text = "Quit";
	tr[3].disabled = 0;
	tr[3].checked = 0;
	tr[3].checkbox = 0;
	tr[3].cb = quit_cb;
	tr[3].submenu = NULL;

	tr[4].text = NULL;
	tr[4].disabled = 0;
	tr[4].checked = 0;
	tr[4].checkbox = 0;
	tr[4].submenu = NULL;

	ctray.icon = TRAY_ICON1;
	ctray.tooltip = "oscLauncher";
	ctray.menu = tr;

	ShowWindow(GetConsoleWindow(), SW_HIDE);

    if (tray_init(&ctray) < 0) {
        printf("failed to create tray\n");
        return 1;
    }

	thread t(_main);
	
	while (tray_loop(1) == 0) {
		printf("iteration\n");
	}

	if (t.joinable()) {
		t.join();
	}

    return 0;
}
