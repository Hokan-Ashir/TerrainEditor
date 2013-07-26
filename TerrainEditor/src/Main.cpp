/* 
 * File:   Main.cpp
 * Author: hokan
 *
 * Created on 26 Июль 2013 г., 17:49
 */

#include <irrTypes.h>
#include <EDriverTypes.h>
#include "../headers/CMainApplication.h"

#ifdef _MSC_VER
// We'll also define this to stop MSVC complaining about sprintf().
//#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib, "Irrlicht.lib")
#pragma comment(linker, "/subsystem:console /ENTRY:mainCRTStartup")
#endif

irr::s32 main(irr::s32 argc, irr::c8** argv) {
    video::E_DRIVER_TYPE videoDriverType = video::EDT_OPENGL/*EDT_DIRECT3D9*/;
    bool fullScreen = false;

    //video::E_DRIVER_TYPE pVideoDriverType = pVideoDriverChoiceConsole();
    //if (pVideoDriverType == video::EDT_COUNT)
    //	return 1;
    irr::CMainApplication mainApplication(videoDriverType, fullScreen);
    mainApplication.runDevice();
    return 0;
}
