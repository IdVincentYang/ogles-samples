if(ANDROID)
    add_library( MipMap2D SHARED MipMap2D.c )
    target_link_libraries( MipMap2D Common )
else()
    add_executable( MipMap2D MipMap2D.c )
    target_link_libraries( MipMap2D Common )
endif()