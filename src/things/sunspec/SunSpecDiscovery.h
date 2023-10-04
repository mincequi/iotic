#include <things/ThingsDiscovery.h>

#include <QObject>

class SunSpecDiscovery : public QObject, public ThingsDiscovery {
    Q_OBJECT
public:
    explicit SunSpecDiscovery(QObject *parent = nullptr);

    void start(int msec) override;
    void stop() override;

signals:
};
