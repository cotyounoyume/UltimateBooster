cmake_minimum_required(VERSION 3.21)

# 静的リンクの設定
set(VCPKG_TARGET_TRIPLET "x64-windows-static" CACHE STRING "")
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

# info
project(
	UltimateBooster
	VERSION 1.0.0
	LANGUAGES CXX
)

# vcpkgパスの設定（必要に応じて調整してください）
set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH};e:/Data/vcpkg/vcpkg/installed/x64-windows-static/share")

# boiler
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/common.cmake)

# in-place configuration
configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/cmake/Plugin.h.in
	${CMAKE_CURRENT_BINARY_DIR}/include/Plugin.h
	@ONLY
)

configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/cmake/version.rc.in
	${CMAKE_CURRENT_BINARY_DIR}/version.rc
	@ONLY
)

# source files
execute_process(COMMAND powershell -ExecutionPolicy Bypass -File "${CMAKE_CURRENT_SOURCE_DIR}/!update.ps1" "SOURCEGEN" "${PROJECT_VERSION}" "${CMAKE_CURRENT_BINARY_DIR}")
include(${CMAKE_CURRENT_BINARY_DIR}/sourcelist.cmake)
source_group(
	TREE ${CMAKE_CURRENT_SOURCE_DIR}
	FILES ${SOURCES}
)

source_group(
	TREE ${CMAKE_CURRENT_BINARY_DIR}
	FILES ${CMAKE_CURRENT_BINARY_DIR}/include/Plugin.h
)

# dependencies
set(USING_VCPKG TRUE)
find_package(spdlog CONFIG REQUIRED)
set(SPDLOG_USE_STATIC_LIBS ON)
set(FMT_USE_STATIC_LIBS ON)

find_package(nlohmann_json CONFIG REQUIRED)
find_path(TOMLPLUSPLUS_INCLUDE_DIRS "toml++/toml.h")
find_dependency_path(DKUtil include/DKUtil/Logger.hpp)

set(CommonLibSF_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../extern/CommonLibSF")
message(STATUS "CommonLibSF_DIR: ${CommonLibSF_DIR}")

if(EXISTS "${CommonLibSF_DIR}/CMakeLists.txt")
    message(STATUS "Adding CommonLibSF subdirectory")
    add_subdirectory(${CommonLibSF_DIR} ${CMAKE_CURRENT_BINARY_DIR}/CommonLibSF)
else()
    message(FATAL_ERROR "CMakeLists.txt not found in CommonLibSF directory: ${CommonLibSF_DIR}")
endif()

# CommonLibSFのターゲットが正しく定義されていることを確認
if(TARGET CommonLibSF::CommonLibSF)
    message(STATUS "CommonLibSF target found")
else()
    message(FATAL_ERROR "CommonLibSF target not found after adding subdirectory")
endif()

# cmake target
# runtime
add_library(
	${PROJECT_NAME}
	SHARED
		${SOURCES}
		${CMAKE_CURRENT_BINARY_DIR}/include/Plugin.h
		${CMAKE_CURRENT_BINARY_DIR}/version.rc
		.clang-format
		vcpkg.json)

# include dir
target_include_directories(
	${PROJECT_NAME}
	PRIVATE
		${CMAKE_CURRENT_BINARY_DIR}/include
		${CMAKE_CURRENT_SOURCE_DIR}/include
		${CMAKE_CURRENT_SOURCE_DIR}/src
		${TOMLPLUSPLUS_INCLUDE_DIRS}
)

# linkage
target_link_libraries(
	${PROJECT_NAME} 
	PRIVATE
		DKUtil::DKUtil
		CommonLibSF::CommonLibSF
		spdlog::spdlog
		nlohmann_json::nlohmann_json
		XInput.lib
)

# compiler def
if (MSVC)
	add_compile_definitions(_UNICODE)

	target_compile_options(
		${PROJECT_NAME}
		PRIVATE
			/MP
			/await
			/W0
			/WX
			/permissive-
			/utf-8
			/Zc:alignedNew
			/Zc:auto
			/Zc:__cplusplus
			/Zc:externC
			/Zc:externConstexpr
			/Zc:forScope
			/Zc:hiddenFriend
			/Zc:implicitNoexcept
			/Zc:lambda
			/Zc:noexceptTypes
			/Zc:preprocessor
			/Zc:referenceBinding
			/Zc:rvalueCast
			/Zc:sizedDealloc
			/Zc:strictStrings
			/Zc:ternary
			/Zc:threadSafeInit
			/Zc:trigraphs
			/Zc:wchar_t
			/wd4200 # nonstandard extension used : zero-sized array in struct/union
			/FI${CMAKE_CURRENT_SOURCE_DIR}/src/PCH.h
	)
endif()

# PCH
target_precompile_headers(
	${PROJECT_NAME}
	PRIVATE
		src/PCH.h
)

set_property(
	TARGET 
	${PROJECT_NAME}
	PROPERTY VS_USER_PROPS 
	"${CMAKE_CURRENT_SOURCE_DIR}/cmake/build_stl_modules.props"
)

# 静的リンクの設定
set_target_properties(${PROJECT_NAME} PROPERTIES
  MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
)

# update deployments
# add_custom_command(
#	TARGET 
#	${PROJECT_NAME} 
#	POST_BUILD
#	COMMAND powershell -NoProfile -ExecutionPolicy Bypass -File 
#		"${CMAKE_CURRENT_SOURCE_DIR}/!update.ps1" "DISTRIBUTE" "${PROJECT_VERSION}" "${CMAKE_CURRENT_BINARY_DIR}/$(ConfigurationName)" "${PROJECT_NAME}" 
#)
