
add_library(appEx SHARED appEx.cpp appEx.h)


# # Set the output directory for the shared library
# set_target_properties(appEx PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
# message("Target Location="${CMAKE_CURRENT_SOURCE_DIR}/lib/libappEx.so )

# # Move the header file to the include directory
# configure_file(${CMAKE_CURRENT_SOURCE_DIR}/appEx.h ${CMAKE_SOURCE_DIR}/include/appEx.h COPYONLY)
# message("Header Location="${CMAKE_SOURCE_DIR}/include/appEx.h )
