file( GLOB ALL_MD_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.md )

set( bifrost_json_docs )
foreach( doc_file ${ALL_MD_FILES})
   get_filename_component( basename ${doc_file} NAME )
   list( APPEND bifrost_json_docs ${basename})
endforeach()

install(FILES ${bifrost_json_docs}
    DESTINATION "${BIFROST_JSON_INSTALL_DIR}/docs")