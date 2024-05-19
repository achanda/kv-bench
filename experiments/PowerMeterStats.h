#ifndef POWER_METER_STATS_H
#define POWER_METER_STATS_H

#include "rocksdb/statistics.h"
#include "PowerMeter.h"
#include <atomic>
#include <sstream>

class PowerMeterStats : public rocksdb::Statistics {
private:
    PowerMeter powerMeter;
    std::atomic<uint64_t> power_used_pkg;
    std::atomic<uint64_t> power_used_core;
    std::atomic<uint64_t> power_used_ram;

public:
    PowerMeterStats() : power_used_pkg(0), power_used_core(0), power_used_ram(0) {
        powerMeter.startMeasurement();
    }

    ~PowerMeterStats() {
        powerMeter.stopMeasurement();
        EnergyUsage usage = powerMeter.getEnergyUsage();
        power_used_pkg.store(usage.pkg);
        power_used_core.store(usage.core);
        power_used_ram.store(usage.ram);
    }

    uint64_t getTickerCount(uint32_t tickerType) const override {
        switch (tickerType) {
            case rocksdb::Tickers::POWER_USED_PKG:
                return power_used_pkg.load();
            case rocksdb::Tickers::POWER_USED_CORE:
                return power_used_core.load();
            case rocksdb::Tickers::POWER_USED_RAM:
                return power_used_ram.load();
            default:
                return Statistics::getTickerCount(tickerType);
        }
    }

    void recordTick(uint32_t tickerType, uint64_t count) override {
        // Optionally implement if needed
    }

    uint64_t getAndResetTickerCount(uint32_t tickerType) override {
        // Optionally implement if needed
        return 0;
    }

    void histogramData(uint32_t type, HistogramData* data) const override {
        // Optionally implement if needed
    }

    std::string getHistogramString(uint32_t type) const override {
        return ""; // Optionally implement if needed
    }

    bool HistEnabledForType(uint32_t type) const override {
        return true; // Optionally implement if needed
    }

    std::string ToString() const override {
        std::ostringstream ss;
        ss << "Power Usage (Package): " << power_used_pkg.load() << " Joules\n";
        ss << "Power Usage (Core): " << power_used_core.load() << " Joules\n";
        ss << "Power Usage (RAM): " << power_used_ram.load() << " Joules";
        return ss.str();
    }
};

#endif // POWER_METER_STATS_H