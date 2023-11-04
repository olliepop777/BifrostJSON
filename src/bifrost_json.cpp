#include "bifrost_json.h"

void BifrostJSON::read_json(Amino::String json_file,
    Amino::String json_str,
    bool output_a_bifrost_object,
    bool output_a_json_str,
    bool null_to_string,
    bool force_any_arrays,
    bool debug,
    int print_indent_level,
    Amino::MutablePtr<Bifrost::Object>& out_json_obj,
    Amino::String& out_json_str,
    bool& success,
    Amino::String& msg_if_failed)
{
    success = true;
    out_json_obj = Bifrost::createObject();
    out_json_str = Amino::String();

    std::string json_file_str = json_file.c_str();
    if (json_str.empty() && json_file_str.empty()) {
        success = false;
        msg_if_failed = "Both empty JSON file path and JSON string value";
        return;
    }
    bool use_json_file_path = json_str.empty();

    std::ifstream json_file_stream;
    if (use_json_file_path) {
        json_file_stream = std::ifstream(json_file_str);
        if (!json_file_stream) {
            success = false;
            msg_if_failed = "Could not open or find: \n" + json_file;
            return;
        }
    }

    ReadSettings settings = ReadSettings();
    set_read_settings(
        output_a_bifrost_object,
        output_a_json_str,
        null_to_string,
        force_any_arrays,
        debug,
        print_indent_level,
        settings);

    json json_data;
    try {
        if (use_json_file_path) {
            json_data = json::parse(json_file_stream);
        } else {
            json_data = json::parse(json_str.c_str());
        }
        print_json(json_data, settings);
    } catch (const json::exception& e) {
        success = false;
        msg_if_failed = "Exception parsing JSON for " + Amino::String(use_json_file_path ? "json_file" : "json_str")
            + ": " + (use_json_file_path ? json_file : "") + '\n'
            + "message: " + e.what() + '\n'
            + "nlohmann::json exception id: " + std::to_string(e.id).c_str();
        return;
    } catch (...) {
        success = false;
        msg_if_failed = "There was an unknown error reading JSON for "
            + Amino::String(use_json_file_path ? "json_file" : "json_str") + ":\n"
            + (use_json_file_path ? json_file : "");
        return;
    }

    try {
        create_json_to_bif_obj(json_data, out_json_obj, settings);
        create_json_to_amino_str(json_data, json_str, out_json_str, settings);
    } catch (const BifrostJsonRuntimeException& e) {
        success = false;
        msg_if_failed = e.what();
    }
}

void BifrostJSON::get_property_access_tokens(Amino::String access_expr,
    Amino::MutablePtr<Amino::Array<Amino::Ptr<Bifrost::Object>>>& token_arr,
    bool& success,
    Amino::String& msg_if_failed)
{

    std::string access_expr_str = strip_whitespace(access_expr);

    // Will be overwritten if there is an error
    success = true;
    token_arr = std::move(create_empty_bif_obj_arr());
    parse_expression(access_expr_str, token_arr, success, msg_if_failed);

    if (!success) {
        token_arr.reset();
        token_arr = std::move(create_empty_bif_obj_arr());
    }
}
