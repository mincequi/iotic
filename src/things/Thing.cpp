#include "Thing.h"

Thing::Thing() {
}

const std::string& Thing::id() const {

}

void Thing::setProperty(WriteableThingProperty property, double value) {
    doSetProperty(property, value);
}

grouped_observable<ReadableThingProperty, double> Thing::properties() const {
    return doProperties();
}

void Thing::doSetProperty(WriteableThingProperty property, double value) {
}

grouped_observable<ReadableThingProperty, double> Thing::doProperties() const {
    return {};
}
