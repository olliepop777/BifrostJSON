#ifndef BIFROST_JSON_EXPORT_H
#define BIFROST_JSON_EXPORT_H

#if defined(_WIN32)
#define BIFROST_JSON_EXPORT __declspec(dllexport)
#define BIFROST_JSON_IMPORT __declspec(dllimport)
#elif defined(__GNUC__)
#define BIFROST_JSON_EXPORT __attribute__((visibility("default")))
#define BIFROST_JSON_IMPORT __attribute__((visibility("default")))
#else
#error Unsupported platform.
#endif

#if defined(BIFROST_JSON_BUILD_NODEDEF_DLL)
#define BIFROST_JSON_DECL BIFROST_JSON_EXPORT
#else
#define BIFROST_JSON_DECL BIFROST_JSON_IMPORT
#endif

#endif
