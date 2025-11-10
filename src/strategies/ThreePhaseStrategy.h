#pragma once

#include "Strategy.h"

class ThreePhaseStrategy : public Strategy {
public:
    ThreePhaseStrategy(const std::string& thingId);

private:
    json toJson() const override;
};
