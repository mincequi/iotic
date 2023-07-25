#include "MainWidget.h"

#include <Wt/WApplication.h>
#include <Wt/WGroupBox.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WPanel.h>
#include <Wt/WServer.h>
#include <Wt/WStandardItemModel.h>
#include <Wt/WText.h>
#include <Wt/WVBoxLayout.h>

#include "BarsWidget.h"
#include "CardWidget.h"
#include "ChartWidget.h"
#include "theme/Colors.h"
#include "IconLabel.h"
#include "HistoryModel.h"
#include "../feed/FeedManager.h"

using namespace Wt;

MainWidget::MainWidget(const FeedManager& feedManager) {
    // Main Layout
    auto mainLayout = setLayout(std::make_unique<Wt::WVBoxLayout>());
    mainLayout->setContentsMargins(16, 20, 16, 20);

    // Live Layout
    auto liveLayout = mainLayout->addLayout(std::make_unique<Wt::WHBoxLayout>());
    liveLayout->setContentsMargins(0, 0, 0, 0);
    liveLayout->setSpacing(0); // Set 0 to attach chart to bullet graph

    // Add chart widget and model
    _historyModel = std::make_shared<HistoryModel>();
    _chartWidget = liveLayout->addWidget(std::make_unique<ChartWidget>(_historyModel), 1);
    _chartWidget->setMaxConsumption(5000);
    _chartWidget->setMaxPvPower(2000);
    // Add bars
    _barsWidget = liveLayout->addWidget(std::make_unique<BarsWidget>(), 0, Wt::AlignmentFlag::Right);
    _barsWidget->setMaxPowerDemand(5000);
    _barsWidget->setMaxPvPower(2000);
    // Add icons and readouts
    liveLayout->addSpacing(6);
    auto vBox = liveLayout->addLayout(std::make_unique<Wt::WVBoxLayout>());
    _pvLabel = vBox->addWidget(std::make_unique<IconLabel>("fa-sun-o"));
    _pvLabel->setPower(0);
    _pvLabel->setColor(Colors::green());
    _houseLabel = vBox->addWidget(std::make_unique<IconLabel>("fa-home"));
    _houseLabel->setPower(0);
    _houseLabel->setColor(Colors::yellow());
    _gridLabel = vBox->addWidget(std::make_unique<IconLabel>("fa-flash"));
    _gridLabel->setPower(0);
    _gridLabel->setColor(Colors::cyan());
    vBox->addStretch(1);

    // Add Wallboxes
    mainLayout->addSpacing(6);
    auto wb = mainLayout->addWidget(std::make_unique<CardWidget>());

    _subscriptions.push_back(feedManager.statsFeed().maxSummedInverterPower().subscribe([&](const auto val) {
        Wt::WServer::instance()->postAll([&, val]() {
            _barsWidget->setMaxPvPower(val);
            _chartWidget->setMaxPvPower(val);
            _barsWidget->update();
            wApp->triggerUpdate();
        });
    }));
    _subscriptions.push_back(feedManager.statsFeed().maxSitePower().subscribe([&](const auto val) {
        Wt::WServer::instance()->postAll([&, val]() {
            _barsWidget->setMaxPowerDemand(val);
            _chartWidget->setMaxConsumption(val);
            _barsWidget->update();
            wApp->triggerUpdate();
        });
    }));
    _subscriptions.push_back(feedManager.statsFeed().maxPower().subscribe([&](const auto val) {
        Wt::WServer::instance()->postAll([&, val]() {
            _chartWidget->setMaxValue(val);
            _chartWidget->setMaxValue(val);
            wApp->triggerUpdate();
        });
    }));

    _subscriptions.push_back(feedManager.liveFeed().combinedPowers().subscribe([&](const auto val) {
        Wt::WServer::instance()->postAll([&, val]() {
            _pvLabel->setPower(std::get<0>(val));
            _houseLabel->setPower(abs(std::get<0>(val) - std::get<1>(val)));
            _gridLabel->setPower(abs(std::get<1>(val)));

            _barsWidget->setPvPower(std::get<0>(val));
            _barsWidget->setConsumption(std::get<0>(val) - std::get<1>(val));
            _barsWidget->setExportPower(std::get<1>(val));
            _barsWidget->update();

            _chartWidget->setPvPower(std::get<0>(val));
            _chartWidget->setConsumption(std::get<0>(val) - std::get<1>(val));
            _chartWidget->setExportPower(std::get<1>(val));

            const auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
            _historyModel->addValue(now,
                                    std::get<0>(val),
                                    std::get<1>(val),
                                    std::get<0>(val) - std::get<1>(val));
            _chartWidget->setTimespan(now - 60, now);
            wApp->triggerUpdate();
        });
    }));
}

MainWidget::~MainWidget() {
    for (auto& s : _subscriptions) {
        s.unsubscribe();
    }
    _subscriptions.clear();
}
