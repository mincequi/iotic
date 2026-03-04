#include "ArchiveDayUseCase.h"

#include "Database.h"
#include "DatabaseUtil.h"

using namespace std::chrono;

void ArchiveDayUseCase::operator()(std::string_view thingId,
                                   uvw_iot::ThingPropertyKey property,
                                   year_month_day day,
                                   minutes resolution) {
    // Check if data for the day is already complete (the day is already passed).
    const auto today = floor<days>(system_clock::now());
    if (sys_days(day) >= today) {
        return;
    }

    const auto rawData = _database.rawData(thingId, property, day);
    const int factor = seconds(resolution).count();
    const auto cborData = DatabaseUtil::cborEncode(DatabaseUtil::deltaCompress(DatabaseUtil::downsample(rawData, factor)));
    _database.putArchivedData(thingId,
                              property,
                              day,
                              resolution,
                              std::string_view{(char*)cborData.data(), cborData.size()});
}
