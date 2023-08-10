#ifndef STATSFEED_H
#define STATSFEED_H

#include <rpp/subjects.hpp>
#include <rppqt/sources.hpp>

class LiveFeed;

class StatsFeed {
public:
    StatsFeed(const LiveFeed& liveFeed);

    void reset() const;

    rpp::dynamic_observable<int> maxSummedInverterPower() const;
    rpp::dynamic_observable<int> maxSitePower() const;

    /**
     * @brief maxPower, used to scale bars and chart
     * @return
     */
    rpp::dynamic_observable<int> maxPower() const;

private:
    rpp::subjects::behavior_subject<int> _maxSummedInvertersPowerSubject;
    rpp::subjects::behavior_subject<int> _maxSitePowerSubject;
};

#endif // STATSFEED_H
