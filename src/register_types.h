#pragma once
#ifndef __SO_REGISTRATION_HEADER__
#define __SO_REGISTRATION_HEADER__

#include <godot_cpp/core/class_db.hpp>
using namespace godot;

void initialize_utility_objects_module(ModuleInitializationLevel p_level);
void uninitialize_utility_objects_module(ModuleInitializationLevel p_level);

#endif /// __SO_REGISTRATION_HEADER__