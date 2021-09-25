add_executable(library_program
    main.c
    interface.c
)

target_link_libraries(library_program
    library_management
)


install(TARGETS library_program RUNTIME DESTINATION bin)
