if(ANDROID)
    add_library( Simple_VertexShader SHARED Simple_VertexShader.c )
    target_link_libraries( Simple_VertexShader Common )
else()
    add_executable( Simple_VertexShader Simple_VertexShader.c )
    target_link_libraries( Simple_VertexShader Common )
endif()