

#------------------------------------------------------------------------------
# Base settings

# Minimum CMake version.
cmake_minimum_required(VERSION 3.10)

# Name of the project. Also the name of the target that will be compiled.
# ${CMAKE_PROJECT_NAME} contains chosen name.
project(VulkanVisualizer)

# Compilator version.
set(CMAKE_CXX_STANDARD 23)
# CMAKE_CXX_STANDARD is required.
set(CMAKE_CXX_STANDARD_REQUIRED ON)


#------------------------------------------------------------------------------
# Paths to libraries

# GLFW - static linking.
# Folder with "include" and "lib" subfolders.
set(GLFW_PATH ${PROJECT_SOURCE_DIR} CACHE PATH "Path to GLFW.")
# GLM - source files.
# Folder containing source files.
set(GLM_PATH ${PROJECT_SOURCE_DIR} CACHE PATH "Path to GLM.")
# Vulkan - manual runtime loading.
# ShaderC - dynamic linking.
# Vulkan Memory Allocator - source files.
# Folder with "Include" and "Lib" subfolders.
set(VULKAN_PATH ${PROJECT_SOURCE_DIR} CACHE PATH "Path to Vulkan.")


#------------------------------------------------------------------------------
# Project source files handling

# Project source folder is scanned recursively for relevant files. 
file(GLOB_RECURSE PROJECT_SOURCE_FILES
	src/*.cpp
	src/*.h
	src/*.hpp
	src/*.txt
	src/*.vert
	src/*.frag
)


#------------------------------------------------------------------------------
# Output specification

# Executable name and list of project source files.
add_executable(${CMAKE_PROJECT_NAME}
	${PROJECT_SOURCE_FILES}
)


#------------------------------------------------------------------------------
# Include handling

# Paths to header files.
# SYSTEM should supress warnings from external libraries (might not work in MSVC).
target_include_directories(${CMAKE_PROJECT_NAME} SYSTEM PRIVATE
	${GLFW_PATH}/include
	${GLM_PATH}
	${VULKAN_PATH}/Include
)


#------------------------------------------------------------------------------
# Linking handling

# .lib files that need to be linked.
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE
	# GLFW
	${GLFW_PATH}/lib/glfw3.lib
	# ShaderC, contained within Vulkan SDK, different Debug and Release
	$<$<CONFIG:Debug>:${VULKAN_PATH}/Lib/shaderc_combinedd.lib>
	$<$<NOT:$<CONFIG:Debug>>:${VULKAN_PATH}/Lib/shaderc_combined.lib>
)


#------------------------------------------------------------------------------
# Other settings

# Sets Startup Project to be the target that compiles the application.
# Current project folder, property, value
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${CMAKE_PROJECT_NAME})

# Warning level for compilation.
target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE
	/W4
	# Files included using <...> will use W0.
	/external:W0
	/external:anglebrackets
	# Because of icosahedron subdivision.
	$<$<CXX_COMPILER_ID:MSVC>:/constexpr:steps10000000>
)

# Setup of compiler directives.
# Addition of VK_NO_PROTOTYPES, which sets up Vulkan header file for manual runtime load of Vulkan.
# Addition of GLFW_INCLUDE_VULKAN, which enables GLFW's Vulkan related functions.
# Addition of VULKAN_HPP_HANDLE_ERROR_OUT_OF_DATE_AS_SUCCESS, which ensures that vk::Result::eErrorOutOfDateKHR
# is returned as a value instead of an exception being thrown. Used during swapchain recreation.
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE VK_NO_PROTOTYPES GLFW_INCLUDE_VULKAN VULKAN_HPP_HANDLE_ERROR_OUT_OF_DATE_AS_SUCCESS)

# Suppression of linking warnings.
# "warning LNK4099: PDB 'shadercd.pdb' was not found with 'shaderc_combinedd.lib(shaderc.obj)'"
target_link_options(${CMAKE_PROJECT_NAME} PRIVATE "/ignore:4099")


#------------------------------------------------------------------------------
# Setting up of a project folder structure

# Files inside the project have the same structure as the files in the source folder.
# Iterating over each source file.
foreach(FILE_PATH_NAME IN ITEMS ${PROJECT_SOURCE_FILES})
	# Path to file is extracted.
    get_filename_component(FILE_PATH "${FILE_PATH_NAME}" PATH)
	# Relative path between third and fourth parameter is constructed. 
    file(RELATIVE_PATH FILE_PATH_SRC_REL "${CMAKE_CURRENT_SOURCE_DIR}/src" "${FILE_PATH}")
	# Relative path is formatted.
    string(REPLACE "/" "\\" FORMATTED_FILE_PATH_SRC_REL "${FILE_PATH_SRC_REL}")
	# File is put into the project folder that corresponds to the filepath.
    source_group("${FORMATTED_FILE_PATH_SRC_REL}" FILES "${FILE_PATH_NAME}")
endforeach()
