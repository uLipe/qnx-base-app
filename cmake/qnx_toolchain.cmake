cmake_minimum_required(VERSION 3.20)

include(${CMAKE_CURRENT_LIST_DIR}/qnx_env.cmake)

# ==============================
# Processor selection
# ==============================

if(NOT DEFINED QNX_PROCESSOR)
  set(QNX_PROCESSOR "aarch64le")  # default
endif()

if(NOT QNX_PROCESSOR MATCHES "^(aarch64le|x86_64)$")
  message(FATAL_ERROR "QNX_PROCESSOR must be aarch64le or x86_64")
endif()

set(CMAKE_SYSTEM_NAME QNX)
set(CMAKE_SYSTEM_PROCESSOR ${QNX_PROCESSOR})

# ==============================
# Compilers
# ==============================

set(CMAKE_C_COMPILER   qcc)
set(CMAKE_CXX_COMPILER q++)

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# Variant flags
if(QNX_PROCESSOR STREQUAL "aarch64le")
  set(QNX_VARIANT_C   "-Vgcc_ntoaarch64le")
  set(QNX_VARIANT_CXX "-Vgcc_ntoaarch64le_cxx")
  set(QNX_GDB "ntoaarch64-gdb")
elseif(QNX_PROCESSOR STREQUAL "x86_64")
  set(QNX_VARIANT_C   "-Vgcc_ntox86_64")
  set(QNX_VARIANT_CXX "-Vgcc_ntox86_64_cxx")
  set(QNX_GDB "ntox86_64-gdb")
endif()

set(CMAKE_C_FLAGS_INIT   "${QNX_VARIANT_C}")
set(CMAKE_CXX_FLAGS_INIT "${QNX_VARIANT_CXX}")

# Sysroot
set(CMAKE_SYSROOT "${QNX_TARGET}")
set(CMAKE_FIND_ROOT_PATH "${QNX_TARGET};${QNX_HOST}")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# Debug flags
set(CMAKE_C_FLAGS_DEBUG   "-g -O0")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")

# Export GDB name
set(QNX_GDB ${QNX_GDB} CACHE INTERNAL "")