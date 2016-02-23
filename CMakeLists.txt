cmake_minimum_required(VERSION 2.8)

# Project name without spaces or special characters
project("brewman")

set(APP_TITLE "BrewMan")
set(APP_DESCRIPTION "Homebrew manager and installer")
set(APP_AUTHOR "Cruel")
set(APP_ICON ${PROJECT_SOURCE_DIR}/res/app/icon.png)

# CIA settings
set(APP_UNIQUE_ID 0xc00c13) # This must be unique for your CIA project
set(APP_PRODUCT_CODE ${PROJECT_NAME})
set(BANNER_IMAGE ${PROJECT_SOURCE_DIR}/res/app/banner.png)
set(BANNER_SOUND ${PROJECT_SOURCE_DIR}/res/app/banner.wav)
set(CIA_CONFIG ${PROJECT_SOURCE_DIR}/res/app/cia.rsf)

# Include cpp3ds cmake module
set(CPP3DS $ENV{CPP3DS})
if(NOT CPP3DS)
	message(FATAL_ERROR "You need to set the env variable CPP3DS before compiling.")
endif()
set(CMAKE_MODULE_PATH "${CPP3DS}/cmake" ${CMAKE_MODULE_PATH})
include(cpp3ds)

include_directories(
	${PROJECT_SOURCE_DIR}/external/tween-engine/include
)
FILE(GLOB_RECURSE TWEEN_ENGINE_SOURCE ${PROJECT_SOURCE_DIR}/external/tween-engine/src/*.cpp)

set(SOURCE_FILES
	${PROJECT_SOURCE_DIR}/src/main.cpp
	${PROJECT_SOURCE_DIR}/src/AppItem.cpp
	${PROJECT_SOURCE_DIR}/src/AppList.cpp
	${PROJECT_SOURCE_DIR}/src/BrewMan.cpp
	${PROJECT_SOURCE_DIR}/src/Installer.cpp
	${PROJECT_SOURCE_DIR}/src/Download.cpp
	${PROJECT_SOURCE_DIR}/src/DownloadQueue.cpp
	${PROJECT_SOURCE_DIR}/src/Notification.cpp
	${PROJECT_SOURCE_DIR}/src/GUI/Button.cpp
	${PROJECT_SOURCE_DIR}/src/GUI/NinePatch.cpp
	${PROJECT_SOURCE_DIR}/src/States/State.cpp
	${PROJECT_SOURCE_DIR}/src/States/StateStack.cpp
	${PROJECT_SOURCE_DIR}/src/States/LoadingState.cpp
	${PROJECT_SOURCE_DIR}/src/States/TitleState.cpp
	${PROJECT_SOURCE_DIR}/src/States/BrowseState.cpp
	${PROJECT_SOURCE_DIR}/src/States/SyncState.cpp
	${TWEEN_ENGINE_SOURCE}
	${PROJECT_SOURCE_DIR}/src/boot.c
)

# Use either nihstro or picasso for your shader scripts
set(SHADER_AS nihstro)
FILE(GLOB_RECURSE SHADER_FILES ${PROJECT_SOURCE_DIR}/res/romfs/shaders/[^.]*.vsh)

##################################################
# Don't usually need to change anything below
##################################################
set(CPP3DS_ARM_FLAGS "-O2 -march=armv6k -mtune=mpcore -mfloat-abi=hard -mtp=soft")
set(CPP3DS_EMU_FLAGS "-O0")

set(CPP3DS_ARM_LIBS cpp3ds-window cpp3ds-network cpp3ds-graphics cpp3ds-audio cpp3ds-system citro3d git2 test mbedtls mbedx509 mbedcrypto ctru freetype png z vorbisenc vorbisfile vorbis ogg yaml-cpp archive bz2 lzma)
set(CPP3DS_EMU_LIBS cpp3ds-emu sfml-graphics sfml-window sfml-system sfml-audio openal GLEW GL jpeg freetype vorbisenc vorbisfile vorbis ogg git2 mbedtls mbedx509 mbedcrypto yaml-cpp archive)

add_subdirectory(${CPP3DS}/cmake/template_arm build/arm)
add_subdirectory(${CPP3DS}/cmake/template_emu build/emu)
