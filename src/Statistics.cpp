#include "Statistics.h"

#include <things/sunspec/SunSpecBlock.h>
#include <things/sunspec/SunSpecThing.h>

Statistics::Statistics() {
}

void Statistics::reset() {
    _models.clear();
}

void Statistics::feedModel(const sunspec::SunSpecThing& thing, const sunspec::Model& model) {
    auto& model_ = _models[{thing.sunSpecId(), model.modelId()}];
    model_ = model;
    if (model_.isDirty()) {
        //emit statsChanged(thing, model_);
    }
}

