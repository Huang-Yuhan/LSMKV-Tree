# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

# Default target executed when no arguments are given to make.
default_target: all
.PHONY : default_target

# Allow only one "make -f Makefile2" at a time, but pass parallelism.
.NOTPARALLEL:

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

#=============================================================================
# Targets provided globally by CMake.

# Special rule for the target edit_cache
edit_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Running CMake cache editor..."
	/usr/bin/cmake-gui -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : edit_cache

# Special rule for the target edit_cache
edit_cache/fast: edit_cache
.PHONY : edit_cache/fast

# Special rule for the target rebuild_cache
rebuild_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Running CMake to regenerate build system..."
	/usr/bin/cmake --regenerate-during-build -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : rebuild_cache

# Special rule for the target rebuild_cache
rebuild_cache/fast: rebuild_cache
.PHONY : rebuild_cache/fast

# The main all target
all: cmake_check_build_system
	$(CMAKE_COMMAND) -E cmake_progress_start /home/icecreamsakaz/桌面/advanced_datastructure/LSMKV-Tree/CMakeFiles /home/icecreamsakaz/桌面/advanced_datastructure/LSMKV-Tree//CMakeFiles/progress.marks
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 all
	$(CMAKE_COMMAND) -E cmake_progress_start /home/icecreamsakaz/桌面/advanced_datastructure/LSMKV-Tree/CMakeFiles 0
.PHONY : all

# The main clean target
clean:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 clean
.PHONY : clean

# The main clean target
clean/fast: clean
.PHONY : clean/fast

# Prepare targets for installation.
preinstall: all
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall

# Prepare targets for installation.
preinstall/fast:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall/fast

# clear depends
depend:
	$(CMAKE_COMMAND) -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 1
.PHONY : depend

#=============================================================================
# Target rules for targets named correctness

# Build rule for target.
correctness: cmake_check_build_system
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 correctness
.PHONY : correctness

# fast build rule for target.
correctness/fast:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/correctness.dir/build.make CMakeFiles/correctness.dir/build
.PHONY : correctness/fast

#=============================================================================
# Target rules for targets named persistence

# Build rule for target.
persistence: cmake_check_build_system
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 persistence
.PHONY : persistence

# fast build rule for target.
persistence/fast:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/persistence.dir/build.make CMakeFiles/persistence.dir/build
.PHONY : persistence/fast

correctness.o: correctness.cc.o
.PHONY : correctness.o

# target to build an object file
correctness.cc.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/correctness.dir/build.make CMakeFiles/correctness.dir/correctness.cc.o
.PHONY : correctness.cc.o

correctness.i: correctness.cc.i
.PHONY : correctness.i

# target to preprocess a source file
correctness.cc.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/correctness.dir/build.make CMakeFiles/correctness.dir/correctness.cc.i
.PHONY : correctness.cc.i

correctness.s: correctness.cc.s
.PHONY : correctness.s

# target to generate assembly for a file
correctness.cc.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/correctness.dir/build.make CMakeFiles/correctness.dir/correctness.cc.s
.PHONY : correctness.cc.s

kvstore.o: kvstore.cc.o
.PHONY : kvstore.o

# target to build an object file
kvstore.cc.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/correctness.dir/build.make CMakeFiles/correctness.dir/kvstore.cc.o
	$(MAKE) $(MAKESILENT) -f CMakeFiles/persistence.dir/build.make CMakeFiles/persistence.dir/kvstore.cc.o
.PHONY : kvstore.cc.o

kvstore.i: kvstore.cc.i
.PHONY : kvstore.i

# target to preprocess a source file
kvstore.cc.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/correctness.dir/build.make CMakeFiles/correctness.dir/kvstore.cc.i
	$(MAKE) $(MAKESILENT) -f CMakeFiles/persistence.dir/build.make CMakeFiles/persistence.dir/kvstore.cc.i
.PHONY : kvstore.cc.i

kvstore.s: kvstore.cc.s
.PHONY : kvstore.s

# target to generate assembly for a file
kvstore.cc.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/correctness.dir/build.make CMakeFiles/correctness.dir/kvstore.cc.s
	$(MAKE) $(MAKESILENT) -f CMakeFiles/persistence.dir/build.make CMakeFiles/persistence.dir/kvstore.cc.s
.PHONY : kvstore.cc.s

persistence.o: persistence.cc.o
.PHONY : persistence.o

# target to build an object file
persistence.cc.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/persistence.dir/build.make CMakeFiles/persistence.dir/persistence.cc.o
.PHONY : persistence.cc.o

persistence.i: persistence.cc.i
.PHONY : persistence.i

# target to preprocess a source file
persistence.cc.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/persistence.dir/build.make CMakeFiles/persistence.dir/persistence.cc.i
.PHONY : persistence.cc.i

persistence.s: persistence.cc.s
.PHONY : persistence.s

# target to generate assembly for a file
persistence.cc.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/persistence.dir/build.make CMakeFiles/persistence.dir/persistence.cc.s
.PHONY : persistence.cc.s

skiplist.o: skiplist.cpp.o
.PHONY : skiplist.o

# target to build an object file
skiplist.cpp.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/correctness.dir/build.make CMakeFiles/correctness.dir/skiplist.cpp.o
	$(MAKE) $(MAKESILENT) -f CMakeFiles/persistence.dir/build.make CMakeFiles/persistence.dir/skiplist.cpp.o
.PHONY : skiplist.cpp.o

skiplist.i: skiplist.cpp.i
.PHONY : skiplist.i

# target to preprocess a source file
skiplist.cpp.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/correctness.dir/build.make CMakeFiles/correctness.dir/skiplist.cpp.i
	$(MAKE) $(MAKESILENT) -f CMakeFiles/persistence.dir/build.make CMakeFiles/persistence.dir/skiplist.cpp.i
.PHONY : skiplist.cpp.i

skiplist.s: skiplist.cpp.s
.PHONY : skiplist.s

# target to generate assembly for a file
skiplist.cpp.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/correctness.dir/build.make CMakeFiles/correctness.dir/skiplist.cpp.s
	$(MAKE) $(MAKESILENT) -f CMakeFiles/persistence.dir/build.make CMakeFiles/persistence.dir/skiplist.cpp.s
.PHONY : skiplist.cpp.s

# Help Target
help:
	@echo "The following are some of the valid targets for this Makefile:"
	@echo "... all (the default if no target is provided)"
	@echo "... clean"
	@echo "... depend"
	@echo "... edit_cache"
	@echo "... rebuild_cache"
	@echo "... correctness"
	@echo "... persistence"
	@echo "... correctness.o"
	@echo "... correctness.i"
	@echo "... correctness.s"
	@echo "... kvstore.o"
	@echo "... kvstore.i"
	@echo "... kvstore.s"
	@echo "... persistence.o"
	@echo "... persistence.i"
	@echo "... persistence.s"
	@echo "... skiplist.o"
	@echo "... skiplist.i"
	@echo "... skiplist.s"
.PHONY : help



#=============================================================================
# Special targets to cleanup operation of make.

# Special rule to run CMake to check the build system integrity.
# No rule that depends on this can have commands that come from listfiles
# because they might be regenerated.
cmake_check_build_system:
	$(CMAKE_COMMAND) -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 0
.PHONY : cmake_check_build_system

