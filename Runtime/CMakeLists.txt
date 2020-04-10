cmake_minimum_required(VERSION 3.8)

project(Runtime LANGUAGES C CXX)

set(RUNTIME_FILES
    Application.h
	Platform.h
	InputEvents.h
    Application.cpp
	Platform.cpp
	InputEvents.cpp)

set(COMMON_FILES

    )

set(GEOMETRY_FILES

    )

set(RENDERING_FILES

)

set(RENDERING_SUBPASSES_FILES

)

set(SCENE_GRAPH_FILES

)


set(CORE_FILES

)

set(PLATFORM_FILES
)



set(ANDROID_FILES
	)

set(WINDOWS_FILES
	Platform/Windows/WindowsPlatform.h
    Platform/Windows/WindowsPlatform.cpp)


set(GLFW_FILES

	) 

source_group("\\" FILES ${RUNTIME_FILES})
source_group("common\\" FILES ${COMMON_FILES})
source_group("platform\\" FILES ${PLATFORM_FILES})
source_group("platform\\" FILES ${GLFW_FILES})
source_group("platform\\windows" FILES ${WINDOWS_FILES})
source_group("platform\\android" FILES ${ANDROID_FILES})
source_group("platform\\unix" FILES ${UNIX_FILES})
source_group("platform\\unix" FILES ${LINUX_D2D_FILES})
source_group("core\\" FILES ${CORE_FILES})
source_group("geometry\\" FILES ${GEOMETRY_FILES})
source_group("rendering\\" FILES ${RENDERING_FILES})
source_group("rendering\\subpasses" FILES ${RENDERING_SUBPASSES_FILES})
source_group("scene_graph\\" FILES ${SCENE_GRAPH_FILES})
source_group("scene_graph\\components\\" FILES ${SCENE_GRAPH_COMPONENT_FILES})
source_group("scene_graph\\scripts\\" FILES ${SCENE_GRAPH_SCRIPTS_FILES})
source_group("graphing\\" FILES ${GRAPHING_FILES})

set(PROJECT_FILES
    ${PLATFORM_FILES}
    ${COMMON_FILES}
    ${RUNTIME_FILES}
    ${CORE_FILES}
    ${GEOMETRY_FILES}
    ${RENDERING_FILES}
    ${RENDERING_SUBPASSES_FILES}
    ${SCENE_GRAPH_FILES}
    ${SCENE_GRAPH_COMPONENT_FILES}
    ${SCENE_GRAPH_SCRIPTS_FILES}
    ${GRAPHING_FILES})

    # Add files based on platform
    if(ANDROID)
        list(APPEND PROJECT_FILES ${ANDROID_FILES})
        message(STATUS "Android platform detected")
    else()
            list(APPEND PROJECT_FILES ${GLFW_FILES})
            if(WIN32)
                list(APPEND PROJECT_FILES ${WINDOWS_FILES})
                message(STATUS "Windows platform detected")
            else()
                list(APPEND PROJECT_FILES ${UNIX_FILES})
                message(STATUS "Unix platform detected")
            endif()
    endif()

# Pre compiled headers
vulkan_samples_pch(PROJECT_FILES pch.cpp)

add_library(${PROJECT_NAME} STATIC ${PROJECT_FILES})

# compiler flags based on compiler type
if(NOT MSVC)
    target_compile_options(${PROJECT_NAME} PUBLIC -fexceptions)
endif()

if(MSVC)
    target_compile_options(${PROJECT_NAME} PUBLIC /MP)
endif()

if(${VKB_VALIDATION_LAYERS})
    target_compile_definitions(${PROJECT_NAME} PUBLIC VKB_VALIDATION_LAYERS)
endif()

if(${VKB_WARNINGS_AS_ERRORS})
    message(STATUS "Warnings as Errors Enabled")
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        target_compile_options(${PROJECT_NAME} PRIVATE -Werror)
    elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
        target_compile_options(${PROJECT_NAME} PRIVATE /W3 /WX)
    endif()
endif()

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Link third party libraries
target_link_libraries(${PROJECT_NAME}
    glm
    glslang
    SPIRV
    spirv-cross-glsl
    glslang-default-resource-limits)

# Link platform specific libraries
if(ANDROID)
    target_link_libraries(${PROJECT_NAME} log android native_app_glue)
else()
    if (DIRECT_TO_DISPLAY)
        target_link_libraries(${PROJECT_NAME} dl)
    else()
        target_link_libraries(${PROJECT_NAME} glfw)
    endif()
endif()
