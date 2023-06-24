cmake_minimum_required(VERSION 3.15)

project(MumblePlugin
	VERSION "1.0.0"
	DESCRIPTION "Terraria Proximity Chat Plugin"
	LANGUAGES "C"
)

add_library(TerrariaPlugin
	SHARED
		tmodloader.c
		_tmodloader.c
)

add_executable(test
	_tmodloader.c
	tmodloader.c
)

target_include_directories(TerrariaPlugin
	PUBLIC "${CMAKE_SOURCE_DIR}/include/"
)