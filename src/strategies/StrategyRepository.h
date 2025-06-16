#pragma once

#include <list>
#include <rpp/observables/dynamic_observable.hpp>
#include <rpp/subjects/behavior_subject.hpp>

class Strategy;
namespace uvw_iot {
class ThingRepository;
}

class StrategyRepository {
public:
    StrategyRepository(const uvw_iot::ThingRepository& thingRepository);
    ~StrategyRepository() = default;

    inline void addStrategy(std::shared_ptr<Strategy> strategy) {
        _strategies.push_back(std::move(strategy));
    }

    inline const std::list<std::shared_ptr<Strategy>>& strategies() const {
        return _strategies;
    }

    inline rpp::dynamic_observable<std::list<std::shared_ptr<Strategy>>> strategiesObservable() const {
        return _strategiesSubject.get_observable();
    }

private:
    const uvw_iot::ThingRepository& _thingRepository;

    rpp::subjects::behavior_subject<std::list<std::shared_ptr<Strategy>>> _strategiesSubject;
    std::list<std::shared_ptr<Strategy>> _strategies;
};
