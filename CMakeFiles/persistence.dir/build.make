# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

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

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/icecreamsakaz/桌面/advanced_datastructure/LSMKV-Tree

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/icecreamsakaz/桌面/advanced_datastructure/LSMKV-Tree

# Include any dependencies generated for this target.
include CMakeFiles/persistence.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/persistence.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/persistence.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/persistence.dir/flags.make

CMakeFiles/persistence.dir/persistence.cc.o: CMakeFiles/persistence.dir/flags.make
CMakeFiles/persistence.dir/persistence.cc.o: persistence.cc
CMakeFiles/persistence.dir/persistence.cc.o: CMakeFiles/persistence.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/icecreamsakaz/桌面/advanced_datastructure/LSMKV-Tree/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/persistence.dir/persistence.cc.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/persistence.dir/persistence.cc.o -MF CMakeFiles/persistence.dir/persistence.cc.o.d -o CMakeFiles/persistence.dir/persistence.cc.o -c /home/icecreamsakaz/桌面/advanced_datastructure/LSMKV-Tree/persistence.cc

CMakeFiles/persistence.dir/persistence.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/persistence.dir/persistence.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/icecreamsakaz/桌面/advanced_datastructure/LSMKV-Tree/persistence.cc > CMakeFiles/persistence.dir/persistence.cc.i

CMakeFiles/persistence.dir/persistence.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/persistence.dir/persistence.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/icecreamsakaz/桌面/advanced_datastructure/LSMKV-Tree/persistence.cc -o CMakeFiles/persistence.dir/persistence.cc.s

CMakeFiles/persistence.dir/skiplist.cpp.o: CMakeFiles/persistence.dir/flags.make
CMakeFiles/persistence.dir/skiplist.cpp.o: skiplist.cpp
CMakeFiles/persistence.dir/skiplist.cpp.o: CMakeFiles/persistence.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/icecreamsakaz/桌面/advanced_datastructure/LSMKV-Tree/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/persistence.dir/skiplist.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/persistence.dir/skiplist.cpp.o -MF CMakeFiles/persistence.dir/skiplist.cpp.o.d -o CMakeFiles/persistence.dir/skiplist.cpp.o -c /home/icecreamsakaz/桌面/advanced_datastructure/LSMKV-Tree/skiplist.cpp

CMakeFiles/persistence.dir/skiplist.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/persistence.dir/skiplist.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/icecreamsakaz/桌面/advanced_datastructure/LSMKV-Tree/skiplist.cpp > CMakeFiles/persistence.dir/skiplist.cpp.i

CMakeFiles/persistence.dir/skiplist.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/persistence.dir/skiplist.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/icecreamsakaz/桌面/advanced_datastructure/LSMKV-Tree/skiplist.cpp -o CMakeFiles/persistence.dir/skiplist.cpp.s

CMakeFiles/persistence.dir/kvstore.cc.o: CMakeFiles/persistence.dir/flags.make
CMakeFiles/persistence.dir/kvstore.cc.o: kvstore.cc
CMakeFiles/persistence.dir/kvstore.cc.o: CMakeFiles/persistence.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/icecreamsakaz/桌面/advanced_datastructure/LSMKV-Tree/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/persistence.dir/kvstore.cc.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/persistence.dir/kvstore.cc.o -MF CMakeFiles/persistence.dir/kvstore.cc.o.d -o CMakeFiles/persistence.dir/kvstore.cc.o -c /home/icecreamsakaz/桌面/advanced_datastructure/LSMKV-Tree/kvstore.cc

CMakeFiles/persistence.dir/kvstore.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/persistence.dir/kvstore.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/icecreamsakaz/桌面/advanced_datastructure/LSMKV-Tree/kvstore.cc > CMakeFiles/persistence.dir/kvstore.cc.i

CMakeFiles/persistence.dir/kvstore.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/persistence.dir/kvstore.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/icecreamsakaz/桌面/advanced_datastructure/LSMKV-Tree/kvstore.cc -o CMakeFiles/persistence.dir/kvstore.cc.s

# Object files for target persistence
persistence_OBJECTS = \
"CMakeFiles/persistence.dir/persistence.cc.o" \
"CMakeFiles/persistence.dir/skiplist.cpp.o" \
"CMakeFiles/persistence.dir/kvstore.cc.o"

# External object files for target persistence
persistence_EXTERNAL_OBJECTS =

persistence: CMakeFiles/persistence.dir/persistence.cc.o
persistence: CMakeFiles/persistence.dir/skiplist.cpp.o
persistence: CMakeFiles/persistence.dir/kvstore.cc.o
persistence: CMakeFiles/persistence.dir/build.make
persistence: CMakeFiles/persistence.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/icecreamsakaz/桌面/advanced_datastructure/LSMKV-Tree/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Linking CXX executable persistence"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/persistence.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/persistence.dir/build: persistence
.PHONY : CMakeFiles/persistence.dir/build

CMakeFiles/persistence.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/persistence.dir/cmake_clean.cmake
.PHONY : CMakeFiles/persistence.dir/clean

CMakeFiles/persistence.dir/depend:
	cd /home/icecreamsakaz/桌面/advanced_datastructure/LSMKV-Tree && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/icecreamsakaz/桌面/advanced_datastructure/LSMKV-Tree /home/icecreamsakaz/桌面/advanced_datastructure/LSMKV-Tree /home/icecreamsakaz/桌面/advanced_datastructure/LSMKV-Tree /home/icecreamsakaz/桌面/advanced_datastructure/LSMKV-Tree /home/icecreamsakaz/桌面/advanced_datastructure/LSMKV-Tree/CMakeFiles/persistence.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/persistence.dir/depend

