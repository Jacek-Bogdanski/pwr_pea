# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.25

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "C:\Program Files\JetBrains\CLion 2023.1.1\bin\cmake\win\x64\bin\cmake.exe"

# The command to remove a file.
RM = "C:\Program Files\JetBrains\CLion 2023.1.1\bin\cmake\win\x64\bin\cmake.exe" -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = C:\Users\busin\CLionProjects\pwr_pea\pea_2

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = C:\Users\busin\CLionProjects\pwr_pea\pea_2\cmake-build-release

# Include any dependencies generated for this target.
include CMakeFiles/pea_2.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/pea_2.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/pea_2.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/pea_2.dir/flags.make

CMakeFiles/pea_2.dir/main.cpp.obj: CMakeFiles/pea_2.dir/flags.make
CMakeFiles/pea_2.dir/main.cpp.obj: C:/Users/busin/CLionProjects/pwr_pea/pea_2/main.cpp
CMakeFiles/pea_2.dir/main.cpp.obj: CMakeFiles/pea_2.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\Users\busin\CLionProjects\pwr_pea\pea_2\cmake-build-release\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/pea_2.dir/main.cpp.obj"
	C:\PROGRA~1\JETBRA~1\CLION2~1.1\bin\mingw\bin\G__~1.EXE $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/pea_2.dir/main.cpp.obj -MF CMakeFiles\pea_2.dir\main.cpp.obj.d -o CMakeFiles\pea_2.dir\main.cpp.obj -c C:\Users\busin\CLionProjects\pwr_pea\pea_2\main.cpp

CMakeFiles/pea_2.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/pea_2.dir/main.cpp.i"
	C:\PROGRA~1\JETBRA~1\CLION2~1.1\bin\mingw\bin\G__~1.EXE $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E C:\Users\busin\CLionProjects\pwr_pea\pea_2\main.cpp > CMakeFiles\pea_2.dir\main.cpp.i

CMakeFiles/pea_2.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/pea_2.dir/main.cpp.s"
	C:\PROGRA~1\JETBRA~1\CLION2~1.1\bin\mingw\bin\G__~1.EXE $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S C:\Users\busin\CLionProjects\pwr_pea\pea_2\main.cpp -o CMakeFiles\pea_2.dir\main.cpp.s

# Object files for target pea_2
pea_2_OBJECTS = \
"CMakeFiles/pea_2.dir/main.cpp.obj"

# External object files for target pea_2
pea_2_EXTERNAL_OBJECTS =

pea_2.exe: CMakeFiles/pea_2.dir/main.cpp.obj
pea_2.exe: CMakeFiles/pea_2.dir/build.make
pea_2.exe: CMakeFiles/pea_2.dir/linkLibs.rsp
pea_2.exe: CMakeFiles/pea_2.dir/objects1
pea_2.exe: CMakeFiles/pea_2.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=C:\Users\busin\CLionProjects\pwr_pea\pea_2\cmake-build-release\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable pea_2.exe"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\pea_2.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/pea_2.dir/build: pea_2.exe
.PHONY : CMakeFiles/pea_2.dir/build

CMakeFiles/pea_2.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles\pea_2.dir\cmake_clean.cmake
.PHONY : CMakeFiles/pea_2.dir/clean

CMakeFiles/pea_2.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" C:\Users\busin\CLionProjects\pwr_pea\pea_2 C:\Users\busin\CLionProjects\pwr_pea\pea_2 C:\Users\busin\CLionProjects\pwr_pea\pea_2\cmake-build-release C:\Users\busin\CLionProjects\pwr_pea\pea_2\cmake-build-release C:\Users\busin\CLionProjects\pwr_pea\pea_2\cmake-build-release\CMakeFiles\pea_2.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/pea_2.dir/depend
