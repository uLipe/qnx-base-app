cmake_minimum_required(VERSION 3.20)
project(qnx_app C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(qnx_app
    src/main.c
)

target_compile_options(qnx_app PRIVATE -Wall -Wextra)

# =========================================
# Remote config (defaults)
# =========================================

if(NOT DEFINED QNX_TARGET_HOST)
    set(QNX_TARGET_HOST "qnxpi")
endif()

if(NOT DEFINED QNX_TARGET_USER)
    set(QNX_TARGET_USER "qnxuser")
endif()

if(NOT DEFINED QNX_DEPLOY_DIR)
    set(QNX_DEPLOY_DIR "/tmp")
endif()

if(NOT DEFINED QNX_QCONN_PORT)
    set(QNX_QCONN_PORT "8000")
endif()

set(REMOTE_PATH "${QNX_DEPLOY_DIR}/$<TARGET_FILE_NAME:qnx_app>")

# =========================================
# Deploy
# =========================================

add_custom_target(deploy
    COMMAND ${CMAKE_COMMAND} -E echo "Deploying to ${QNX_TARGET_USER}@${QNX_TARGET_HOST}"
    COMMAND scp -q $<TARGET_FILE:qnx_app> ${QNX_TARGET_USER}@${QNX_TARGET_HOST}:${REMOTE_PATH}
    DEPENDS qnx_app
    USES_TERMINAL
)

# =========================================
# Debug via qconn
# =========================================

add_custom_target(debug
    COMMAND ${CMAKE_COMMAND} -E echo "Ensure qconn is running on target"
    COMMAND ${QNX_GDB}
        -ex "file $<TARGET_FILE:qnx_app>"
        -ex "target qnx ${QNX_TARGET_HOST}:${QNX_QCONN_PORT}"
        -ex "set nto-stop-on-exec 1"
        -ex "set remote exec-file ${REMOTE_PATH}"
        -ex "b main"
        -ex "run"
    DEPENDS deploy
    USES_TERMINAL
)