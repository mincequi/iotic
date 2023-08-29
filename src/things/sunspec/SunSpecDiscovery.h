#include <things/ThingsDiscovery.h>

#include <QObject>

class SunSpecDiscovery : public QObject, public ThingsDiscovery {
    Q_OBJECT
public:
    explicit SunSpecDiscovery(QObject *parent = nullptr);

    void start(uint16_t seconds = 60) override;
    void stop() override;

signals:
};
