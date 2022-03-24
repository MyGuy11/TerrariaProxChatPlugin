cmake_minimum_required(VERSION 3.0)

set(version "1.0.0")

project(TerrariaPlugin
	VERSION "${version}"
	DESCRIPTION "A Mumble proximity chat plugin for Terraria. Requires tModLoader 1.4 and the ProxChat mod."
	LANGUAGES "CXX"
)

# Cpp17 or later is required in order to use the wrapper
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# In order to use additional parts of the plugin interface, you have to enable the respective option
# e.g. set(server-event-callbacks ON CACHE BOOL "" FORCE)
set(all-features ON CACHE BOOL "" FORCE)

# We need the wrapper as a dependency
add_subdirectory("${CMAKE_CURRENT_BINARY_DIR}/wrapper")

add_library(TerrariaPlugin SHARED
	terraria.cpp
)

target_link_libraries(TerrariaPlugin PRIVATE mumble_plugin_cpp_wrapper)
