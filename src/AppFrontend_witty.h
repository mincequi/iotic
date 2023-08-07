#pragma once

class FeedManager;

class AppFrontend {
public:
    static int runBlocking(int argc, char* argv[], const FeedManager& liveFeed);

private:
    AppFrontend();
};
