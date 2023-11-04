#include "parse_access_expr.h"

AccessExpr::AccessType AccessExpr::char_to_access_type(char cur_char)
{
    if (cur_char == OPEN_CURLY || cur_char == CLOSE_CURLY) {
        return AccessType::OBJECT;
    } else if (cur_char == OPEN_BRACKET || cur_char == CLOSE_BRACKET) {
        return AccessType::ARRAY;
    } else {
        return AccessType::CHAR_TOKEN;
    }
}

bool AccessExpr::is_opening_char(char cur_char)
{
    if (cur_char == OPEN_CURLY || cur_char == OPEN_BRACKET) {
        return true;
    } else {
        return false;
    }
}

bool AccessExpr::is_closing_char(char cur_char)
{
    if (cur_char == CLOSE_CURLY || cur_char == CLOSE_BRACKET) {
        return true;
    } else {
        return false;
    }
}

std::string strip_whitespace(Amino::String amino_str)
{
    std::string str = std::string(amino_str.c_str());
    const char whitespace = ' ';
    for (auto iter = str.begin(); iter != str.end();) {
        if ((*iter) == whitespace) {
            iter = str.erase(iter);
        } else {
            ++iter;
        }
    }
    return str;
}

void set_arr_index_accessor(Amino::MutablePtr<Bifrost::Object>& bif_obj,
    std::string& cur_token,
    const Amino::String& access_key, bool& success,
    Amino::String& msg_if_failed)
{
    int index;
    try {
        index = std::stoi(cur_token);
    } catch (const std::exception& _) {
        success = false;
        msg_if_failed = "Non-numeric value inside array access expression or out of range for "
                        "integer index access";
        return;
    }

    bif_obj->setProperty(access_key, index);
}

void create_new_token_bif_obj(
    Amino::MutablePtr<Amino::Array<Amino::Ptr<Bifrost::Object>>>& token_arr,
    std::string& cur_token, char opening_char, bool& success,
    Amino::String& msg_if_failed)
{
    Amino::MutablePtr<Bifrost::Object> bif_obj = Bifrost::createObject();
    int cur_token_index = 0;
    AccessExpr::AccessType access_type = AccessExpr::char_to_access_type(opening_char);

    const Amino::String type_key("is_object");
    bool is_object = access_type == AccessExpr::AccessType::OBJECT;
    bif_obj->setProperty(type_key, is_object);
    const Amino::String access_key("accessor");
    if (is_object) {
        bif_obj->setProperty(access_key, Amino::String(cur_token.c_str()));
    } else {
        set_arr_index_accessor(bif_obj, cur_token, access_key, success,
            msg_if_failed);
    }

    if (success) {
        token_arr->emplace_back(std::move(bif_obj));
    } else {
        // Reset it manually for safety
        bif_obj.reset();
    }
}

void parse_expression(
    std::string& access_expr,
    Amino::MutablePtr<Amino::Array<Amino::Ptr<Bifrost::Object>>>& token_arr,
    bool& success, Amino::String& msg_if_failed)
{
    std::string cur_token = "";
    char opening_char;
    bool is_opening_char;
    bool is_closing_char;
    bool is_escape_char;
    bool is_opened = false;
    bool saw_escape_char = false;
    for (char cur_char : access_expr) {
        is_opening_char = AccessExpr::is_opening_char(cur_char);
        is_closing_char = AccessExpr::is_closing_char(cur_char);
        is_escape_char = (cur_char == AccessExpr::ESCAPE);
        if (cur_token.empty() && !is_opened && !is_opening_char) {
            // Fail when we expect an opening char but it's not
            success = false;
            msg_if_failed = "Expected an opening character at start or after a close but was not "
                            "found.";
            return;
        } else if (!is_opened) {
            opening_char = cur_char;
            is_opened = true;
            continue;
        }

        if (saw_escape_char) {
            if (is_opening_char || is_closing_char || is_escape_char) {
                cur_token += cur_char;
                saw_escape_char = false;
                continue;
            } else {
                success = false;
                msg_if_failed = "Saw an escape char followed by non opening, closing or backslash "
                                "\\ char";
            }
        }

        if (is_opening_char) {
            success = false;
            msg_if_failed = "Two consecutive opening chars without a closing char between.";
            return;
        }

        if (is_closing_char) {
            if (AccessExpr::char_to_access_type(opening_char) != AccessExpr::char_to_access_type(cur_char)) {
                success = false;
                msg_if_failed = "Mismatching opening and closing char types.";
            } else if (cur_token.empty()) {
                success = false;
                msg_if_failed = "Accessor was empty";
            } else {
                // This function will set its own succes and failure messages
                create_new_token_bif_obj(token_arr, cur_token, opening_char, success,
                    msg_if_failed);
            }
            if (!success) {
                return;
            }
            cur_token = "";
            is_opened = false;
            continue;
        }

        saw_escape_char = is_escape_char;

        if (saw_escape_char) {
            continue;
        }

        // Default case just keep appending to cur_token
        cur_token += cur_char;
    }

    if (token_arr->size() == 0 || !cur_token.empty() || is_opened) {
        success = false;
        msg_if_failed = "Incomplete or empty access expression";
    }
}

Amino::MutablePtr<Amino::Array<Amino::Ptr<Bifrost::Object>>> create_empty_bif_obj_arr()
{
    Amino::Array<Amino::Ptr<Bifrost::Object>> amino_arr_bif_obj(0);
    return Amino::newMutablePtr<Amino::Array<Amino::Ptr<Bifrost::Object>>>(
        std::move(amino_arr_bif_obj));
}
