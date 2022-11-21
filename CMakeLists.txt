cmake_minimum_required(VERSION 3.2)

if(APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.13" CACHE STRING "Minimum OS X deployment version")
endif()

project(R2grap-gl)

set(CMAKE_CXX_STANDARD 17)

set(BASE
    src/base/BezierGen.hpp
    src/base/JsonReader.hpp
    src/base/Polygon.hpp
    src/base/KeyframeGen.hpp
)

set(SHADER
    src/shader/bezier_insert.geom
    src/shader/r2grap_m.frag
    src/shader/r2grap_m.vert
)

set(CODEC
    src/codec/AniInfoManager.h
    src/codec/FillInfo.h
    src/codec/PathInfo.h
	  src/codec/StrokeInfo.h
    src/codec/LayersInfo.h
    src/codec/LayersInfo.cpp
    src/codec/Transform.h
    src/codec/Transform.cpp
    src/codec/LinkLayer.h
    src/codec/LinkLayer.cpp
)

set(RENDER
    src/render/BaseRenderData.h
    src/render/ColorRenderData.h
    src/render/ColorRenderData.cpp
    src/render/RenderContent.h
    src/render/RenderContent.cpp
    src/render/RenderDataFactory.h
    src/render/RenderDataFactory.cpp
    src/render/TransRenderData.h
    src/render/TransRenderData.cpp
    src/render/VertRenderData.h
    src/render/VertRenderData.cpp
)

set(MAIN src/r2grap-gl.cpp)

source_group(shader FILES ${SHADER})
source_group(base FILES ${BASE})
source_group(codec FILES ${CODEC})
source_group(render FILES ${RENDER})

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
)

include_directories(
    ${CMAKE_CURRENT_LIST_DIR}/include
)

set(LIBS_DIRECTORIES
	lib
)

add_library(GLAD include/glad/glad.c)

if(WIN32)
	link_directories(${LIBS_DIRECTORIES})
else()	
	link_directories(/opt/homebrew/Cellar//glew/2.2.0_1/lib)
	link_directories(/opt/homebrew/Cellar/glfw/3.3.7/lib)
endif()

add_executable(${PROJECT_NAME} ${SHADER} ${RENDER} ${CODEC} ${BASE} ${MAIN})

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
else()
	target_link_libraries(${PROJECT_NAME} GLFW GLAD)
endif()

if(MSVC)    
  add_definitions(/W2 /GR)
else()
  add_definitions(-fexceptions -frtti)
endif()