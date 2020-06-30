
#include <atomic>
#include <chrono>
#include <string>
#include <thread>
#include <iostream>
#include <iomanip>

#include "include/configparser.hpp"

using Timestamp = std::chrono::system_clock::time_point;

// A sensor sample consisting of a value (int) and a timestamp when it was "sensed" (rather when it was created)
class Sample {
public:

    explicit Sample(int value) 
        : m_value(value) 
    {
    }

    int getValue() const 
    {
        return m_value;
    }

    Timestamp getTimestamp() const
    {
        return m_timestamp;
    }

    int m_value = -1;
    Timestamp m_timestamp = std::chrono::system_clock::now();
};

// The NonBlockingSensor generates Integers starting at 0 and incrementing every intervalMS milliseconds.
// The sensor can be configured with a name and an interval.
// The sensor is non-blocking, i.e. all its functions and in particular sense() will return more or less immediately.
class NonBlockingSensor {
public:

    NonBlockingSensor(const std::string& name, int intervalMS)
        : m_run(false), 
        m_currentSample(Sample(-1)),
        m_intervalMS(intervalMS),
        m_name(name)
    {
    }

    ~NonBlockingSensor() {
        m_run = false;
        m_sensorThread.join();
    }

    // Start generating integers with interval m_intervalMS starting at 0 and counting up.
    // if the sensor is already running, this function won't do anything
    void start()
    {
        if (m_run) {
            return;
        }
        m_run = true;

        m_sensorThread = std::thread([this]() {
            int v = 0;
            while (m_run) {
                std::this_thread::sleep_for(m_intervalMS);
                m_currentSample.store(Sample(v++));
            }
            });
    }

    void stop()
    {
        m_run = false;
    }

    // returns the most recent sensor sample.
    Sample sense() const {
        return m_currentSample.load();
    }

    const std::string& getName() const
    {
        return m_name;
    }

private:
    std::thread m_sensorThread;
    std::atomic<Sample> m_currentSample;
    std::atomic_bool m_run;
    std::chrono::duration<int, std::milli> m_intervalMS;
    std::string m_name;
};


// The BlockingSensor generates Integers starting at 0 and incrementing every intervalMS milliseconds.
// The sensor can be configured with a name and an interval.
// The sensor is blocking, i.e. when sense() is called it will wait for the next value.
class BlockingSensor {
public:

    BlockingSensor(const std::string& name, int intervalMS)
        : m_run(false),
        m_currentSample(Sample(-1)),
        m_intervalMS(intervalMS),
        m_name(name)
    {
    }

    ~BlockingSensor() {
        m_run = false;
        m_sensorThread.join();
    }

    // Start generating integers with interval m_intervalMS starting at 0 and counting up.
    // if the sensor is already running, this function won't do anything
    void start()
    {
        if (m_run) {
            return;
        }
        m_run = true;

        m_sensorThread = std::thread([this]() {
            int v = 0;
            while (m_run) {
                std::this_thread::sleep_for(m_intervalMS);
                m_currentSample.store(Sample(v++));
            }
            });
    }

    void stop()
    {
        m_run = false;
    }

    // If the sensor is running it waits for a new sample and returns it.
    // If the sensor is not runnign it will return the most recent sample.
    Sample sense() const 
    {
        Timestamp lastTS = m_currentSample.load().getTimestamp();

        while (m_run && lastTS == m_currentSample.load().getTimestamp()) {
            std::this_thread::sleep_for(std::chrono::duration<int, std::micro>(5));
        }

        return m_currentSample.load();
    }

    const std::string& getName() const
    {
        return m_name;
    }

private:
    std::thread m_sensorThread;
    std::atomic<Sample> m_currentSample;
    std::atomic_bool m_run;
    std::chrono::duration<int, std::milli> m_intervalMS;
    std::string m_name;
};

int main() {

    using namespace std::chrono_literals;
    Timestamp program_start = std::chrono::system_clock::now();

    constexpr int precision = 4;
    constexpr int width = precision + 4;
    std::cout << std::fixed << std::setprecision(precision);
    
/***                       ***/
    std::string filePath = "/home/xif/dev/deepup/yaml/sensor_config.yml";
    ConfigParser conf(filePath);




/***                       ***/

    NonBlockingSensor nb("nb 1", 1000);
    BlockingSensor b(" b 1", 3500);
    std::this_thread::sleep_for(1s);

    nb.start();
    b.start();
    std::this_thread::sleep_for(1010ms);
    
    auto s = nb.sense();
    std::chrono::duration<double> elapsed_seconds = s.getTimestamp() - program_start;
    std::cout << "[" << std::setw(width) << elapsed_seconds.count() << "s] '" << nb.getName() << "': " << s.getValue() << std::endl;

    s = b.sense();
    elapsed_seconds = s.getTimestamp() - program_start;
    std::cout << "[" << std::setw(width) << elapsed_seconds.count() << "s] '" << b.getName() << "': " << s.getValue() << std::endl;

    b.stop();
    nb.stop();

}