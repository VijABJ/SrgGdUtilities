
#include "register_types.h"
#include "config_settings.h"
#include "player_profile.h"
#include "files_source.h"
#include "config_store.h"
#include "profile_manager.h"

#include <gdextension_interface.h>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>


void initialize_utility_objects_module(ModuleInitializationLevel p_level)
{
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE)
        return;

    ClassDB::register_class<PathResolver>();
    ClassDB::register_class<FileLocator>();
    ClassDB::register_class<PathScannerBase>();
    ClassDB::register_class<DirectoryList>();
    ClassDB::register_class<FileList>();
    ClassDB::register_class<PathNamesCollection>();

    ClassDB::register_abstract_class<ConfigItem>();
    ClassDB::register_class<BoolConfigItem>();
    ClassDB::register_class<IntConfigItem>();
    ClassDB::register_class<FloatConfigItem>();
    ClassDB::register_class<StringConfigItem>();
    ClassDB::register_class<ConfigItems>();

    ClassDB::register_class<ConfigStore>();

    ClassDB::register_class<NamedStatistics>();
    ClassDB::register_class<PlayerProfile>();
    ClassDB::register_class<ProfileManager>();
}

void uninitialize_utility_objects_module(ModuleInitializationLevel p_level)
{
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE)
        return;
}

using namespace godot;

extern "C" {
    GDExtensionBool GDE_EXPORT SrgUtilsInit(
        const GDExtensionInterfaceGetProcAddress p_interface,
        GDExtensionClassLibraryPtr p_library,
        GDExtensionInitialization* r_initialization)
    {
        //
        godot::GDExtensionBinding::InitObject init_obj(p_interface, p_library, r_initialization);

        init_obj.register_initializer(initialize_utility_objects_module);
        init_obj.register_terminator(uninitialize_utility_objects_module);
        init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

        return init_obj.init();
    }

} // extern "C"