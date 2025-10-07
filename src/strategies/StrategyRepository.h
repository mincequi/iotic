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
    using Strategies = std::list<std::shared_ptr<Strategy>>;

    StrategyRepository(const uvw_iot::ThingRepository& thingRepository);
    ~StrategyRepository() = default;

    void addStrategy(std::shared_ptr<Strategy> strategy);

    inline const Strategies& strategies() const {
        return _strategies;
    }

    inline rpp::dynamic_observable<Strategies> strategiesObservable() const {
        return _strategiesSubject.get_observable();
    }

private:
    const uvw_iot::ThingRepository& _thingRepository;

    rpp::subjects::behavior_subject<Strategies> _strategiesSubject;
    Strategies _strategies;
};
