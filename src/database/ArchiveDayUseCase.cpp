#include "ArchiveDayUseCase.h"

#include "Database.h"
#include "DatabaseUtil.h"

void ArchiveDayUseCase::operator()(std::string_view thingId, uvw_iot::ThingPropertyKey property, const std::chrono::year_month_day& day) {
    // Check if data for the day is already complete (the day is already passed).
    const auto today = std::chrono::floor<std::chrono::days>(std::chrono::system_clock::now());
    if (std::chrono::sys_days(day) >= today) {
        return;
    }

    const auto rawData = _database.rawData(thingId, property, day);
    const auto cborData = DatabaseUtil::cborEncode(DatabaseUtil::deltaCompress(DatabaseUtil::downsample(rawData, 60)));
    _database.putArchivedData(thingId, property, day, std::string_view{(char*)cborData.data(), cborData.size()});
}
