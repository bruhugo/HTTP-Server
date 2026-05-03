#pragma once
#include <stdexcept>

namespace server{
namespace network{

struct HttpError : public std::exception{
public:
    HttpError(int status);
    HttpError(int status, std::string payload);

    ~HttpError();

    const char* what() const noexcept override;

    static HttpError ServerError(std::string payload);
    static HttpError NotFoundError(std::string payload);
    static HttpError BadRequestError(std::string payload);

    std::string getPayload() const noexcept;
    int getStatus() const noexcept;

private:
    int status;
    std::string payload;
};   

} 
} 


