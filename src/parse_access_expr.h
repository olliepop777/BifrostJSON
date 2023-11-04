#ifndef PARSE_ACCES_EXPR_H
#define PARSE_ACCES_EXPR_H

// std includes
#include <string>

// Bifrost includes
#include <Amino/Core/Array.h>
#include <Amino/Core/Ptr.h>
#include <Amino/Core/String.h>
#include <Bifrost/Object/Object.h>

struct AccessExpr {
    static const char OPEN_CURLY = '{';
    static const char CLOSE_CURLY = '}';
    static const char OPEN_BRACKET = '[';
    static const char CLOSE_BRACKET = ']';
    static const char ESCAPE = '\\';

    enum struct AccessType { OBJECT,
        ARRAY,
        CHAR_TOKEN };

    static AccessType char_to_access_type(char cur_char);

    static bool is_opening_char(char cur_char);

    static bool is_closing_char(char cur_char);
};

std::string strip_whitespace(Amino::String amino_str);

void set_arr_index_accessor(Amino::MutablePtr<Bifrost::Object>& bif_obj,
    std::string& cur_token,
    const Amino::String& access_key, bool& success,
    Amino::String& msg_if_failed);

void create_new_token_bif_obj(
    Amino::MutablePtr<Amino::Array<Amino::Ptr<Bifrost::Object>>>& token_arr,
    std::string& cur_token, char opening_char, bool& success,
    Amino::String& msg_if_failed);

void parse_expression(
    std::string& access_expr,
    Amino::MutablePtr<Amino::Array<Amino::Ptr<Bifrost::Object>>>& token_arr,
    bool& success, Amino::String& msg_if_failed);

Amino::MutablePtr<Amino::Array<Amino::Ptr<Bifrost::Object>>>
create_empty_bif_obj_arr();

#endif // PARSE_ACCES_EXPR_H