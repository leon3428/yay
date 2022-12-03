add_executable(${PROJECT_NAME} 
    Yay.cpp
    Grammar.cpp
    TableBuilder.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC
    "${PROJECT_BINARY_DIR}"
)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror)

add_subdirectory(analizator)