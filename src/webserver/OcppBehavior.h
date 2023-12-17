#pragma once

#include <App.h>

struct UserData {};

class OcppBehavior {
public:
    OcppBehavior() = delete;
    static uWS::App::WebSocketBehavior<UserData> create();
};
