# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 4.0

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
CMAKE_COMMAND = /snap/cmake/1463/bin/cmake

# The command to remove a file.
RM = /snap/cmake/1463/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = "/mnt/c/Users/a/source/repos/C Programming Language"

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = "/mnt/c/Users/a/source/repos/C Programming Language/build"

# Include any dependencies generated for this target.
include CMakeFiles/packer.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/packer.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/packer.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/packer.dir/flags.make

CMakeFiles/packer.dir/codegen:
.PHONY : CMakeFiles/packer.dir/codegen

CMakeFiles/packer.dir/TP3/TP3/TP3.c.o: CMakeFiles/packer.dir/flags.make
CMakeFiles/packer.dir/TP3/TP3/TP3.c.o: /mnt/c/Users/a/source/repos/C\ Programming\ Language/TP3/TP3/TP3.c
CMakeFiles/packer.dir/TP3/TP3/TP3.c.o: CMakeFiles/packer.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir="/mnt/c/Users/a/source/repos/C Programming Language/build/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/packer.dir/TP3/TP3/TP3.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/packer.dir/TP3/TP3/TP3.c.o -MF CMakeFiles/packer.dir/TP3/TP3/TP3.c.o.d -o CMakeFiles/packer.dir/TP3/TP3/TP3.c.o -c "/mnt/c/Users/a/source/repos/C Programming Language/TP3/TP3/TP3.c"

CMakeFiles/packer.dir/TP3/TP3/TP3.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/packer.dir/TP3/TP3/TP3.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E "/mnt/c/Users/a/source/repos/C Programming Language/TP3/TP3/TP3.c" > CMakeFiles/packer.dir/TP3/TP3/TP3.c.i

CMakeFiles/packer.dir/TP3/TP3/TP3.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/packer.dir/TP3/TP3/TP3.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S "/mnt/c/Users/a/source/repos/C Programming Language/TP3/TP3/TP3.c" -o CMakeFiles/packer.dir/TP3/TP3/TP3.c.s

# Object files for target packer
packer_OBJECTS = \
"CMakeFiles/packer.dir/TP3/TP3/TP3.c.o"

# External object files for target packer
packer_EXTERNAL_OBJECTS =

bin/packer: CMakeFiles/packer.dir/TP3/TP3/TP3.c.o
bin/packer: CMakeFiles/packer.dir/build.make
bin/packer: CMakeFiles/packer.dir/compiler_depend.ts
bin/packer: /mnt/c/Users/a/source/repos/C\ Programming\ Language/TP3/TP3/packing_Wang.o
bin/packer: /mnt/c/Users/a/source/repos/C\ Programming\ Language/TP1/TP1/logical_Wang.o
bin/packer: CMakeFiles/packer.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir="/mnt/c/Users/a/source/repos/C Programming Language/build/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable bin/packer"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/packer.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/packer.dir/build: bin/packer
.PHONY : CMakeFiles/packer.dir/build

CMakeFiles/packer.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/packer.dir/cmake_clean.cmake
.PHONY : CMakeFiles/packer.dir/clean

CMakeFiles/packer.dir/depend:
	cd "/mnt/c/Users/a/source/repos/C Programming Language/build" && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" "/mnt/c/Users/a/source/repos/C Programming Language" "/mnt/c/Users/a/source/repos/C Programming Language" "/mnt/c/Users/a/source/repos/C Programming Language/build" "/mnt/c/Users/a/source/repos/C Programming Language/build" "/mnt/c/Users/a/source/repos/C Programming Language/build/CMakeFiles/packer.dir/DependInfo.cmake" "--color=$(COLOR)"
.PHONY : CMakeFiles/packer.dir/depend

