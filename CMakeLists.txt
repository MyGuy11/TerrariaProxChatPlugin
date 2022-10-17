cmake_minimum_required(VERSION 3.15)

project(MumblePlugin
	VERSION "1.0.0"
	DESCRIPTION "Terraria Proximity Chat Plugin"
	LANGUAGES "C"
)

add_library(plugin
	SHARED
		tmodloader.c
		_tmodloader.c
)

target_include_directories(plugin
	PUBLIC "${CMAKE_SOURCE_DIR}/include/"
)