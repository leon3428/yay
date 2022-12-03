add_executable(${PROJECT_NAME} 
    Yay.cpp
    Grammar.cpp
    TableBuilder.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC
    "${PROJECT_BINARY_DIR}"
)

add_subdirectory(analizator)