#include <things/ThingsDiscovery.h>

class SunSpecDiscovery : public ThingsDiscovery {
public:
    SunSpecDiscovery();

    void start(int msec) override;
    void stop() override;
};
