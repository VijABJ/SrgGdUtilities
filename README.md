# SrgGdUtilities
Godot GDNative utilities

## What's in this project

*First off, **disclaimer** -- this is a work in progress!  There's no guarantee it 
will work for you.  Look at it more like an example/starting point.*

Some utility objects that I'm using for my project, that I thought make decent
examples of how to make C++ GDNative modules.  I figured a working, or at least
compiling, example of this would be useful for someone else to start off off.
And also someone might be able to point out fixes for some of the mistakes.

Currently, this contains, mostly resource types:
* PathResolver - give it a path, it will resolve the path when you run.  It can create folders, or map the folder to the runtime path.
* FileLocator - extends PathResolver to provide a full path and filename.  Same options.
* PathScannerBase, and subclasses - for scanning folders for folders or files.
* PathNamesCollection - wraps multiple scanners into one, and provides a facade for all sources.
* ConfigItems - collection of configuration values.
* PlayerProfile - simple player profile, with an attached collections of settings.
* Json helpers to read/write profiles and configuration items.
* Environment class to wrap everything into one resource you can edit.

## First off, this project requires:

* godot-cpp being installed, built, and configured in a peer folder
* It uses CMake 3.4+ to generate the files (more on this later)
* Requires release build in VS, or it doesn't match up with godot libs.
* Requires my [helper project](https://github.com/VijABJ/SrgGdHelpers/tree/main)
* Visual Studio (version 2022 at the time of this writing)
* Scons (godot requires it)
* Godot 4.2 (latest version at the time of this writing)

From the cmakefile's point of view, it requires the following tree structure:

+godot project
+-extensions
  +-godot-cpp
  +-__projects
  |+-vs2022
  +-SrgGdHelpers
  +-SrgGdUtilities
  +CMakeLists.txt

Inside some folders are .gdignore files so Godot won't scan these folders.

## Build instructions

1. Build godot-cpp using `scons use_static_cpp=yes`
2. Go to the folder __projects/vs2022 (see structure above), and use cmake to create the vs files via `cmake ../../.`
3. Open VS solution, switch to Release.  (Aside, if anyone can help make this work for Debug, that'd be great).  Build.
4. Import the project to godot.  You should be able to add the various resource types.






