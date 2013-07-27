#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux-x86
CND_DLIB_EXT=so
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/src/CEventReceiverManager.o \
	${OBJECTDIR}/src/CGUIMainMenu.o \
	${OBJECTDIR}/src/CGUIManager.o \
	${OBJECTDIR}/src/CMainApplication.o \
	${OBJECTDIR}/src/CScreenShotFactory.o \
	${OBJECTDIR}/src/CTerrainEditor.o \
	${OBJECTDIR}/src/Main.o \
	${OBJECTDIR}/src/TextureTools.o \
	${OBJECTDIR}/src/terrain.editor/CBrushManager.o \
	${OBJECTDIR}/src/terrain.editor/CMultiTexturingManager.o \
	${OBJECTDIR}/src/terrain.editor/CSceneNodeAnimatorCameraTerrain.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-L../../Irrlicht/irrlicht-1.8/lib/Linux ../../Irrlicht/irrlicht-1.8/lib/Linux/libIrrlicht.a `pkg-config --libs gl` `pkg-config --libs xxf86vm`  

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/terraineditor

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/terraineditor: ../../Irrlicht/irrlicht-1.8/lib/Linux/libIrrlicht.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/terraineditor: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/terraineditor ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/src/CEventReceiverManager.o: src/CEventReceiverManager.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -I../../Irrlicht/irrlicht-1.8/include `pkg-config --cflags gl` `pkg-config --cflags xxf86vm`   -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/CEventReceiverManager.o src/CEventReceiverManager.cpp

${OBJECTDIR}/src/CGUIMainMenu.o: src/CGUIMainMenu.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -I../../Irrlicht/irrlicht-1.8/include `pkg-config --cflags gl` `pkg-config --cflags xxf86vm`   -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/CGUIMainMenu.o src/CGUIMainMenu.cpp

${OBJECTDIR}/src/CGUIManager.o: src/CGUIManager.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -I../../Irrlicht/irrlicht-1.8/include `pkg-config --cflags gl` `pkg-config --cflags xxf86vm`   -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/CGUIManager.o src/CGUIManager.cpp

${OBJECTDIR}/src/CMainApplication.o: src/CMainApplication.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -I../../Irrlicht/irrlicht-1.8/include `pkg-config --cflags gl` `pkg-config --cflags xxf86vm`   -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/CMainApplication.o src/CMainApplication.cpp

${OBJECTDIR}/src/CScreenShotFactory.o: src/CScreenShotFactory.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -I../../Irrlicht/irrlicht-1.8/include `pkg-config --cflags gl` `pkg-config --cflags xxf86vm`   -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/CScreenShotFactory.o src/CScreenShotFactory.cpp

${OBJECTDIR}/src/CTerrainEditor.o: src/CTerrainEditor.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -I../../Irrlicht/irrlicht-1.8/include `pkg-config --cflags gl` `pkg-config --cflags xxf86vm`   -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/CTerrainEditor.o src/CTerrainEditor.cpp

${OBJECTDIR}/src/Main.o: src/Main.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -I../../Irrlicht/irrlicht-1.8/include `pkg-config --cflags gl` `pkg-config --cflags xxf86vm`   -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/Main.o src/Main.cpp

${OBJECTDIR}/src/TextureTools.o: src/TextureTools.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -I../../Irrlicht/irrlicht-1.8/include `pkg-config --cflags gl` `pkg-config --cflags xxf86vm`   -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/TextureTools.o src/TextureTools.cpp

${OBJECTDIR}/src/terrain.editor/CBrushManager.o: src/terrain.editor/CBrushManager.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/terrain.editor
	${RM} $@.d
	$(COMPILE.cc) -g -I../../Irrlicht/irrlicht-1.8/include `pkg-config --cflags gl` `pkg-config --cflags xxf86vm`   -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/terrain.editor/CBrushManager.o src/terrain.editor/CBrushManager.cpp

${OBJECTDIR}/src/terrain.editor/CMultiTexturingManager.o: src/terrain.editor/CMultiTexturingManager.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/terrain.editor
	${RM} $@.d
	$(COMPILE.cc) -g -I../../Irrlicht/irrlicht-1.8/include `pkg-config --cflags gl` `pkg-config --cflags xxf86vm`   -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/terrain.editor/CMultiTexturingManager.o src/terrain.editor/CMultiTexturingManager.cpp

${OBJECTDIR}/src/terrain.editor/CSceneNodeAnimatorCameraTerrain.o: src/terrain.editor/CSceneNodeAnimatorCameraTerrain.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/terrain.editor
	${RM} $@.d
	$(COMPILE.cc) -g -I../../Irrlicht/irrlicht-1.8/include `pkg-config --cflags gl` `pkg-config --cflags xxf86vm`   -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/terrain.editor/CSceneNodeAnimatorCameraTerrain.o src/terrain.editor/CSceneNodeAnimatorCameraTerrain.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/terraineditor

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
