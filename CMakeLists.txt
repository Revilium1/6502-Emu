cmake_minimum_required(VERSION 3.14)

project( 6502 )

# Turn on the ability to create folders to organize projects (.vcproj)
# It creates "CMakePredefinedTargets" folder by default and adds CMake
# defined projects like INSTALL.vcproj and ZERO_CHECK.vcproj
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Build the library before the test executable that links against it.
add_subdirectory(6502/6502Lib)
add_subdirectory(6502/6502Test)