file( GLOB ALL_COMPOUND_JSON_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.json )

set( bifrost_json_compounds )
foreach( compound_json_file ${ALL_COMPOUND_JSON_FILES})
   get_filename_component( basename ${compound_json_file} NAME )
   list( APPEND bifrost_json_compounds ${basename})
endforeach()

install(FILES ${bifrost_json_compounds}
    DESTINATION "${BIFROST_JSON_INSTALL_DIR}/json")