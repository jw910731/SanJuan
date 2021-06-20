# SanJuan [![CodeFactor](https://www.codefactor.io/repository/github/jw910731/sanjuan/badge/main)](https://www.codefactor.io/repository/github/jw910731/sanjuan/overview/main)
My Computer Programming class final project. The tabletop game San Juan.

# Build & Install
This project do not use GNU make as build system. Instead, cmake is being used.
Please installed the dependencies and follow the instruction below to build.
## Requirement
- Cmake
- Readline (Installed by most GNU/Linux distributions)
- Doxygen (for generating documentation)

## Build
Run following command:
```shell
mkdir build && cd build
cmake ..
make
```
The location of executable is under `src/SanJuan`

To generate document, use build target `doxygen`.
The result html document will be located at `html` directory.

# TODO Feature
- Support single player & multiplayer
- Command line interface with auto complete and history
- Smart AI from San Juan java edition \( [Author Link](http://www.compoundeye.net/jsanjuan/download.html) \)
