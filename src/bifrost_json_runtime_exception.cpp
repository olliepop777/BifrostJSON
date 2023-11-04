#include "bifrost_json_runtime_exception.h"

const std::string BifrostJsonRuntimeException::EXCEPTION_NAME = "BifrostJsonRuntimeException: ";

BifrostJsonRuntimeException::BifrostJsonRuntimeException(const char* message)
    : m_message(EXCEPTION_NAME)
{
    m_message += message;
}

const char* BifrostJsonRuntimeException::what() const noexcept
{
    return m_message.c_str();
}