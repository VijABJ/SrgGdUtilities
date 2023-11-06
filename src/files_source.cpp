#include "files_source.h"
#include "common_utils.h"
#include <godot_cpp/classes/os.hpp>
#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/classes/dir_access.hpp>


void PathResolver::_bind_methods()
{
    DECLARE_PROPERTY(PathResolver, SourceFolder, newFolder, Variant::STRING);
    DECLARE_PROPERTY(PathResolver, CreateFolderIfMissing, newState, Variant::BOOL);
    DECLARE_PROPERTY(PathResolver, AppRelative, newState, Variant::BOOL);
    DECLARE_PROPERTY_READONLY(PathResolver, ActualSourceFolder, Variant::STRING, getActualSourceFolder);
}

PathResolver::PathResolver() : sourceFolder_(String()), createFolderIfMissing_(false), appRelative_(false)
{}

String PathResolver::folderActual() const
{
    OS* os = OS::get_singleton();
    auto result = sourceFolder_;
    if (appRelative_ && !os->has_feature("editor")) {
        auto exePath = os->get_executable_path().get_base_dir();
        result = exePath.path_join(sourceFolder_);
    }

    if (createFolderIfMissing_) {
        ensureFolderExists(result);
    }

    return result;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

void FileLocator::_bind_methods()
{
    DECLARE_PROPERTY(FileLocator, BaseFilename, newName, Variant::STRING);
    DECLARE_PROPERTY_READONLY(FileLocator, ResolvedPath, Variant::STRING, getResolvedPath);
}

FileLocator::FileLocator() : baseFilename_(String())
{}

String FileLocator::getResolvedPath() const
{
    auto workFolder = folderActual();
    return workFolder.path_join(baseFilename_);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////

void PathScannerBase::_bind_methods()
{
    DECLARE_PROPERTY(DirectoryList, Recursive, newState, Variant::BOOL);
    DECLARE_PROPERTY(PathScannerBase, AlwaysRefresh, newState, Variant::BOOL);
    ClassDB::bind_method(D_METHOD("getItems"), &PathScannerBase::getItems);
    ClassDB::bind_method(D_METHOD("getItemsLong"), &PathScannerBase::getItemsLong);
    ClassDB::bind_method(D_METHOD("clear"), &PathScannerBase::clear);
}

PathScannerBase::PathScannerBase() : recursive_(false), alwaysRefresh_(false), items_()
{}

Array PathScannerBase::retrieveItems(const bool prefixItems)
{
    Array result;

    // if we are supposed to be app-relative, then we shouldn't do anything otherwise
    OS* os = OS::get_singleton();
    if (appRelative_ && os->has_feature("editor"))
        return result;

    // bail out on empty source folder
    String workFolder = folderActual();
    if (workFolder.is_empty())
        return result;

    // check if folder exists.  if not, check if we have to create.
    // if both failed, bail out
    if (!DirAccess::dir_exists_absolute(workFolder)) {
        if (createFolderIfMissing_)
            DirAccess::make_dir_recursive_absolute(workFolder);
        else
            return result;
    }

    // clear the list if we're supposed to always refresh
    if (alwaysRefresh_) {
        items_.clear();
    }
    else if (items_.size() == 0) {
        gatherItems(workFolder);
    }

    // in here, we convert our list to Array format
    if (!prefixItems) {
        for (auto& item : items_) {
            result.append(item);
        }
    }
    else {
        for (auto& item : items_) {
            result.append(workFolder.path_join(item));
        }
    }

    return result;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

void DirectoryList::_bind_methods()
{
}

DirectoryList::DirectoryList()
{}

void DirectoryList::gatherItems(const String baseFolder)
{
    auto subStrStartIndex = baseFolder.length() + 1;
    scanRecursive(baseFolder, subStrStartIndex);
}

void DirectoryList::scanRecursive(const String currentFolder, const int64_t subStrStartIndex)
{
    auto dirs = DirAccess::get_directories_at(currentFolder);
    for (int i = 0; i < dirs.size(); i++) {
        auto thisPath = currentFolder.path_join(dirs[i]);
        auto itemToAdd = thisPath.substr(subStrStartIndex);
        items_.push_back(itemToAdd);
        if (recursive_)
            scanRecursive(thisPath, subStrStartIndex);
    }
}


//////////////////////////////////////////////////////////////////////////////////////////////////////

void FileList::_bind_methods()
{
    DECLARE_PROPERTY(FileList, SuffixFilter, newFilter, Variant::STRING);
}

FileList::FileList() : suffixFilter_(String())
{}

void FileList::gatherItems(const String baseFolder)
{
    auto subStrStartIndex = baseFolder.length() + 1;

    Array filters = split(suffixFilter_, ",");
    scanRecursive(baseFolder, filters, subStrStartIndex);
}

void FileList::scanRecursive(const String currentFolder, const Array filters, const int64_t subStrStartIndex)
{
    // we need to get the files first, since this is, after all, a FileList
    auto files = DirAccess::get_files_at(currentFolder);
    for (int i = 0; i < files.size(); i++) {
        bool includeThis = (filters.size() == 0);
        auto file = files[i];
        if (!includeThis) {
            for (int j = 0; j < filters.size(); j++) {
                if (file.ends_with(filters[j])) {
                    includeThis = true;
                    break;
                }
            }
        }
        if (includeThis) {
            auto thisPath = currentFolder.path_join(file);
            items_.push_back(thisPath.substr(subStrStartIndex));
        }
    }

    // go down a directory level if we're recursive
    if (recursive_) {
        auto dirs = DirAccess::get_directories_at(currentFolder);
        for (int i = 0; i < dirs.size(); i++) {
            auto thisPath = currentFolder.path_join(dirs[i]);
            if (recursive_)
                scanRecursive(thisPath, filters, subStrStartIndex);
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////

void PathNamesCollection::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("getItems"), &PathNamesCollection::getItems);
    ClassDB::bind_method(D_METHOD("getItemsLong"), &PathNamesCollection::getItemsLong);
    ClassDB::bind_method(D_METHOD("clear"), &PathNamesCollection::clear);
    ClassDB::bind_method(D_METHOD("setCount", "newCount"), &PathNamesCollection::setCount);
    ClassDB::bind_method(D_METHOD("getCount"), &PathNamesCollection::getCount);

    ClassDB::bind_method(D_METHOD("getScannerAt", "index"), &PathNamesCollection::getScannerAt);
    ClassDB::bind_method(D_METHOD("isItemEnabled", "index"), &PathNamesCollection::isItemEnabled);

    //ADD_ARRAY_COUNT("items_", "itemCount", "setCount", "getCount", "item_");
    ClassDB::add_property(get_class_static(),
        PropertyInfo(Variant::INT, "itemCount", PROPERTY_HINT_NONE, "",
            PROPERTY_USAGE_DEFAULT | PROPERTY_USAGE_ARRAY, "Items,items_"),
        "setCount", "getCount");
}

PathNamesCollection::PathNamesCollection() : items_()
{
}

int64_t PathNamesCollection::getCount()
{
    return items_.size();
}

void PathNamesCollection::setCount(int64_t newCount)
{
    if (items_.size() != newCount) {
        items_.resize(newCount);
        notify_property_list_changed();
    }
}

Array PathNamesCollection::getItems()
{
    Array result;

    for (auto& item : items_) {
        if (item.Enabled) {
            result.append_array(item.Scanner->getItems());
        }
    }

    return result;
}

Array PathNamesCollection::getItemsLong()
{
    Array result;

    for (auto& item : items_) {
        if (item.Enabled) {
            result.append_array(item.Scanner->getItemsLong());
        }
    }

    return result;
}

void PathNamesCollection::clear() {
    //if (primary_.is_valid())
    //    primary_->clear();
    //if (secondary_.is_valid())
    //    secondary_->clear();
}

Ref<PathScannerBase> PathNamesCollection::getScannerAt(const int64_t index) const
{
    if (index >= 0 && index < items_.size())
        return items_[index].Scanner;

    return Ref<PathScannerBase>();
}
bool PathNamesCollection::isItemEnabled(const int64_t index) const
{
    if (index >= 0 && index < items_.size())
        return items_[index].Enabled;

    return false;
}

void PathNamesCollection::_get_property_list(List<PropertyInfo>* p_list) const
{
    DEBUG("_get_property_list()");
    for (int i = 0; i < items_.size(); i++) {
        DEBUG(vformat("item_%d", i));
        PropertyInfo pi = PropertyInfo(Variant::OBJECT, vformat("item_%d/Scanner", i), PROPERTY_HINT_RESOURCE_TYPE, "PathScannerBase");
        pi.usage &= ~(getScannerAt(i).is_null() ? PROPERTY_USAGE_STORAGE : 0);
        p_list->push_back(pi);

        pi = PropertyInfo(Variant::BOOL, vformat("item_%d/Enabled", i));
        pi.usage &= ~(isItemEnabled(i) ? PROPERTY_USAGE_STORAGE : 0);
        p_list->push_back(pi);
    }
}

bool PathNamesCollection::_set(const StringName& p_name, const Variant& p_value) {
    //DEBUG(vformat("_set(%s)", String(p_name)));
    auto components = String(p_name).split("/", true, 2);
    if (components.size() >= 2 && components[0].begins_with("item_") && components[0].trim_prefix("item_").is_valid_int()) {
        int item_index = components[0].trim_prefix("item_").to_int();
        if (components[1] == "Scanner") {
            auto scanner = (Ref<PathScannerBase>) p_value;
            items_[item_index].Scanner = scanner;
            return true;
        }
        else if (components[1] == "Enabled") {
            auto enabled = (bool)p_value;
            items_[item_index].Enabled = enabled;
            return true;
        }
    }
    return false;
}

bool PathNamesCollection::_get(const StringName& p_name, Variant& r_ret) const {
    //DEBUG(vformat("_get(%s)", String(p_name)));
    auto components = String(p_name).split("/", true, 2);
    if (components.size() >= 2 && components[0].begins_with("item_") && components[0].trim_prefix("item_").is_valid_int()) {
        int item_index = components[0].trim_prefix("item_").to_int();
        if (components[1] == "Scanner") {
            r_ret = items_[item_index].Scanner;
            return true;
        }
        else if (components[1] == "Enabled") {
            r_ret = items_[item_index].Enabled;
            return true;
        }
    }
    return false;
}


