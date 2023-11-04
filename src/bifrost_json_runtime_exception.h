#ifndef BIFROST_JSON_RUNTIME_EXCEPTION_H
#define BIFROST_JSON_RUNTIME_EXCEPTION_H

#include <exception>
#include <string>

class BifrostJsonRuntimeException : public std::exception {
public:
    BifrostJsonRuntimeException(const char* message);

    const char* what() const noexcept override;

private:
    static const std::string EXCEPTION_NAME;
    std::string m_message;
};

#endif // BIFROST_JSON_RUNTIME_EXCEPTION_H