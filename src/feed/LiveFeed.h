#pragma once

#include <rpp/observers.hpp>
#include <rpp/observables.hpp>
#include <rpp/subjects/publish_subject.hpp>

/**
 * @brief The LiveFeed class
 *
 * Any subject within this class reflects the power each item imports or exports.
 * Positive values mean export, while negative values mean import.
 * A LiveFeed is never opinionated. The provided data is as raw as possible.
 */
class LiveFeed {
public:
    LiveFeed();

    /// Feed inverter power
    void setInverterPower(const std::string& id, int power) const;
    /// Feed meter power
    void setGridMeterPower(const std::string& id, int power) const;

    /// Combined power inputs, outputs of inverters, house and grid
    rpp::dynamic_observable<std::tuple<int, int>> combinedPowers() const;

    rpp::dynamic_observable<int> summedInverterPower() const;
    rpp::dynamic_observable<int> meterPower() const;

private:
    rpp::subjects::publish_subject<std::pair<std::string, int>> _invertersSubject;
    rpp::subjects::publish_subject<int> _summedInverterSubject;
    rpp::subjects::publish_subject<int> _meterSubject;

    rpp::subjects::publish_subject<std::tuple<int, int>> _combinedPowers;

    friend class AppBackend;
    friend class FeedManager;
};
