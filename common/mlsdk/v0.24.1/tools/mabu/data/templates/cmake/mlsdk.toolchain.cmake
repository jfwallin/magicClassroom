set(LUMIN TRUE)

set(MLSDK "@@MLSDK@@" CACHE PATH "Path to Lumin SDK")

# canonicalize
string(REPLACE "\\" "/" LUMIN_SDK "${MLSDK}")

set(CMAKE_SYSTEM_NAME    Linux)
set(CMAKE_SYSTEM_VERSION 1)

SET(TOOL_OS_SUFFIX "")

set(CMAKE_SYSTEM_PROCESSOR "@@CMAKE_SYSTEM_PROCESSOR@@")
set(CMAKE_SYSROOT          "@@CMAKE_SYSROOT@@")

set(CMAKE_MAKE_PROGRAM     "@@CMAKE_MAKE_PROGRAM@@" CACHE PATH "make executable" FORCE)
set(CMAKE_C_COMPILER       "@@CMAKE_C_COMPILER@@${TOOL_OS_SUFFIX}"  CACHE PATH "C compiler" FORCE)
set(CMAKE_CXX_COMPILER     "@@CMAKE_CXX_COMPILER@@${TOOL_OS_SUFFIX}" CACHE PATH "C++ compiler" FORCE)
set(CMAKE_AR               "@@CMAKE_AR@@" CACHE PATH "C/C++ archiver" FORCE)
set(CMAKE_RANLIB           "@@CMAKE_RANLIB@@" CACHE PATH "C/C++ ranlib" FORCE)

# Configure the behaviour of the find commands
set(CMAKE_PROGRAM_PATH "@@TOOLS_BIN@@")
set(CMAKE_FIND_ROOT_PATH "${CMAKE_SYSROOT}")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

set(CMAKE_C_FLAGS "@@CMAKE_C_FLAGS@@" CACHE INTERNAL "" FORCE)
set(CMAKE_CXX_FLAGS "@@CMAKE_CXX_FLAGS@@" CACHE INTERNAL "" FORCE)
set(CMAKE_EXE_LINKER_FLAGS "@@CMAKE_EXE_LINKER_FLAGS@@" CACHE INTERNAL "" FORCE)
set(CMAKE_SHARED_LINKER_FLAGS "@@CMAKE_SHARED_LINKER_FLAGS@@" CACHE INTERNAL "" FORCE)
set(CMAKE_STATIC_LINKER_FLAGS "@@CMAKE_STATIC_LINKER_FLAGS@@" CACHE INTERNAL "" FORCE)
set(CMAKE_LIBRARY_PATH @@CMAKE_LIBRARY_PATH@@ CACHE INTERNAL "" FORCE)

include_directories(SYSTEM @@INCLUDE_DIRECTORIES@@)

#
# The following file was used by `mabu --create-cmake-toolchain` to determine what
# compiler and linker settings to use.
# You can pass `--path ...` or set the env var MABU_PATH=... to override where "base.comp" is discovered.
#
# MABU_TEMPLATE = @@MABU_TEMPLATE@@
# MABU_BASE = @@MABU_BASE@@
# MABU_SETTINGS = @@MABU_SETTINGS@@
#