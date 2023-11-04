# Build and install the library
add_library( BifrostJSONOps SHARED)
target_compile_definitions( BifrostJSONOps PRIVATE BIFROST_JSON_BUILD_NODEDEF_DLL )
target_sources( BifrostJSONOps PRIVATE 
    json_to_bif_obj.cpp 
    parse_access_expr.cpp
    bifrost_json_runtime_exception.cpp
    bifrost_json.cpp
)
target_link_libraries( BifrostJSONOps PUBLIC Amino::Cpp )
target_link_libraries( BifrostJSONOps PUBLIC Amino::Core )
target_link_libraries( BifrostJSONOps PUBLIC Bifrost::Object )
bifrost_set_install_rpath(BifrostJSONOps)
install( TARGETS BifrostJSONOps DESTINATION lib )

# Generate and install the json files
set( headers_to_parse 
    bifrost_json.h
)

bifrost_header_parser( 
    BifrostJSONJsonFiles  # Name of the target.
    "json"                 # Install dir - relative to install prefix.
    INCLUDES    ${CMAKE_CURRENT_SOURCE_DIR}
    INPUTS      ${headers_to_parse})

# Create config json file to load the bifrost pack
set( CONFIG_PACK_VERSION "${PROJECT_VERSION}")
set( CONFIG_PACK_NAME "bifrost_json")
set( CONFIG_SHARED_LIBS "./lib")
set( CONFIG_JSON_FILES "./json")
set( config_json ${CMAKE_BINARY_DIR}/${PROJECT_NAME}PackConfig.json )
configure_file(PackConfig.json.in ${config_json} )
install(FILES ${config_json} DESTINATION . )

add_subdirectory(resources)
add_subdirectory(compounds)