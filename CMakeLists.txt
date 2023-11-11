cmake_minimum_required(VERSION 3.27.3)

project(procgenapp)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(wxWidgets CONFIG REQUIRED)

file(GLOB_RECURSE SRC_FILES Source/*.cpp Source/*.h)
add_executable(${PROJECT_NAME} ${SRC_FILES})

target_link_libraries(procgenapp PRIVATE wx::core wx::base wx::gl wx::propgrid GLEW::GLEW)

if(WIN32)
  target_link_options(procgenapp PRIVATE "/SUBSYSTEM:WINDOWS")
else()
  target_link_options(procgenapp PRIVATE "-Wl,--subsystem,windows")
endif()

target_include_directories(procgenapp PRIVATE ${CMAKE_SOURCE_DIR}/Source/)

add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/Bin/ $<TARGET_FILE_DIR:procgenapp>)
