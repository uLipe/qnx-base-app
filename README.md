# QNX CMake Application Template

Modern CMake-based template for building native QNX applications without Momentics.

This project provides:

- A clean CMake toolchain for QNX SDP 8.0
- Cross-compilation support for `aarch64le` (default) and `x86_64`
- Remote deployment via `scp`
- Remote debugging via `qconn`
- A minimal example application demonstrating QNX IPC

Licensed under the MIT License.

---

# What Is This?

This repository is a modern alternative to using Momentics for application development.

It provides a structured CMake-based workflow to:

- Build QNX applications from Linux
- Deploy to a remote QNX target
- Debug using `qconn`
- Maintain a clean, IDE-agnostic development flow

The example application included demonstrates a simple QNX IPC interaction to give developers a quick hands-on experience with QNX native messaging mechanisms.

---

# Features

- Cross-compilation for QNX
- Supports `aarch64le` and `x86_64`
- Remote deployment via SSH
- Remote debugging via `qconn`
- CMake Presets for clean configuration
- No dependency on Momentics for app builds

---

# Prerequisites

To build this project, you need:

- QNX SDP 8.0 installed
  (QNX provides a free developer version of SDP 8.0)

Download from:
https://www.qnx.com/

After installation, make sure to load the QNX environment:

```bash
source ~/qnx800/qnxsdp-env.sh
````

You also need:

* CMake >= 3.20
* Ninja (recommended)
* SSH access to your QNX target
* `qconn` running on the target for debugging

---

# Getting the Repository

```bash
git clone https://github.com/yourusername/qnx-cmake-app.git
cd qnx-cmake-app
```

---

# Build Instructions

## Default (aarch64le)

```bash
cmake --preset qnx-aarch64
cmake --build --preset qnx-aarch64
```

## x86_64

```bash
cmake --preset qnx-x86_64
cmake --build --preset qnx-x86_64
```

Build artifacts will be located in:

```
build/aarch64/
build/x86_64/
```

---

# Running and Deploying to Target

By default:

* Hostname: `qnxpi`
* User: `qnxuser`
* Deploy directory: `/tmp`

You can change these using CMake variables:

```bash
cmake -S . -B build/aarch64 \
  -DCMAKE_TOOLCHAIN_FILE=cmake/qnx_toolchain.cmake \
  -DQNX_PROCESSOR=aarch64le \
  -DQNX_TARGET_HOST=192.168.1.100 \
  -DQNX_TARGET_USER=qnxuser \
  -DQNX_DEPLOY_DIR=/tmp
```

## Deploy

```bash
cmake --build build/aarch64 --target deploy
```

This copies the binary to the target using `scp`.

---

# Debugging via qconn

On the QNX target:

```sh
/usr/sbin/qconn &
```

On the host:

```bash
cmake --build build/aarch64 --target debug
```

This will:

1. Deploy the binary
2. Launch the appropriate QNX GDB
3. Connect to `qconn`
4. Start the application under debugger control

---

# Project Structure

```
qnx-app/
├── CMakeLists.txt
├── CMakePresets.json
├── cmake/
│   ├── qnx_toolchain.cmake
│   └── qnx_env.cmake
└── src/
    └── main.c
```

---

# IPC Example

The example application demonstrates a basic QNX IPC interaction.

This gives developers a practical starting point for:

* Message passing
* Resource managers
* Native QNX communication mechanisms

You are encouraged to extend this example to experiment with more advanced IPC patterns.

---

# Contributing

Contributions are welcome.

If you would like to:

* Improve the build system
* Add new templates (drivers, services, shared libraries)
* Improve documentation
* Add new examples

Please open a pull request.

If you find this project useful:

Consider giving the repository a star.

For direct contact, suggestions, or collaboration inquiries:

[ryukokki.felipe@gmail.com](mailto:ryukokki.felipe@gmail.com)

---

# License

The License here is simple: Do what you want with this project, I don't assume any responsibillity for any damage caused and you agree to this when using this repository contents.
