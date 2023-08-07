#pragma once

#include <Wt/WContainerWidget.h>

#include <rpp/subscriptions/subscription_base.hpp>

class BarsWidget;
class ChartWidget;
class IconLabel;
class FeedManager;
class HistoryModel;

class MainWidget : public Wt::WContainerWidget {
public:
    MainWidget(const FeedManager& feedManager);
    ~MainWidget();

private:
    Wt::WStackedWidget* _mainStack;
    std::shared_ptr<HistoryModel> _historyModel;
    ChartWidget* _chartWidget;
    BarsWidget* _barsWidget;
    IconLabel* _pvLabel;
    IconLabel* _houseLabel;
    IconLabel* _gridLabel;

    std::list<rpp::subscription_base> _subscriptions;
};
