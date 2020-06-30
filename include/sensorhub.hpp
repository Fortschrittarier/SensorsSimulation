#ifndef SENSORHUB_HPP
#define SENSORHUB_HPP

#include <memory>

#include "./configparser.hpp"
#include "./sensor.hpp"

class SensorHub {
public:
    virtual std::vector<std::shared_ptr<Sensor>>& sensors() {
        return m_sensors;
    }

    virtual void createSensor(SensorConfig cfg) 
    {
        if (!cfg.valid()) {
            throw std::runtime_error("ERROR: Bad sensor configuration");
        }

        std::shared_ptr<Sensor> snr;
        switch (cfg.type())
        {
        case SensorConfig::Sort::blocking:
            snr = std::make_shared<BlockingSensor>(cfg.name(), cfg.interval());
            break;

        case SensorConfig::Sort::nonBlocking:
            snr = std::make_shared<NonBlockingSensor>(cfg.name(), cfg.interval());
            break;
        default:
            throw std::runtime_error("ERROR: Bad sensor configuration");
            break;
        }
        m_sensors.push_back(snr);
    }

protected:
    std::vector<std::shared_ptr<Sensor>> m_sensors;
};


#endif  // SENSORHUB_HPP