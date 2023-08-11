#include "Statistics.h"

#include <things/sunspec/SunSpecBlock.h>
#include <things/sunspec/SunSpecThing.h>

Statistics::Statistics(QObject *parent) : QObject(parent) {
}

void Statistics::reset() {
    m_models.clear();
}

void Statistics::feedModel(const sunspec::SunSpecThing& thing, const sunspec::Model& model) {
    auto& _model = m_models[{thing.sunSpecId(), model.modelId()}];
    _model = model;
    if (_model.isDirty()) {
        emit statsChanged(thing, _model);
    }
}

