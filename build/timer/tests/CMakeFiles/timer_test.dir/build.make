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
include timer/tests/CMakeFiles/timer_test.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include timer/tests/CMakeFiles/timer_test.dir/compiler_depend.make

# Include the progress variables for this target.
include timer/tests/CMakeFiles/timer_test.dir/progress.make

# Include the compile flags for this target's objects.
include timer/tests/CMakeFiles/timer_test.dir/flags.make

timer/tests/CMakeFiles/timer_test.dir/test_timer.cpp.o: timer/tests/CMakeFiles/timer_test.dir/flags.make
timer/tests/CMakeFiles/timer_test.dir/test_timer.cpp.o: /home/zy/pro2/Reactor/timer/tests/test_timer.cpp
timer/tests/CMakeFiles/timer_test.dir/test_timer.cpp.o: timer/tests/CMakeFiles/timer_test.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/zy/pro2/Reactor/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object timer/tests/CMakeFiles/timer_test.dir/test_timer.cpp.o"
	cd /home/zy/pro2/Reactor/build/timer/tests && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT timer/tests/CMakeFiles/timer_test.dir/test_timer.cpp.o -MF CMakeFiles/timer_test.dir/test_timer.cpp.o.d -o CMakeFiles/timer_test.dir/test_timer.cpp.o -c /home/zy/pro2/Reactor/timer/tests/test_timer.cpp

timer/tests/CMakeFiles/timer_test.dir/test_timer.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/timer_test.dir/test_timer.cpp.i"
	cd /home/zy/pro2/Reactor/build/timer/tests && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/zy/pro2/Reactor/timer/tests/test_timer.cpp > CMakeFiles/timer_test.dir/test_timer.cpp.i

timer/tests/CMakeFiles/timer_test.dir/test_timer.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/timer_test.dir/test_timer.cpp.s"
	cd /home/zy/pro2/Reactor/build/timer/tests && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/zy/pro2/Reactor/timer/tests/test_timer.cpp -o CMakeFiles/timer_test.dir/test_timer.cpp.s

timer/tests/CMakeFiles/timer_test.dir/__/__/log/pr.cpp.o: timer/tests/CMakeFiles/timer_test.dir/flags.make
timer/tests/CMakeFiles/timer_test.dir/__/__/log/pr.cpp.o: /home/zy/pro2/Reactor/log/pr.cpp
timer/tests/CMakeFiles/timer_test.dir/__/__/log/pr.cpp.o: timer/tests/CMakeFiles/timer_test.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/zy/pro2/Reactor/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object timer/tests/CMakeFiles/timer_test.dir/__/__/log/pr.cpp.o"
	cd /home/zy/pro2/Reactor/build/timer/tests && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT timer/tests/CMakeFiles/timer_test.dir/__/__/log/pr.cpp.o -MF CMakeFiles/timer_test.dir/__/__/log/pr.cpp.o.d -o CMakeFiles/timer_test.dir/__/__/log/pr.cpp.o -c /home/zy/pro2/Reactor/log/pr.cpp

timer/tests/CMakeFiles/timer_test.dir/__/__/log/pr.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/timer_test.dir/__/__/log/pr.cpp.i"
	cd /home/zy/pro2/Reactor/build/timer/tests && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/zy/pro2/Reactor/log/pr.cpp > CMakeFiles/timer_test.dir/__/__/log/pr.cpp.i

timer/tests/CMakeFiles/timer_test.dir/__/__/log/pr.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/timer_test.dir/__/__/log/pr.cpp.s"
	cd /home/zy/pro2/Reactor/build/timer/tests && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/zy/pro2/Reactor/log/pr.cpp -o CMakeFiles/timer_test.dir/__/__/log/pr.cpp.s

timer/tests/CMakeFiles/timer_test.dir/__/__/log/log.cpp.o: timer/tests/CMakeFiles/timer_test.dir/flags.make
timer/tests/CMakeFiles/timer_test.dir/__/__/log/log.cpp.o: /home/zy/pro2/Reactor/log/log.cpp
timer/tests/CMakeFiles/timer_test.dir/__/__/log/log.cpp.o: timer/tests/CMakeFiles/timer_test.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/zy/pro2/Reactor/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object timer/tests/CMakeFiles/timer_test.dir/__/__/log/log.cpp.o"
	cd /home/zy/pro2/Reactor/build/timer/tests && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT timer/tests/CMakeFiles/timer_test.dir/__/__/log/log.cpp.o -MF CMakeFiles/timer_test.dir/__/__/log/log.cpp.o.d -o CMakeFiles/timer_test.dir/__/__/log/log.cpp.o -c /home/zy/pro2/Reactor/log/log.cpp

timer/tests/CMakeFiles/timer_test.dir/__/__/log/log.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/timer_test.dir/__/__/log/log.cpp.i"
	cd /home/zy/pro2/Reactor/build/timer/tests && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/zy/pro2/Reactor/log/log.cpp > CMakeFiles/timer_test.dir/__/__/log/log.cpp.i

timer/tests/CMakeFiles/timer_test.dir/__/__/log/log.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/timer_test.dir/__/__/log/log.cpp.s"
	cd /home/zy/pro2/Reactor/build/timer/tests && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/zy/pro2/Reactor/log/log.cpp -o CMakeFiles/timer_test.dir/__/__/log/log.cpp.s

# Object files for target timer_test
timer_test_OBJECTS = \
"CMakeFiles/timer_test.dir/test_timer.cpp.o" \
"CMakeFiles/timer_test.dir/__/__/log/pr.cpp.o" \
"CMakeFiles/timer_test.dir/__/__/log/log.cpp.o"

# External object files for target timer_test
timer_test_EXTERNAL_OBJECTS =

timer/tests/timer_test: timer/tests/CMakeFiles/timer_test.dir/test_timer.cpp.o
timer/tests/timer_test: timer/tests/CMakeFiles/timer_test.dir/__/__/log/pr.cpp.o
timer/tests/timer_test: timer/tests/CMakeFiles/timer_test.dir/__/__/log/log.cpp.o
timer/tests/timer_test: timer/tests/CMakeFiles/timer_test.dir/build.make
timer/tests/timer_test: timer/tests/CMakeFiles/timer_test.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/home/zy/pro2/Reactor/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Linking CXX executable timer_test"
	cd /home/zy/pro2/Reactor/build/timer/tests && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/timer_test.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
timer/tests/CMakeFiles/timer_test.dir/build: timer/tests/timer_test
.PHONY : timer/tests/CMakeFiles/timer_test.dir/build

timer/tests/CMakeFiles/timer_test.dir/clean:
	cd /home/zy/pro2/Reactor/build/timer/tests && $(CMAKE_COMMAND) -P CMakeFiles/timer_test.dir/cmake_clean.cmake
.PHONY : timer/tests/CMakeFiles/timer_test.dir/clean

timer/tests/CMakeFiles/timer_test.dir/depend:
	cd /home/zy/pro2/Reactor/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/zy/pro2/Reactor /home/zy/pro2/Reactor/timer/tests /home/zy/pro2/Reactor/build /home/zy/pro2/Reactor/build/timer/tests /home/zy/pro2/Reactor/build/timer/tests/CMakeFiles/timer_test.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : timer/tests/CMakeFiles/timer_test.dir/depend
