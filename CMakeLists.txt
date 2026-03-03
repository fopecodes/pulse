cmake_minimum_required(VERSION 3.25)
project(pulse LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

FetchContent_Declare(
    ftxui
    GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
    GIT_TAG v6.1.9
)
FetchContent_MakeAvailable(ftxui)

add_executable(pulse
    src/main.cpp
    src/proc/cpu.cpp
    src/proc/memory.cpp
    src/proc/disk.cpp
    src/proc/network.cpp
    src/proc/process.cpp
    src/ui/cpu_panel.cpp
    src/ui/memory_panel.cpp
    src/ui/disk_panel.cpp
    src/ui/network_panel.cpp
    src/ui/process_panel.cpp
)

target_include_directories(pulse PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(pulse PRIVATE
    ftxui::screen
    ftxui::dom
    ftxui::component
)
