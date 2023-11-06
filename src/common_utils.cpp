#include "common_utils.h"
#include <godot_cpp/classes/dir_access.hpp>


void ensureFolderExists(const String folder)
{
    if(!DirAccess::dir_exists_absolute(folder))
        DirAccess::make_dir_recursive_absolute(folder);
}
