#ifndef SENSORHUB_HPP
#define SENSORHUB_HPP

#include <memory>

#include "./configparser.hpp"
#include "./sensor.hpp"
#include "./sampleslogger.hpp"


#define SIMPLE_DEBUG 1
#include "./simple_debug.hpp"

class SensorHub {
public:
    SensorHub(std::shared_ptr<SamplesLogger> l) {
        logger = l ;
    }

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
        case Sensor::Sort::blocking:
            snr = std::make_shared<BlockingSensor>(cfg.name(), cfg.interval());
            break;

        case Sensor::Sort::nonBlocking:
            snr = std::make_shared<NonBlockingSensor>(cfg.name(), cfg.interval());
            break;
        default:
            throw std::runtime_error("ERROR: Bad sensor configuration");
            break;
        }
        m_sensors.push_back(snr);
    }

    virtual void start() 
    {
        for( auto snr : m_sensors ) {
            std::shared_ptr<std::thread> thr;
            snr->start();

            switch (snr->getSort())
            {
            case Sensor::Sort::blocking:
                 thr = std::make_shared<std::thread>([this, snr]() {
                     while( snr->running() ) {
                        // DEBUG_MSG(snr->getName())
                        logger->queue( snr->getName(), snr->sense() );
                        // std::this_thread::sleep_for( std::chrono::duration<int, std::milli>(100) ); ///// !
                     }
                });
                break;

            case Sensor::Sort::nonBlocking:
                thr = std::make_shared<std::thread>([this, snr]() {
                    std::chrono::system_clock::time_point last =
                        std::chrono::system_clock::from_time_t(0);

                    while( snr->running() ) {
                        // DEBUG_MSG(snr->getName())
                        Sample s = snr->sense();
                        if( s.getTimestamp() != last ) {
                            last = s.getTimestamp();
                            logger->queue( snr->getName(), s );
                        }
                        std::this_thread::sleep_for( snr->interval()/2 );
                        // std::this_thread::sleep_for( std::chrono::duration<int, std::milli>(100) ); ///// !
                     }

                });
                break;
            
            default:
                break;
            }
            m_sensor_thread_map.insert({snr, thr});
        }
    }

    virtual void stop() {
        for( auto const& st_pair : m_sensor_thread_map ) {
            st_pair.first->stop();   // Sensor ptr
            st_pair.second->join();   // According thread
        }
    }

protected:
    std::vector<std::shared_ptr<Sensor>> m_sensors;
    std::map<std::shared_ptr<Sensor>, std::shared_ptr<std::thread>> m_sensor_thread_map;
    std::shared_ptr<SamplesLogger> logger;

};


#endif  // SENSORHUB_HPP