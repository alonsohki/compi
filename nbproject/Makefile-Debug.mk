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
FC=g77
AS=as

# Macros
CND_PLATFORM=MinGW-Windows
CND_CONF=Debug
CND_DISTDIR=dist

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=build/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/CClassifier.o \
	${OBJECTDIR}/main.o \
	${OBJECTDIR}/CTranslator.o \
	${OBJECTDIR}/CTokenizer.o


# C Compiler Flags
CFLAGS=-Wall

# CC Compiler Flags
CCFLAGS=-Wall
CXXFLAGS=-Wall

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-Debug.mk dist/Debug/MinGW-Windows/analex.exe

dist/Debug/MinGW-Windows/analex.exe: ${OBJECTFILES}
	${MKDIR} -p dist/Debug/MinGW-Windows
	g++ -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/analex ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/CClassifier.o: CClassifier.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/CClassifier.o CClassifier.cpp

${OBJECTDIR}/main.o: main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/main.o main.cpp

${OBJECTDIR}/CTranslator.o: CTranslator.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/CTranslator.o CTranslator.cpp

${OBJECTDIR}/CTokenizer.o: CTokenizer.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/CTokenizer.o CTokenizer.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/Debug
	${RM} dist/Debug/MinGW-Windows/analex.exe

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
