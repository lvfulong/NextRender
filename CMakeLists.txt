set(PROJECT_NAME "NextRender")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(NEXT_RENDER_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR})
cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)
cmake_policy(VERSION 3.1.0)

project (${PROJECT_NAME})

# The version number.
set (${PROJECT_NAME}_VERSION_MAJOR 1)
set (${PROJECT_NAME}_VERSION_MINOR 0)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_MODULE_PATH
  "${CMAKE_SOURCE_DIR}/CMake"
  ${CMAKE_MODULE_PATH}
)

# Precompiled header macro. Parameters are source file list and filename for pch cpp file.
macro(vulkan_samples_pch SRCS PCHCPP)
  if(MSVC AND CMAKE_GENERATOR MATCHES "^Visual Studio")
    set(PCH_NAME "$(IntDir)\\pch.pch")
    # make source files use/depend on PCH_NAME
    set_source_files_properties(${${SRCS}} PROPERTIES COMPILE_FLAGS "/Yupch.h /FIpch.h /Fp${PCH_NAME} /Zm300" OBJECT_DEPENDS "${PCH_NAME}")
    # make PCHCPP file compile and generate PCH_NAME
    set_source_files_properties(${PCHCPP} PROPERTIES COMPILE_FLAGS "/Ycpch.h /Fp${PCH_NAME} /Zm300" OBJECT_OUTPUTS "${PCH_NAME}")
    list(APPEND ${SRCS} "${PCHCPP}")
  endif()
endmacro(vulkan_samples_pch)

add_subdirectory (Runtime)
add_subdirectory (Samples)
add_subdirectory (ThirdParty)