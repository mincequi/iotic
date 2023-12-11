#pragma once

#include <things/Thing.h>

class CandidatesRepository {
public:
    CandidatesRepository();

    dynamic_observable<ThingPtr> candidateAdded() const;
};
