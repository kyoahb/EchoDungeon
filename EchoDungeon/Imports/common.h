#pragma once
/*
Common imports that usually clash.
*/

#include "raylib-cpp/raylib-cpp.hpp"

#include "raylib-imgui-compat/rlImGui.h"
#include <imgui.h>

// Disable deprecated API warnings for enet.h
#define _WINSOCK_DEPRECATED_NO_WARNINGS

// Disable specific Windows networking-related macros
#define WIN32_LEAN_AND_MEAN
#define NOUSER            // All USER defines and routines
#define NOGDI             // All GDI defines and routines

#include <enet/enet.h> // Include ENET
#include "Utils/Logger/Logger.h"