# The following lines of boilerplate have to be in your project's CMakeLists
# in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

add_compile_options(-fdiagnostics-color=always)


include($ENV{IDF_PATH}/tools/cmake/project.cmake)

get_filename_component(PROJ_NAME "${CMAKE_CURRENT_SOURCE_DIR}" NAME)
project(${PROJ_NAME})


spiffs_create_partition_image(json ${CMAKE_CURRENT_LIST_DIR}/json FLASH_IN_PROJECT)


git_describe(PROJECT_VERSION ${COMPONENT_DIR})
message("Project commit: " ${PROJECT_VERSION})
