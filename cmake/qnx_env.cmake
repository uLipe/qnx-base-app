# cmake/qnx_env.cmake

if(NOT DEFINED ENV{QNX_HOST})
  message(FATAL_ERROR "QNX_HOST not found. Run: source ~/qnx800/qnxsdp-env.sh")
endif()

if(NOT DEFINED ENV{QNX_TARGET})
  message(FATAL_ERROR "QNX_TARGET not found. Run: source ~/qnx800/qnxsdp-env.sh")
endif()

set(QNX_HOST   $ENV{QNX_HOST})
set(QNX_TARGET $ENV{QNX_TARGET})