#############################################################################
# Setting up the global compiler settings...

ENVP= MACOSX_DEPLOYMENT_TARGET=10.4

# The names of the executables
CXX           = ${ENVP} g++
RM            = rm -f
MAKE          = make -j4

DBG           = off

OS            = $(shell uname -s)

FLAGS_CMN     = -g -fno-stack-protector -pipe -Wno-ctor-dtor-privacy -Wno-multichar -D_REENTRANT -D_GNU_SOURCE -Wno-non-template-friend -DNL_STATIC=1
#dynamic FLAGS_CMN     = -g -pipe -Wno-ctor-dtor-privacy -Wno-multichar -D_REENTRANT -D_GNU_SOURCE -Wno-non-template-friend

FLAGS_DBG_on  = -O0 -finline-functions -DNL_DEBUG
FLAGS_DBG_off = -O3 -ftemplate-depth-24 -funroll-loops -DNL_RELEASE_DEBUG
DIR_DBG_on    = debug
DIR_DBG_off   = release

FLAGS = $(FLAGS_CMN) $(FLAGS_DBG_$(DBG))

EXT = $(HOME)/cvs/external

#old libs
# CHANGE THESE PATHS IF NECESSARY
#ODE_INCLUDE     = $(HOME)/external/ode/include
#ODE_LIB         = $(HOME)/external/ode/lib/ReleaseLib
#NEL_SOURCE      = $(HOME)/cvs/code/nel/src
#NEL_INCLUDE     = $(HOME)/cvs/code/nel/include
#NEL_LIB		= $(HOME)/cvs/code/nel/install/$(DIR_DBG_$(DBG))/lib
#LIBXML2_INCLUDE = $(HOME)/external/libxml2/install/include
#LIBXML2_LIB	= $(HOME)/external/libxml2/install/lib
#LUA_INCLUDE	= /usr/include/lua50
#LUA_LIB		= /usr/lib

SDK=/Developer/SDKs/MacOSX10.4u.sdk

############################################## INCLUDES

CXXFLAGS_Linux = -I$(EXT)/inst/include/libxml2
CXXFLAGS_Darwin = -isysroot ${SDK} -mmacosx-version-min=10.4 -I${SDK}/usr/include/libxml2 -I"/Developer/FMOD Programmers API/api/inc"
CXXFLAGS = $(CXXFLAGS_$(OS)) -I$(EXT)/inst/include -I$(HOME)/cvs/code/nel/include -I$(HOME)/cvs/code/nel/src

############################################## LIBS

LDFLAGS_Linux = -lrt
LDFLAGS_Darwin = -isysroot ${SDK} -Wl,-syslibroot,${SDK} -L${SDK}/usr/X11R6/lib -L"/Developer/FMOD Programmers API/api/lib"
LDFLAGS = $(LDFLAGS_$(OS)) -L$(HOME)/cvs/code/nel/inst/$(DIR_DBG_$(DBG))/lib -L$(EXT)/inst/lib -llua

# uncomment and setup this 2 lines below if you want fmod sound support
FMOD_CXXFLAGS  = -DUSE_FMOD
FMOD_LDFLAGS   = -lfmodex
