# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.29

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
CMAKE_COMMAND = /snap/cmake/1378/bin/cmake

# The command to remove a file.
RM = /snap/cmake/1378/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/zy/pro2/Reactor

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/zy/pro2/Reactor/build

# Include any dependencies generated for this target.
include log/tests/CMakeFiles/q_test.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include log/tests/CMakeFiles/q_test.dir/compiler_depend.make

# Include the progress variables for this target.
include log/tests/CMakeFiles/q_test.dir/progress.make

# Include the compile flags for this target's objects.
include log/tests/CMakeFiles/q_test.dir/flags.make

log/tests/CMakeFiles/q_test.dir/test_q.cpp.o: log/tests/CMakeFiles/q_test.dir/flags.make
log/tests/CMakeFiles/q_test.dir/test_q.cpp.o: /home/zy/pro2/Reactor/log/tests/test_q.cpp
log/tests/CMakeFiles/q_test.dir/test_q.cpp.o: log/tests/CMakeFiles/q_test.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/zy/pro2/Reactor/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object log/tests/CMakeFiles/q_test.dir/test_q.cpp.o"
	cd /home/zy/pro2/Reactor/build/log/tests && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT log/tests/CMakeFiles/q_test.dir/test_q.cpp.o -MF CMakeFiles/q_test.dir/test_q.cpp.o.d -o CMakeFiles/q_test.dir/test_q.cpp.o -c /home/zy/pro2/Reactor/log/tests/test_q.cpp

log/tests/CMakeFiles/q_test.dir/test_q.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/q_test.dir/test_q.cpp.i"
	cd /home/zy/pro2/Reactor/build/log/tests && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/zy/pro2/Reactor/log/tests/test_q.cpp > CMakeFiles/q_test.dir/test_q.cpp.i

log/tests/CMakeFiles/q_test.dir/test_q.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/q_test.dir/test_q.cpp.s"
	cd /home/zy/pro2/Reactor/build/log/tests && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/zy/pro2/Reactor/log/tests/test_q.cpp -o CMakeFiles/q_test.dir/test_q.cpp.s

log/tests/CMakeFiles/q_test.dir/__/pr.cpp.o: log/tests/CMakeFiles/q_test.dir/flags.make
log/tests/CMakeFiles/q_test.dir/__/pr.cpp.o: /home/zy/pro2/Reactor/log/pr.cpp
log/tests/CMakeFiles/q_test.dir/__/pr.cpp.o: log/tests/CMakeFiles/q_test.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/zy/pro2/Reactor/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object log/tests/CMakeFiles/q_test.dir/__/pr.cpp.o"
	cd /home/zy/pro2/Reactor/build/log/tests && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT log/tests/CMakeFiles/q_test.dir/__/pr.cpp.o -MF CMakeFiles/q_test.dir/__/pr.cpp.o.d -o CMakeFiles/q_test.dir/__/pr.cpp.o -c /home/zy/pro2/Reactor/log/pr.cpp

log/tests/CMakeFiles/q_test.dir/__/pr.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/q_test.dir/__/pr.cpp.i"
	cd /home/zy/pro2/Reactor/build/log/tests && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/zy/pro2/Reactor/log/pr.cpp > CMakeFiles/q_test.dir/__/pr.cpp.i

log/tests/CMakeFiles/q_test.dir/__/pr.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/q_test.dir/__/pr.cpp.s"
	cd /home/zy/pro2/Reactor/build/log/tests && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/zy/pro2/Reactor/log/pr.cpp -o CMakeFiles/q_test.dir/__/pr.cpp.s

# Object files for target q_test
q_test_OBJECTS = \
"CMakeFiles/q_test.dir/test_q.cpp.o" \
"CMakeFiles/q_test.dir/__/pr.cpp.o"

# External object files for target q_test
q_test_EXTERNAL_OBJECTS =

log/tests/q_test: log/tests/CMakeFiles/q_test.dir/test_q.cpp.o
log/tests/q_test: log/tests/CMakeFiles/q_test.dir/__/pr.cpp.o
log/tests/q_test: log/tests/CMakeFiles/q_test.dir/build.make
log/tests/q_test: log/tests/CMakeFiles/q_test.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/home/zy/pro2/Reactor/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable q_test"
	cd /home/zy/pro2/Reactor/build/log/tests && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/q_test.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
log/tests/CMakeFiles/q_test.dir/build: log/tests/q_test
.PHONY : log/tests/CMakeFiles/q_test.dir/build

log/tests/CMakeFiles/q_test.dir/clean:
	cd /home/zy/pro2/Reactor/build/log/tests && $(CMAKE_COMMAND) -P CMakeFiles/q_test.dir/cmake_clean.cmake
.PHONY : log/tests/CMakeFiles/q_test.dir/clean

log/tests/CMakeFiles/q_test.dir/depend:
	cd /home/zy/pro2/Reactor/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/zy/pro2/Reactor /home/zy/pro2/Reactor/log/tests /home/zy/pro2/Reactor/build /home/zy/pro2/Reactor/build/log/tests /home/zy/pro2/Reactor/build/log/tests/CMakeFiles/q_test.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : log/tests/CMakeFiles/q_test.dir/depend
