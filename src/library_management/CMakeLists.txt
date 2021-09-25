add_library(library_management STATIC
    book_management.c
    loans_management.c
    user_management.c
)

target_include_directories(library_management PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)
