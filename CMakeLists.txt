cmake_minimum_required(VERSION 3.2)

if(APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.13" CACHE STRING "Minimum OS X deployment version")
endif()

project(R2grap-gl)

set(CMAKE_CXX_STANDARD 17)

file (GLOB BASE CONFIG_DEFENDS
	src/base/*.hpp
)

file(GLOB CODEC CONFIG_DEFENDS
	src/codec/*.h
	src/codec/*.cpp
)

file(GLOB RENDER CONFIG_DEFENDS
	src/render/*.h
	src/render/*.cpp
)
if(WIN32)
file(GLOB DIRECTX CONFIG_DEPENDS
  src/directx/*.h
  src/directx/*.cpp
)
endif()
file(GLOB OPENGL CONFIG_DEPENDS
  src/opengl/*h
  src/opengl/*cpp
)

set(SHADER
    src/shader/glsl/bezier_insert.geom
    src/shader/glsl/r2grap_m.frag
    src/shader/glsl/r2grap_m.vert
    src/shader/hlsl/r2grap_d3d.hlsli
    src/shader/hlsl/r2grap_d3d_PS.hlsl
    src/shader/hlsl/r2grap_d3d_VS.hlsl
)

set(MAIN
    src/main.cpp
)

source_group(shader FILES ${SHADER})
source_group(base FILES ${BASE})
source_group(codec FILES ${CODEC})
source_group(render FILES ${RENDER})
source_group(directx FILES ${DIRECTX})
source_group(opengl FILES ${OPENGL})

set(CMAKE_INCLUDE_CURRENT_DIR ON)

if(WIN32)
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi")
  set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /DEBUG /OPT:REF /OPT:ICF /SUBSYSTEM:WINDOWS /entry:mainCRTStartup")
endif()

if(UNIX)
  set(CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE)
  set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
  if(NOT APPLE)
    set(CMAKE_INSTALL_RPATH "$ORIGIN")
  else()
    set(CMAKE_MACOSX_RPATH ON)
    set(CMAKE_INSTALL_RPATH "@executable_path")
  endif()
endif()

if(CMAKE_SIZEOF_VOID_P MATCHES 8)
	set(PROJECT_ARCH "x64")
elseif(CMAKE_SIZEOF_VOID_P MATCHES 4)
	set(PROJECT_ARCH "Win32")
endif()

if(APPLE)
  set(CONFIG_TYPE $(CONFIGURATION))
else()
  set(CONFIG_TYPE ${CMAKE_CFG_INTDIR})
endif()
set(TARGET_TRIPLE_CONFIG ${CMAKE_SYSTEM_NAME}-${PROJECT_ARCH}-${CONFIG_TYPE})

set(INCLUDE_DIRECTORIES
  ${CMAKE_CURRENT_LIST_DIR}/include
  ${CMAKE_CURRENT_LIST_DIR}/src/codec
  ${CMAKE_CURRENT_LIST_DIR}/src/base
  ${CMAKE_CURRENT_LIST_DIR}/src/render
  ${CMAKE_CURRENT_LIST_DIR}/src/directx
  ${CMAKE_CURRENT_LIST_DIR}/src
)

include_directories(
    ${CMAKE_CURRENT_LIST_DIR}/include
)

set(LIBS_DIRECTORIES
	lib
)

if(WIN32)
	link_directories(${LIBS_DIRECTORIES})
else()
	link_directories(/opt/homebrew/Cellar//glew/2.2.0_1/lib)
	link_directories(/opt/homebrew/Cellar/glfw/3.3.7/lib)
endif()


add_library(GLAD include/glad/glad.c)
if(WIN32)
add_definitions(-D_DISABLE_EXTENDED_ALIGNED_STORAGE)
endif()
add_executable(${PROJECT_NAME} ${RENDER} ${CODEC} ${BASE} ${OPENGL} ${DIRECTX} ${MAIN})

target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIRECTORIES})
target_compile_definitions(${PROJECT_NAME} PRIVATE GLEW_STATIC)

if(WIN32)
  target_compile_definitions(${PROJECT_NAME} PUBLIC _CRT_SECURE_NO_DEPRECATE _CRT_SECURE_NO_WARNINGS)
else()
  target_compile_options(${PROJECT_NAME} PUBLIC -std=c++17)
  target_compile_definitions(${PROJECT_NAME} PUBLIC _FILE_OFFSET_BITS=64)
  target_compile_definitions(${PROJECT_NAME} PRIVATE USING_OPENGL_RENDER=1)
endif()

if(WIN32)
	target_link_libraries(${PROJECT_NAME} glfw3 GLAD)
	target_link_libraries(${PROJECT_NAME} d3d11.lib dxgi.lib dxguid.lib D3DCompiler.lib d2d1.lib dwrite.lib winmm.lib)
elseif(APPLE)
	target_link_libraries(${PROJECT_NAME} GLFW GLAD)
else()
target_link_libraries(${PROJECT_NAME} glfw GLAD)
endif()

if(MSVC)
  add_definitions(/W2 /GR)
else()
  add_definitions(-fexceptions -frtti)
endif()