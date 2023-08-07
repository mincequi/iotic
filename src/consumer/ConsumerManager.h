#pragma once

class ConsumerManager {
public:
    ConsumerManager(int availableAmperage = 40);

    /// Set maximum amperage for entire site
    void setMaximumAmperage(int amps);

    /// Set PV surplus in amperage. This should be already filtered for spikes, etc.
    void setPvSurplusAmperage(int amps);
};
