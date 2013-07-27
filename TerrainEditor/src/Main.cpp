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

/**
 * TODO
 * brush rotation
 * transparency (brush texture or foreign)
 * brush preview (with/without border)
 * rotation, scale via event receiver; rotation with mouse wheel, scale with ALT + mouse wheel
 * class wrapper
 * setters, getters
 * brush settings window (size, form, border colour, transparency)
 * fixes
 * statistics via text - number of verticies, current LOD, fps, mouse position
 * screenshot function
 * 
 * вшитые значения ambient-света (только тройки RBG-значений, для всех типов цветов),
 * но можно добавлять новые (пока что только ambient). В дополнение есть меню с плавающими полосками,
 * (треугольники на полосках) для выбора значения каждого из частей цвета, real-time действие
 * 
 * it maybe usefull to iherit all XXXXXXManager classes from singleton interface of template-class
 **/

/**
 * Entry point of application
 * 
 * @param argc  number of command line parameters
 * @param argv  double pointer (array of pointers) to u8 of command line parameters
 * @return application exit code (0 if success, any other value otherwise)
 */
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
