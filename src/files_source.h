#pragma once
#ifndef __FILES_SOURCE_HEADER__
#define __FILES_SOURCE_HEADER__

#include "../../SrgGdHelpers/include/__templates.hpp"
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/templates/vector.hpp>
#include <vector>

///
/// This class resolves a path, creates it if it's missing,
/// and will return the actual useable path for further use
/// It can be used solely for that, or as a base class of
/// code the scans this folder's contents.  Note that if
/// the path is app-relative, then it won't really be useful 
/// when running inside the editor.  If it is, in fact,
/// in the editor, it always just returns the configured source.
///
class PathResolver GDX_SUBCLASS(Resource)
{
    GDX_CLASS_PREFIX(PathResolver, Resource);

public:
    PathResolver();
    virtual ~PathResolver() {};

    String getSourceFolder() const { return sourceFolder_; }
    void setSourceFolder(const String source) { sourceFolder_ = source; };

    bool getAppRelative() const { return appRelative_; }
    void setAppRelative(bool newState) { appRelative_ = newState; }

    bool getCreateFolderIfMissing() const { return createFolderIfMissing_; }
    void setCreateFolderIfMissing(bool newSetting) { createFolderIfMissing_ = newSetting; }

    String getActualSourceFolder() const { return folderActual(); }

protected:
    String sourceFolder_;
    bool createFolderIfMissing_;
    bool appRelative_;

    String folderActual() const;
};

/// wraps a file location to make it easier to use between design time and runtime
class FileLocator GDX_SUBCLASS(PathResolver)
{
    GDX_CLASS_PREFIX(FileLocator, PathResolver);

public:
    FileLocator();
    virtual ~FileLocator() {};

    String getBaseFilename() const { return baseFilename_; }
    void setBaseFilename(const String newName) { baseFilename_ = newName; }

    String getResolvedPath() const;

private:
    String baseFilename_;
};

///
/// Contains storage for items after scanning a given folder.
/// This is basically an abstract class for later implementations.
/// By itself, it doesn't do anything -- the list will always
/// be empty.
///
class PathScannerBase GDX_SUBCLASS(PathResolver)
{
    GDX_CLASS_PREFIX(PathScannerBase, PathResolver);

public:
    PathScannerBase();
    virtual ~PathScannerBase() {};

    bool getRecursive() const { return recursive_; }
    void setRecursive(bool newState) { recursive_ = newState; }

    bool getAlwaysRefresh() const { return alwaysRefresh_; }
    void setAlwaysRefresh(bool newState) { alwaysRefresh_ = newState; }

    void clear() { items_.clear(); }
    Array getItems() { return retrieveItems(false); }
    Array getItemsLong() { return retrieveItems(true); }

protected:
    bool recursive_;    
    bool alwaysRefresh_;
    std::vector<String> items_;

    Array retrieveItems(const bool prefixItems = false);

    // override, and put items in items_
    virtual void gatherItems(const String baseFolder) {};
};

///
/// scans a folder to retrieve all subfolders. can be recursive.
///
class DirectoryList GDX_SUBCLASS(PathScannerBase)
{
    GDX_CLASS_PREFIX(DirectoryList, PathScannerBase);

public:
    DirectoryList();
    virtual ~DirectoryList() {};

protected:
    virtual void gatherItems(const String baseFolder);
    void scanRecursive(const String currentFolder, const int64_t subStrStartIndex);
};

class FileList GDX_SUBCLASS(PathScannerBase)
{
    GDX_CLASS_PREFIX(FileList, PathScannerBase);

public:
    FileList();
    virtual ~FileList() {};

    String getSuffixFilter() const { return suffixFilter_; }
    void setSuffixFilter(const String suffixFilter) { suffixFilter_ = suffixFilter; }

protected:
    String suffixFilter_;

    void retrieveFilenames();
    Array getItems(const bool fileNamesOnly = true);
    void ScanFolder(String workFolder);

    virtual void gatherItems(const String baseFolder);
    void scanRecursive(const String currentFolder, const Array filters, const int64_t subStrStartIndex);
};

class PathNamesCollection GDX_SUBCLASS(Resource)
{
    GDX_CLASS_PREFIX(PathNamesCollection, Resource);

public:
    PathNamesCollection();
    virtual ~PathNamesCollection() {};

    Array getItems();
    Array getItemsLong();

    void clear();

    int64_t getCount();
    void setCount(int64_t newCount);

    Ref<PathScannerBase> getScannerAt(const int64_t index) const;
    bool isItemEnabled(const int64_t index) const;

protected:
    struct CollectibleItem {
        Ref<PathScannerBase> Scanner = Ref<PathScannerBase>();
        bool Enabled = true;
    };

    std::vector<CollectibleItem> items_;

    bool _set(const StringName& p_name, const Variant& p_value);
    bool _get(const StringName& p_name, Variant& r_ret) const;
    void _get_property_list(List<PropertyInfo>* p_list) const;
};


#endif /// __FILES_SOURCE_HEADER__
