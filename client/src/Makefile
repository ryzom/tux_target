#############################################################################
# Setting up the compiler settings...

# Global variables
include ../../Variables.mk

ifeq (Objects.mk,$(wildcard Objects.mk))
include Objects.mk
endif

# The flags for the C++ compiler
CXXFLAGS_CLIENT_Linux =
CXXFLAGS_CLIENT_Darwin = -arch ppc -arch i386

CXXFLAGS       += $(FLAGS) $(FMOD_CXXFLAGS) \
		-I../../common

CXXFLAGS_FULL = $(CXXFLAGS) $(CXXFLAGS_CLIENT_$(OS))

# The flags for the linker
LDFLAGS_CLIENT_Linux = -Wl,--start-group -lnel_drv_opengl -lnel3d -Wl,--end-group
LDFLAGS_CLIENT_Darwin = -framework CoreFoundation -dylib_file /System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib:/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib -arch ppc -arch i386 -framework ApplicationServices

LDFLAGS        += $(FMOD_LDFLAGS) $(LDFLAGS_CLIENT_$(OS)) \
		-lnel_drv_opengl -lnel3d \
		-lfreetype \
		-lX11 -lXxf86vm -lz \
		-lnelnet -lnelmisc -lxml2 -lcurl -ljpeg -lGL


#############################################################################

TARGET		= client


#############################################################################
# Rules

include ../../Rules.mk
