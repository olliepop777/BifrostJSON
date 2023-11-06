#include "json_to_bif_obj.h"

//------------------------------------------------------------------------------

void set_read_settings(
    bool output_a_bifrost_object,
    bool output_a_json_str,
    bool null_to_string,
    bool force_any_arrays,
    bool debug,
    int print_indent_level,
    ReadSettings& settings)
{
    settings.output_a_bifrost_object = output_a_bifrost_object;
    settings.output_a_json_str = output_a_json_str;
    settings.null_to_string = null_to_string;
    settings.force_any_arrays = force_any_arrays;
    settings.debug = debug;
    settings.print_indent_level = print_indent_level;
}

template <typename T>
T get_iter_val(json::iterator& iter, ReadSettings& settings)
{
    // Generic case
    return (T)iter.value();
}

template <>
bool get_iter_val<bool>(json::iterator& iter, ReadSettings& settings)
{
    if (iter->type() == json::value_t::null) {
        return false;
    } else {
        return (bool)iter.value();
    }
}

template <>
Amino::String get_iter_val<Amino::String>(json::iterator& iter, ReadSettings& settings)
{
    if (iter->type() == json::value_t::null) {
        return Amino::String("null");
    } else {
        return Amino::String(((std::string)iter.value()).c_str());
    }
}

template <>
Amino::Ptr<Bifrost::Object> get_iter_val<Amino::Ptr<Bifrost::Object>>(
    json::iterator& iter, ReadSettings& settings)
{
    json json_data = iter.value();
    return std::move(recursive_create_bif_obj(json_data, settings));
}

void set_primitive_key_value(json::iterator& iter,
    Amino::MutablePtr<Bifrost::Object>& bif_obj,
    Amino::String& amino_key,
    ReadSettings& settings)
{
    switch (iter->type()) {
    case json::value_t::boolean:
        bif_obj->setProperty(amino_key, get_iter_val<bool>(iter, settings));
        break;
    case json::value_t::string:
        bif_obj->setProperty(amino_key, get_iter_val<Amino::String>(iter, settings));
        break;
    case json::value_t::number_integer:
    case json::value_t::number_unsigned:
        bif_obj->setProperty(amino_key, get_iter_val<std::int64_t>(iter, settings));
        break;
    case json::value_t::number_float:
        bif_obj->setProperty(amino_key, get_iter_val<double>(iter, settings));
        break;
    case json::value_t::null:
        if (settings.null_to_string) {
            bif_obj->setProperty(amino_key, Amino::String("null"));
        } else {
            bif_obj->setProperty(amino_key, false);
        }
        break;
    default:
        throw BifrostJsonRuntimeException("JSON primitive type detected incorrectly.");
        break;
    }
}

Amino::Any get_base_arr_any_value(json::iterator& iter, ReadSettings& settings)
{
    Amino::Any return_val;

    switch (iter->type()) {
    case json::value_t::boolean:
        return_val = get_iter_val<bool>(iter, settings);
        break;
    case json::value_t::string:
        return_val = std::move(get_iter_val<Amino::String>(iter, settings));
        break;
    case json::value_t::number_integer:
    case json::value_t::number_unsigned:
        return_val = get_iter_val<std::int64_t>(iter, settings);
        break;
    case json::value_t::number_float:
        return_val = get_iter_val<double>(iter, settings);
        break;
    case json::value_t::null:
        if (settings.null_to_string) {
            return_val = Amino::String("null");
        } else {
            return_val = false;
        }
        break;
    case json::value_t::object:
        return_val = std::move(get_iter_val<Amino::Ptr<Bifrost::Object>>(iter, settings));
        break;
    default:
        throw BifrostJsonRuntimeException("JSON primitive type detected incorrectly.");
        break;
    }

    return return_val;
}

template <typename T>
void set_amino_arr_values_base(json& json_data, T& array_ptr,
    std::size_t json_size, ReadSettings& settings)
{
    using ptr_type = typename RemovePtrReference<decltype(array_ptr)>::type;
    using amino_arr_type = typename ptr_type::element_type;
    using base_prim_type = typename amino_arr_type::value_type;
    amino_arr_type amino_arr(json_size);

    int cur_array_index = 0;
    for (auto iter = json_data.begin(); iter != json_data.end(); ++iter) {
        amino_arr[cur_array_index] = get_iter_val<base_prim_type>(iter, settings);
        cur_array_index++;
    }
    // Memory of previous pointer should be released when using move semantics
    array_ptr = std::move(Amino::newClassPtr<amino_arr_type>(std::move(amino_arr)));
}

template <typename T, std::size_t depth>
T& set_amino_arr_values_recursive_helper<T, depth>::apply(json& json_data, T& array_ptr, std::size_t json_size, ReadSettings& settings)
{
    using ptr_type = typename RemovePtrReference<decltype(array_ptr)>::type;
    using amino_arr_type = typename ptr_type::element_type;
    using base_prim_type = typename amino_arr_type::value_type;
    amino_arr_type amino_arr(json_size);

    int cur_array_index = 0;
    for (auto iter = json_data.begin(); iter != json_data.end(); ++iter) {
        auto& nested_arr_ptr = amino_arr[cur_array_index];
        using nested_arr_type = decltype(nested_arr_ptr);
        amino_arr[cur_array_index] = set_amino_arr_values_recursive_helper<
            nested_arr_type, depth - 1>::apply(*iter, nested_arr_ptr,
            iter->size(), settings);
        cur_array_index++;
    }
    // Memory of previous pointer should be released when using move semantics
    array_ptr = std::move(Amino::newClassPtr<amino_arr_type>(std::move(amino_arr)));
    return array_ptr;
}

template <typename T>
T& set_amino_arr_values_recursive_helper<T, 1>::apply(json& json_data, T& array_ptr, std::size_t json_size, ReadSettings& settings)
{
    set_amino_arr_values_base<T>(json_data, array_ptr, json_size, settings);
    return array_ptr;
}

template <typename T, std::size_t depth>
void set_amino_arr_values_recursive(json& json_data, T& array_ptr,
    std::size_t json_size, ReadSettings& settings)
{
    set_amino_arr_values_recursive_helper<T, depth>::apply(json_data, array_ptr,
        json_size, settings);
}

Amino::Ptr<Amino::Array<Amino::Any>> create_amino_any_arr_recursive(
    json& json_data, std::size_t json_size, ReadSettings& settings)
{
    Amino::Array<Amino::Any> amino_array(json_size);

    int cur_array_index = 0;
    for (auto iter = json_data.begin(); iter != json_data.end(); ++iter) {
        if (iter->is_primitive() || iter->is_object()) {
            amino_array[cur_array_index] = get_base_arr_any_value(iter, settings);
        } else {
            amino_array[cur_array_index] = create_amino_any_arr_recursive(*iter, iter->size(), settings);
        }
        cur_array_index++;
    }
    return Amino::newClassPtr<Amino::Array<Amino::Any>>(std::move(amino_array));
}

template <std::size_t depth, typename T>
void handle_homogenous_nested_array_set_value(
    json& json_data, Amino::MutablePtr<Bifrost::Object>& bif_obj,
    Amino::String& amino_key, std::size_t json_size, ReadSettings& settings)
{
    auto amino_array = Amino::ArrayD_t<depth, T>();
    using array_type = decltype(amino_array);
    auto array_ptr = Amino::newClassPtr<array_type>(std::move(amino_array));
    using ptr_type = decltype(array_ptr);
    set_amino_arr_values_recursive<ptr_type, depth>(json_data, array_ptr,
        json_size, settings);
    bif_obj->setProperty(amino_key, array_ptr);
}

template <std::size_t depth>
void handle_homogenous_nested_array_limited_depth(
    json& json_data, Amino::MutablePtr<Bifrost::Object>& bif_obj,
    Amino::String& amino_key, ArrayTypeInfo& array_type_info,
    std::size_t json_size,
    ReadSettings& settings)
{
    switch (array_type_info.json_type) {
    case json::value_t::boolean:
        handle_homogenous_nested_array_set_value<depth, bool>(
            json_data, bif_obj, amino_key, json_size, settings);
        break;
    case json::value_t::string:
        handle_homogenous_nested_array_set_value<depth, Amino::String>(
            json_data, bif_obj, amino_key, json_size, settings);
        break;
    case json::value_t::number_integer:
    case json::value_t::number_unsigned:
        handle_homogenous_nested_array_set_value<depth, std::int64_t>(
            json_data, bif_obj, amino_key, json_size, settings);
        break;
    case json::value_t::number_float:
        handle_homogenous_nested_array_set_value<depth, double>(
            json_data, bif_obj, amino_key, json_size, settings);
        break;
    case json::value_t::null:
        if (settings.null_to_string) {
            handle_homogenous_nested_array_set_value<depth, Amino::String>(
                json_data, bif_obj, amino_key, json_size, settings);
        } else {
            handle_homogenous_nested_array_set_value<depth, bool>(
                json_data, bif_obj, amino_key, json_size, settings);
        }
        break;
    case json::value_t::object:
        handle_homogenous_nested_array_set_value<depth, Amino::Ptr<Bifrost::Object>>(
            json_data, bif_obj, amino_key, json_size, settings);
        break;
    default:
        throw BifrostJsonRuntimeException("JSON array type detected incorrectly.");
        break;
    }
}

void handle_non_homogenous_array(json& json_data,
    Amino::MutablePtr<Bifrost::Object>& bif_obj,
    Amino::String& amino_key,
    std::size_t json_size,
    ReadSettings& settings)
{
    Amino::Ptr<Amino::Array<Amino::Any>> array_ptr = create_amino_any_arr_recursive(json_data, json_size, settings);
    bif_obj->setProperty(amino_key, array_ptr);
}

bool is_equivalent_number_type(json::value_t first_type, json::value_t cur_type)
{
    bool first_is_floating = first_type == json::value_t::number_float;
    bool second_is_floating = cur_type == json::value_t::number_float;
    bool first_is_integer = (first_type == json::value_t::number_integer || first_type == json::value_t::number_unsigned);
    bool second_is_integer = (cur_type == json::value_t::number_integer || cur_type == json::value_t::number_unsigned);

    return (first_is_floating && second_is_integer) || (first_is_integer && second_is_floating);
}

std::size_t get_array_type_info(json& json_data,
    ArrayTypeInfo& array_type_info)
{
    auto first_iter = json_data.begin();
    json::value_t first_type = first_iter->type();
    bool level_contains_array = false;
    std::size_t prev_cur_depth = 0;
    std::size_t cur_depth = 0;

    for (auto iter = json_data.begin(); iter != json_data.end(); ++iter) {
        if (!array_type_info.is_homogenous) {
            return 0;
        }
        if (iter->type() != first_type) {
            if (is_equivalent_number_type(first_type, iter->type())) {
                // If they are mixed then force everything to a floating point type
                first_type = json::value_t::number_float;
            } else {
                array_type_info.is_homogenous = false;
            }
        }
        if (iter->is_array()) {
            level_contains_array = true;
            cur_depth = get_array_type_info(*iter, array_type_info) + 1;
            // Check if array dimensions are equal throughout
            if (prev_cur_depth != cur_depth && iter != first_iter) {
                array_type_info.is_homogenous = false;
            }
            prev_cur_depth = cur_depth;
            if (array_type_info.depth < cur_depth) {
                array_type_info.depth = cur_depth;
            }
        }
    }

    if (!level_contains_array) {
        // Check that all the arrays in the level are the same type
        if (array_type_info.type_is_set && array_type_info.json_type != first_type) {
            array_type_info.is_homogenous = false;
        } else {
            array_type_info.type_is_set = true;
            array_type_info.json_type = first_type;
        }
        return 1;
    } else {
        return array_type_info.depth;
    }
}

void recursive_create_amino_array(json& json_data,
    Amino::MutablePtr<Bifrost::Object>& bif_obj,
    Amino::String& amino_key,
    std::size_t json_size,
    ReadSettings& settings)
{
    ArrayTypeInfo array_type_info;
    get_array_type_info(json_data, array_type_info);
    bool force_any_arrays = settings.force_any_arrays;

    if (force_any_arrays || !array_type_info.is_homogenous || array_type_info.depth > ArrayTypeInfo::MAX_NESTED_DEPTH) {
        handle_non_homogenous_array(json_data, bif_obj, amino_key, json_size, settings);
        return;
    }

    switch (array_type_info.depth) {
    case 3:
        handle_homogenous_nested_array_limited_depth<3>(
            json_data, bif_obj, amino_key, array_type_info, json_size, settings);
        break;
    case 2:
        handle_homogenous_nested_array_limited_depth<2>(
            json_data, bif_obj, amino_key, array_type_info, json_size, settings);
        break;
    case 1:
        handle_homogenous_nested_array_limited_depth<1>(
            json_data, bif_obj, amino_key, array_type_info, json_size, settings);
        break;
    default:
        // This function should never be called if the depth was not one of the
        // predetermined values
        throw BifrostJsonRuntimeException(
            "ArrayTypeInfo computed incorrectly, unknown case for depth count.");
        break;
    }
}

Amino::MutablePtr<Bifrost::Object> recursive_create_bif_obj(json& json_data, ReadSettings& settings)
{
    Amino::MutablePtr<Bifrost::Object> bif_obj = Bifrost::createObject();
    for (auto iter = json_data.begin(); iter != json_data.end(); ++iter) {
        if (iter->is_primitive()) {
            Amino::String amino_key = Amino::String(iter.key().c_str());
            set_primitive_key_value(iter, bif_obj, amino_key, settings);
        } else if (iter->is_object()) {
            Amino::MutablePtr<Bifrost::Object> nested_bif_obj = recursive_create_bif_obj(*iter, settings);
            Amino::String amino_key = Amino::String(iter.key().c_str());
            bif_obj->setProperty(amino_key, std::move(nested_bif_obj));
        } else if (iter->is_array()) {
            Amino::String amino_key = Amino::String(iter.key().c_str());
            recursive_create_amino_array(*iter, bif_obj, amino_key, iter->size(), settings);
        } else {
            throw BifrostJsonRuntimeException(
                "Unexpected call to object parser. Cannot create Bifrost Object");
        }
    }

    return bif_obj;
}

void create_json_to_bif_obj(json& json_data,
    Amino::MutablePtr<Bifrost::Object>& root_bif_obj,
    ReadSettings& settings)
{
    if (!settings.output_a_bifrost_object) {
        return;
    }
    const char* json_data_key_raw = "json_data";
    Amino::String json_data_key = Amino::String(json_data_key_raw);

    if (json_data.is_primitive()) {
        auto iter = json_data.begin();
        set_primitive_key_value(iter, root_bif_obj, json_data_key, settings);
    } else if (json_data.is_object()) {
        Amino::MutablePtr<Bifrost::Object> bif_obj = recursive_create_bif_obj(json_data, settings);
        root_bif_obj->setProperty(json_data_key, std::move(bif_obj));
    } else if (json_data.is_array()) {
        recursive_create_amino_array(json_data, root_bif_obj, json_data_key,
            json_data.size(), settings);
    } else {
        throw BifrostJsonRuntimeException(
            "Unexpected initial JSON type. Cannot parse to Bifrost Object.");
    }
}

void create_json_to_amino_str(json& json_data,
    Amino::String& in_json_str,
    Amino::String& out_json_str,
    ReadSettings& settings)
{
    if (!settings.output_a_json_str) {
        return;
    }

    if (!in_json_str.empty()) {
        out_json_str = in_json_str;
    } else {
        std::string json_data_str = json_data.dump(settings.print_indent_level);
        out_json_str = json_data_str.c_str();
    }
}

void print_json(json& json_data, ReadSettings& settings)
{
    if (!settings.debug) {
        return;
    }

    std::string json_data_str = json_data.dump(settings.print_indent_level);

    std::cout << "Dump parsed JSON:\n"
              << std::endl;
    std::cout << json_data_str << std::endl;
}
