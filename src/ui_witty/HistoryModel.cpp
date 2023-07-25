#include "HistoryModel.h"

#include <array>

using namespace Wt;

HistoryModel::HistoryModel() {
    // Demo model setup
    for (unsigned i = 0; i < 40; ++i) {
        double x = (static_cast<double>(i)) / 4;
        addValue(x, (1 + std::sin(x)) * 100, 0, (1 + std::cos(x)) * 125);
    }
}

void HistoryModel::setTimespan(std::time_t from, std::time_t to) {
}

void HistoryModel::addValue(std::time_t ts, int pvPower, int gridPower, int siteConsumption) {
    _data.push_back({ts, pvPower, abs(gridPower), siteConsumption});
}

void HistoryModel::setMaxValue(int value) {
    _maxValue = value;
    changed().emit();
}

double HistoryModel::maxValue() const {
    return _maxValue;
}

Wt::WString HistoryModel::headerData(int column) const {
    switch (column) {
    case 0: return "X";
    case 1: return "Y = sin(X)";
    case 2: return "Y = cos(X)";
    default: return "";
    }
}

double HistoryModel::data(int row, int column) const {
    if (column == 0) return std::get<0>(_data.at(row));
    if (column == 1) return std::get<1>(_data.at(row));
    if (column == 2) return std::get<2>(_data.at(row));
    if (column == 3) return std::get<3>(_data.at(row));

    return 0.0;
}

int HistoryModel::columnCount() const {
    return 4;
}

int HistoryModel::rowCount() const {
    return _data.size();
}
