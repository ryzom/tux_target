// This file is part of Mtp Target.
// Copyright (C) 2008 Vialek
// 
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
// 
// Vianney Lecroart - gpl@vialek.com

#define USE_JPEG

#define CURL_STATICLIB

#include "nel/misc/types_nl.h"

#ifdef NL_OS_WINDOWS
#	define NOMINMAX
#	include <winsock2.h>
#	include <windows.h>
#endif

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <list>
#include <deque>
#include <string>
#include <vector>
#include <sstream>
#include <utility>
#include <algorithm>
#include <exception>

#include <nel/misc/i18n.h>
#include <nel/misc/file.h>
#include <nel/misc/path.h>
#include <nel/misc/debug.h>
#include <nel/misc/vector.h>
#include <nel/misc/matrix.h>
#include <nel/misc/common.h>
#include <nel/misc/events.h>
#include <nel/misc/bsphere.h>
#include <nel/misc/fast_mem.h>
#include <nel/misc/singleton.h>
#include <nel/misc/config_file.h>
#include <nel/misc/system_info.h>
#include <nel/misc/mem_displayer.h>

#include <nel/3d/u_scene.h>
#include <nel/3d/u_camera.h>
#include <nel/3d/u_driver.h>
#include <nel/3d/u_texture.h>
#include <nel/3d/u_instance.h>
#include <nel/3d/u_text_context.h>
#include <nel/3d/u_particle_system_instance.h>
#include <nel/3d/u_visual_collision_manager.h>

using namespace std;

#ifdef NL_OS_WINDOWS
#	define USE_FMOD
#	include <fmodex/fmod.hpp>
#	include <fmodex/fmod_errors.h>
#elif defined(USE_FMOD)
#	include <fmod.hpp>
#	include <fmod_errors.h>
#endif


// must be the first mtp target include
#include "../../common/constant.h"

#include "task.h"
