#ifndef JSON_TO_BIF_OBJ_H
#define JSON_TO_BIF_OBJ_H

// std includes
#include <fstream>
#include <iostream>
#include <string>

// Third party includes
#include "../external/nlohmann_json-3.11.2/json.hpp"
using json = nlohmann::json;

// Bifrost includes
#include <Amino/Core/Array.h>
#include <Amino/Core/Ptr.h>
#include <Amino/Core/String.h>
#include <Bifrost/Object/Object.h>

// This package includes
#include "bifrost_json_runtime_exception.h"

struct ReadSettings {
    bool output_a_bifrost_object;
    bool output_a_json_str;
    bool null_to_string;
    bool force_any_arrays;
    bool debug;
    int print_indent_level;
};

void set_read_settings(
    bool output_a_bifrost_object,
    bool output_a_json_str,
    bool null_to_string,
    bool force_any_arrays,
    bool debug,
    int print_indent_level,
    ReadSettings& settings);

struct ArrayTypeInfo {
    static const std::size_t MAX_NESTED_DEPTH = 3;
    static const json::value_t DEFAULT_ARRAY_TYPE = json::value_t::number_float;

    bool is_homogenous = true;
    json::value_t json_type = json::value_t::null;
    std::size_t depth = 1;
    bool type_is_set = false;
};

template <typename T>
struct RemovePtrReference {
    using type = T;
};

template <typename T>
struct RemovePtrReference<T&> {
    using type = T;
};

// Handle type specific conversion for primitive json types
template <typename T>
T get_iter_val(json::iterator& iter, ReadSettings& settings);

template <>
bool get_iter_val<bool>(json::iterator& iter, ReadSettings& settings);

template <>
Amino::String get_iter_val<Amino::String>(json::iterator& iter, ReadSettings& settings);

template <>
Amino::Ptr<Bifrost::Object> get_iter_val<Amino::Ptr<Bifrost::Object>>(json::iterator& iter, ReadSettings& settings);

void set_primitive_key_value(json::iterator& iter,
    Amino::MutablePtr<Bifrost::Object>& bif_obj,
    Amino::String& amino_key,
    ReadSettings& settings);

Amino::Any get_base_arr_any_value(json::iterator& iter, ReadSettings& settings);

template <typename T>
void set_amino_arr_values_base(json& json_data,
    T& array_ptr,
    std::size_t json_size,
    ReadSettings& settings);

template <typename T, std::size_t depth>
struct set_amino_arr_values_recursive_helper {
    static T& apply(json& json_data, T& array_ptr, std::size_t json_size, ReadSettings& settings);
};

template <typename T>
struct set_amino_arr_values_recursive_helper<T, 1> {
    static T& apply(json& json_data, T& array_ptr, std::size_t json_size, ReadSettings& settings);
};

template <typename T, std::size_t depth>
void set_amino_arr_values_recursive(json& json_data, T& array_ptr,
    std::size_t json_size, ReadSettings& settings);

Amino::Ptr<Amino::Array<Amino::Any>> create_amino_any_arr_recursive(
    json& json_data, std::size_t json_size, ReadSettings& settings);

template <std::size_t depth, typename T>
void handle_homogenous_nested_array_set_value(
    json& json_data, Amino::MutablePtr<Bifrost::Object>& bif_obj,
    Amino::String& amino_key, std::size_t json_size, ReadSettings& settings);

// Assumes a homogenous array type has been detected in JSON data
template <std::size_t depth>
void handle_homogenous_nested_array_limited_depth(
    json& json_data, Amino::MutablePtr<Bifrost::Object>& bif_obj,
    Amino::String& amino_key, ArrayTypeInfo& array_type_info,
    std::size_t json_size,
    ReadSettings& settings);

void handle_non_homogenous_array(json& json_data,
    Amino::MutablePtr<Bifrost::Object>& bif_obj,
    Amino::String& amino_key,
    std::size_t json_size,
    ReadSettings& settings);

// JSON data only has a type called number which can either be an int or double
// in C++. This will return true if and only if the types are both numbers AND
// different number types. That is, if one type is json::value_t::number_float
// and the other is (json::value_t::number_integer OR json::value_t::number_unsigned)
bool is_equivalent_number_type(json::value_t first_type, json::value_t cur_type);

// Traverse once to get all the info we need
std::size_t get_array_type_info(json& json_data,
    ArrayTypeInfo& array_type_info);

void recursive_create_amino_array(json& json_data,
    Amino::MutablePtr<Bifrost::Object>& bif_obj,
    Amino::String& amino_key,
    std::size_t json_size,
    ReadSettings& settings);

Amino::MutablePtr<Bifrost::Object> recursive_create_bif_obj(json& json_data, ReadSettings& settings);

void create_json_to_bif_obj(json& json_data,
    Amino::MutablePtr<Bifrost::Object>& root_bif_obj,
    ReadSettings& settings);

void create_json_to_amino_str(json& json_data,
    Amino::String& in_json_str,
    Amino::String& out_json_str,
    ReadSettings& settings);

void print_json(json& json_data, ReadSettings& settings);

#endif // JSON_TO_BIF_OBJ_H