cmake_minimum_required(VERSION 3.18)
project(vkrt)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(_src_dir "${CMAKE_CURRENT_SOURCE_DIR}/src")

file(GLOB _source_list "${_src_dir}/*.cpp" "${_src_dir}/*.hpp")

set(_target "vkrt")
add_executable(${_target} ${_source_list})

find_package(Vulkan REQUIRED)
add_subdirectory(submodules/glfw)

target_include_directories(${_target} PRIVATE ${_srcdir} ${Vulkan_INCLUDE_DIRS} "submodules/glfw/include")
target_link_libraries(${_target} PRIVATE glfw ${Vulkan_LIBRARIES})
target_compile_options(${_target} PRIVATE "/wd26812")