#ifndef SENSOR_HPP
#define SENSOR_HPP

#include <mutex>
#include <atomic>
#include <chrono>
#include <string>
#include <thread>
#include <iostream>
#include <iomanip>
#include <condition_variable>


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

protected:
    int m_value = -1;
    Timestamp m_timestamp = std::chrono::system_clock::now();
};


class Sensor {
public:
    enum class Sort {
        blocking,
        nonBlocking
    };
    
    Sensor(const std::string& name, int intervalMS)
        : m_run(false), 
        // m_currentSample(Sample(-1)),
        m_intervalMS(intervalMS),
        m_name(name)
    {
    }

    virtual ~Sensor() {
        m_run = false;
        m_sensorThread.join();
    }

    // To be implemented by children
    virtual Sample sense() = 0;

    // Start generating integers with interval m_intervalMS starting at 0 and counting up.
    // if the sensor is already running, this function won't do anything
    virtual void start()
    {
        if (m_run) {
            return;
        }
        m_run = true;

        m_sensorThread = std::thread([this]() {
                int v = 0;
                while (m_run) {
                    std::this_thread::sleep_for(m_intervalMS);
                    {
                        std::lock_guard<std::mutex> lk(nv_mx_);
                        m_currentSample.store(Sample(v++));
                        new_value_.notify_one();
                    }
                }
            });
    }

    virtual void stop()
    {
        m_run = false;
    }

    virtual const std::string& getName() const
    {
        return m_name;
    }

    virtual const Sort getSort() const 
    {
        return m_sort;
    }

    virtual const bool running() const
    {
        return m_run.load();
    }

    virtual const std::chrono::duration<int, std::milli> interval() const
    {
        return m_intervalMS;
    }


protected:
    std::thread m_sensorThread;
    std::atomic<Sample> m_currentSample{Sample(-1)};
    std::atomic_bool m_run;
    std::chrono::duration<int, std::milli> m_intervalMS;
    std::string m_name;

    Sort m_sort;
    std::condition_variable new_value_;
    std::mutex nv_mx_;
};



// The NonBlockingSensor generates Integers starting at 0 and incrementing every intervalMS milliseconds.
// The sensor can be configured with a name and an interval.
// The sensor is non-blocking, i.e. all its functions and in particular sense() will return more or less immediately.
class NonBlockingSensor : public Sensor {
public:

    NonBlockingSensor(const std::string& name, int intervalMS)
        : Sensor(name, intervalMS)
    {
        m_sort = Sensor::Sort::nonBlocking;
    }

    // returns the most recent sensor sample.
    Sample sense() override 
    {
        return m_currentSample.load();
    }

};


// The BlockingSensor generates Integers starting at 0 and incrementing every intervalMS milliseconds.
// The sensor can be configured with a name and an interval.
// The sensor is blocking, i.e. when sense() is called it will wait for the next value.
class BlockingSensor : public Sensor {
public:

    BlockingSensor(const std::string& name, int intervalMS)
        : Sensor(name, intervalMS)
    {
        m_sort = Sensor::Sort::blocking;
    }

    // If the sensor is running it waits for a new sample and returns it.
    // If the sensor is not runnign it will return the most recent sample.
    Sample sense() override
    {
        std::unique_lock<std::mutex> lk(nv_mx_);
        new_value_.wait(lk);
        return m_currentSample.load();
    }

};

#endif  // SENSOR_HPP