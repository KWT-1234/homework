cmake_minimum_required(VERSION 3.10)
project(FoodOrderingSystem VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files
set(SOURCES
    src/main.cpp
    src/Menu.cpp
    src/Order.cpp
    src/UI.cpp
)

# Headers (for IDE reference)
set(HEADERS
    src/MenuItem.hpp
    src/Menu.hpp
    src/Order.hpp
    src/UI.hpp
)

add_executable(FoodOrderingSystem ${SOURCES} ${HEADERS})

if(MSVC)
    target_compile_options(FoodOrderingSystem PRIVATE /utf-8)
else()
    target_compile_options(FoodOrderingSystem PRIVATE -finput-charset=utf-8 -fexec-charset=utf-8)
endif()

# Ensure the executable can locate the data folder relative to working directory
# We can copy the data folder to build directory as a post-build step or just run from project root
