#include "ofMain.h"
#include "oscLouncher.h"
#include "ofAppNoWindow.h"

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

//#define USE_INPUT

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

static 	void openlog_cb(struct tray_menu* item) {
	(void)item;
	stringstream ss;
	ss << "explorer log.txt";
	system(ss.str().c_str());
	tray_update(&ctray);
}

static 	void editKeys_cb(struct tray_menu* item) {
	(void)item;
	stringstream ss;
	ss << "explorer appList.json";
	system(ss.str().c_str());

	stringstream ss2;
	ss2 << "explorer keyEdito.bat";
	system(ss2.str().c_str());
	tray_update(&ctray);
}

static 	void editKeyMap_cb(struct tray_menu* item) {
	(void)item;
	stringstream ss;
	ss << "explorer key.json";
	system(ss.str().c_str());
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

int _main()
{
#ifdef USE_INPUT
	ofSetupOpenGL(1024, 768, OF_WINDOW);
#else
	ofAppNoWindow window;
	ofSetupOpenGL(&window, 1024, 768, OF_WINDOW);
#endif
	return ofRunApp(new OscLauncher());
}

int main()
{
	//CTray cTray;
	cout << "strat main()" << endl;
	vector<tray_menu> tr;

	tr.push_back(tray_menu());
	tr.back().text = "Hello World with task tray !";
	tr.back().cb = hello_cb;
	tr.back().disabled = 1;
	tr.back().checked = 0;
	tr.back().checkbox = 0;
	tr.back().submenu = NULL;

	tr.push_back(tray_menu());
	tr.back().text = "Open log";
	tr.back().cb = openlog_cb;
	tr.back().disabled = 0;
	tr.back().checked = 0;
	tr.back().checkbox = 0;
	tr.back().submenu = NULL;

	tr.push_back(tray_menu());
	tr.back().text = "Open Json Editor";
	tr.back().cb = editKeys_cb;
	tr.back().disabled = 0;
	tr.back().checked = 0;
	tr.back().checkbox = 0;
	tr.back().submenu = NULL;

	tr.push_back(tray_menu());
	tr.back().text = "Open KeyVK Editor";
	tr.back().cb = editKeyMap_cb;
	tr.back().disabled = 0;
	tr.back().checked = 0;
	tr.back().checkbox = 0;
	tr.back().submenu = NULL;

	tr.push_back(tray_menu());
	tr.back().text = "-";
	tr.back().disabled = 0;
	tr.back().checked = 0;
	tr.back().checkbox = 0;
	tr.back().submenu = NULL;

	tr.push_back(tray_menu());
	tr.back().text = "Quit";
	tr.back().disabled = 0;
	tr.back().checked = 0;
	tr.back().checkbox = 0;
	tr.back().cb = quit_cb;
	tr.back().submenu = NULL;

	tr.push_back(tray_menu());
	tr.back().text = NULL;
	tr.back().disabled = 0;
	tr.back().checked = 0;
	tr.back().checkbox = 0;
	tr.back().submenu = NULL;

	ctray.icon = TRAY_ICON1;
	ctray.tooltip = "oscLauncher";
	ctray.menu = tr.data();

#ifdef USE_INPUT
#else
	ShowWindow(GetConsoleWindow(), SW_HIDE);
#endif

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
