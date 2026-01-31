#pragma once

#include <uvw.hpp>
#include <llhttp.h>
#include <memory>
#include <string>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <atomic>

enum class HttpError {
    ResolveFailed,
    ConnectFailed,
    WriteFailed,
    ParseError,
    Timeout,
    Cancelled
};

using HttpHeaders = std::unordered_map<std::string, std::string>;

struct HttpCallbacks {
    std::function<void(const HttpHeaders&, int status)> on_headers;
    std::function<void(std::string_view)>               on_body;
    std::function<void()>                               on_done;
    std::function<void(HttpError)>                      on_error;
};

struct HttpResponse {
    int status = 0;
    HttpHeaders headers;
    std::string body; // optional if user wants full accumulation
};

class HttpRequest {
public:
    void cancel();
    bool cancelled() const;

private:
    friend class HttpService;
    struct Impl;
    std::unique_ptr<Impl> impl_;
    explicit HttpRequest(std::unique_ptr<Impl> impl);
};

class HttpService {
public:
    explicit HttpService(std::shared_ptr<uvw::loop> loop);

    std::shared_ptr<HttpRequest> get(
        std::string url,
        HttpCallbacks callbacks,
        std::chrono::milliseconds timeout = std::chrono::milliseconds{5000});

    void shutdown();

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};
