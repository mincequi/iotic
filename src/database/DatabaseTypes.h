#pragma once

#include <cstdint>
#include <map>
#include <utility>
#include <vector>

using Datapoint = std::vector<int16_t>;
using Dataset = std::map<std::uint32_t, Datapoint>;
using DeltaDataset = std::vector<std::pair<std::uint32_t, Datapoint>>;
