add_executable(${PROJECT_NAME} 
    Yay.cpp
    Grammar.cpp
    DfaBuilder.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC
    "${PROJECT_BINARY_DIR}"
)