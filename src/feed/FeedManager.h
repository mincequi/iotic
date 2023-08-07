#pragma once

#include "LiveFeed.h"
#include "StatsFeed.h"

class FeedManager {
public:
    FeedManager();

    const LiveFeed& liveFeed() const;
    const StatsFeed& statsFeed() const;

private:
    LiveFeed _liveFeed;
    StatsFeed _statsFeed;

    friend class AppBackend;
};
