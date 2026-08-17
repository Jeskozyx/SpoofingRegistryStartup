cmake_minimum_required(VERSION 3.10)
project(accsessible)

set(CMAKE_CXX_STANDARD 17)

# Beri tahu compiler bahwa kita buat aplikasi Windows GUI (WIN32 / no console)
add_executable(accsessible WIN32 
    main.cpp
    audio_manager.cpp
    startup_manager.cpp
)

# Link ke library Windows yang dibutuhkan
target_link_libraries(accsessible
    user32
    winmm
    ole32
    gdi32
)