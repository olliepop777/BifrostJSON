#ifndef BIFROST_JSON_HEADER_PARSER_MACROS
#define BIFROST_JSON_HEADER_PARSER_MACROS

#include <Amino/Cpp/Annotate.h>

#define BIFROST_JSON_FILE_BROWSER_OPEN                                   \
    AMINO_ANNOTATE(                                                      \
        "Amino::Port metadata=[{UIWidget, string, FileBrowserWidget}, "  \
        "{UIWidgetProp, string, browserMode=open;filter=\"JSON (*.json)" \
        ";;All (*.*)\"}] ")

#define BIFROST_JSON_SET_INPUT_DEFAULT_VAL(DEFAULT_VALUE) \
    AMINO_ANNOTATE(                                       \
        "Amino::Port value=" #DEFAULT_VALUE)

#define BIFROST_JSON_SET_NODE_METADATA(DOC_FILENAME, ICON_FILENAME, INTERNAL)  \
    AMINO_ANNOTATE("Amino::Node "                                              \
                   "metadata=[{documentation, string, \\../docs/" DOC_FILENAME \
                   ".md},"                                                     \
                   "{icon, string, \\../icons/" ICON_FILENAME "},"             \
                   "{internal, bool," INTERNAL "}]")

#endif