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
CC=gcc-4.7
CCC=g++-4.7
CXX=g++-4.7
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux-x86
CND_DLIB_EXT=so
CND_CONF=Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/CMasterEventReceiver.o \
	${OBJECTDIR}/CGUIMainMenu.o \
	${OBJECTDIR}/_ext/584002442/CMultiTexturingManager.o \
	${OBJECTDIR}/Main.o \
	${OBJECTDIR}/CSceneNodeAnimatorCameraTerrain.o


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
LDLIBSOPTIONS=-L../../Irrlicht/irrlicht-1.8/lib/Linux ../../Irrlicht/irrlicht-1.8/lib/Linux/libIrrlicht.so `pkg-config --libs gl` `pkg-config --libs xxf86vm`  

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/terraineditor

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/terraineditor: ../../Irrlicht/irrlicht-1.8/lib/Linux/libIrrlicht.so

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/terraineditor: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/terraineditor ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/CMasterEventReceiver.o: CMasterEventReceiver.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -I../../Irrlicht/irrlicht-1.8/include `pkg-config --cflags gl` `pkg-config --cflags xxf86vm`    -MMD -MP -MF $@.d -o ${OBJECTDIR}/CMasterEventReceiver.o CMasterEventReceiver.cpp

${OBJECTDIR}/CGUIMainMenu.o: CGUIMainMenu.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -I../../Irrlicht/irrlicht-1.8/include `pkg-config --cflags gl` `pkg-config --cflags xxf86vm`    -MMD -MP -MF $@.d -o ${OBJECTDIR}/CGUIMainMenu.o CGUIMainMenu.cpp

${OBJECTDIR}/_ext/584002442/CMultiTexturingManager.o: ../IrrlichtTest/CMultiTexturingManager.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/584002442
	${RM} $@.d
	$(COMPILE.cc) -O2 -I../../Irrlicht/irrlicht-1.8/include `pkg-config --cflags gl` `pkg-config --cflags xxf86vm`    -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/584002442/CMultiTexturingManager.o ../IrrlichtTest/CMultiTexturingManager.cpp

${OBJECTDIR}/Main.o: Main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -I../../Irrlicht/irrlicht-1.8/include `pkg-config --cflags gl` `pkg-config --cflags xxf86vm`    -MMD -MP -MF $@.d -o ${OBJECTDIR}/Main.o Main.cpp

${OBJECTDIR}/CSceneNodeAnimatorCameraTerrain.o: CSceneNodeAnimatorCameraTerrain.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -I../../Irrlicht/irrlicht-1.8/include `pkg-config --cflags gl` `pkg-config --cflags xxf86vm`    -MMD -MP -MF $@.d -o ${OBJECTDIR}/CSceneNodeAnimatorCameraTerrain.o CSceneNodeAnimatorCameraTerrain.cpp

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
