#include <things/ThingsDiscovery.h>

class SunSpecFactory : public ThingsDiscovery {
public:
    SunSpecFactory();

    void start(int msec) override;
    void stop() override;
};
