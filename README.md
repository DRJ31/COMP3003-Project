# COMP3003-Project: Phone Book

This is a course project of COMP3003: Data Communications and Networking in BNU-HKBU UIC, an online yellow page for a user to query information of someone from the phone book server through a dedicated client.

## Build

The project depends on the following libraries:

- parson (statically compiled)
- GLib
- GTK+ 3
- LibMySQL (LibMariaDB)

Pull this repository with `git clone --recurse-submodules` to ensure the parson library be correctly included in the project folder.

To build, either type `make` or `cmake . && make` to compile the whole project. The final binary `pb` is in the current directory. CMake is mainly for support from the CLion IDE, and thus is not intended to be used in the actual case.

You may adjust compile options when using `make`:

- `make RELEASE=1` will use a different set of flag to optimize the final binary
- `make CC=clang` to switch to the Clang/LLVM compiler (you may need to do this in macOS)

If you find `pkg-config` is reporting errors on finding packages, please find `pkg-config` commands in the `Makefile`s and adjust the package names manually.

## Features

- Socket programming achieved with UNIX (POSIX) socket APIs
- HTTP support; all requests and responses are in HTTP protocol
- JSON is used to represent information in messages
- User-friendly graphical user interface made with GTK+

This is a course project, and is far from finished. Do not use it for serious business unless you know what you are doing.

## License

This project is licensed under the MIT license. See [COPYING](COPYING) for details.

The included parson library is licensed under the MIT license. Dependencies LibMariaDB, GLib and GTK+ are licensed under LGPL 2.1.
