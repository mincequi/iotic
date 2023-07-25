#include "FeedManager.h"

FeedManager::FeedManager()
    : _statsFeed(_liveFeed) {
}

const LiveFeed& FeedManager::liveFeed() const {
    return _liveFeed;
}

const StatsFeed& FeedManager::statsFeed() const {
    return _statsFeed;
}
