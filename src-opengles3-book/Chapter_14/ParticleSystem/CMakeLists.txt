if(ANDROID)
    add_library( ParticleSystem SHARED ParticleSystem.c )
    target_link_libraries( ParticleSystem Common )
else()
    add_executable( ParticleSystem ParticleSystem.c )
    target_link_libraries( ParticleSystem Common )

    configure_file(smoke.tga ${CMAKE_CURRENT_BINARY_DIR}/smoke.tga COPYONLY)
endif()
