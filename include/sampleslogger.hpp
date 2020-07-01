#include <iostream>
#include <mutex>
#include <vector>

#include "./sensor.hpp"

using namespace std::chrono_literals;

class SamplesLogger {
protected:
    std::mutex mx_;
    std::map<std::string, Sample> samples_;
    Timestamp program_start_;
public:
    SamplesLogger(Timestamp program_start) :
        program_start_(program_start) {
    }

    virtual void log() 
    {
        std::map<std::string, Sample> samples;
        unqueue(samples);
        for (auto const& [producer, spl] : samples) {
            std::chrono::duration<double> elapsed_seconds = spl.getTimestamp() - program_start_;
            std::cout << "[" << std::setw(4) << elapsed_seconds.count() << "s] '" << producer << "': " << spl.getValue() << std::endl;
        }
    }

    virtual void queue(std::string producer, Sample spl) 
    {
        const std::lock_guard<std::mutex> lock(mx_);
        samples_.insert({producer, spl});
    }

    virtual void unqueue(std::map<std::string, Sample>& samples_copy) 
    {
        const std::lock_guard<std::mutex> lock(mx_);
        samples_copy = samples_;
        samples_.clear();
    }

};