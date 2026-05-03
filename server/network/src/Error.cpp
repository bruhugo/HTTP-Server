#include "Error.hpp"
#include "Types.hpp"

using namespace server::network;

HttpError::HttpError(int s): status{s} {}
HttpError::HttpError(int s, std::string p): 
    status{s}, payload{p} {}

HttpError::~HttpError(){}

const char* HttpError::what() const noexcept{
    return payload.c_str();
}

HttpError HttpError::BadRequestError(std::string payload){
    return HttpError(Status::BadRequest, payload);
}

HttpError HttpError::NotFoundError(std::string payload){
    return HttpError(Status::NotFound, payload);
}

HttpError HttpError::ServerError(std::string payload){
    return HttpError(Status::InternalServerError, payload);
}

std::string HttpError::getPayload() const noexcept {
    return payload;
}

int HttpError::getStatus() const noexcept {
    return status;
}
