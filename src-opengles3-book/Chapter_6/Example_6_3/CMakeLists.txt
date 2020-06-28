if(ANDROID)
    add_library( Example_6_3 SHARED Example_6_3.c )
    target_link_libraries( Example_6_3 Common )
else()
    add_executable( Example_6_3 Example_6_3.c )
    target_link_libraries( Example_6_3 Common )
endif()

