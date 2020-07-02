#include <fstream>

#include "include/sensor.hpp"
#include "include/configparser.hpp"
#include "include/sensorhub.hpp"

int main() {

    // Save program start time
    using namespace std::chrono_literals;
    Timestamp program_start = std::chrono::system_clock::now();

    // Set precision for floating point numbers on screen
    constexpr int precision = 4;
    constexpr int width = precision + 4;
    std::cout << std::fixed << std::setprecision(precision);

    // Set interval for continous logging
    std::chrono::duration<int, std::milli> interval;
    interval = std::chrono::duration<int, std::milli>(100);

    // Define config file and parse it
    std::string filePath = "/home/xif/dev/deepup/yaml/sensor_config.yml";
    ConfigParser parser(filePath);

    // Create logger and empty sensor hub
    std::shared_ptr<SamplesLogger> logger = std::make_shared<SamplesLogger>(program_start);
    SensorHub hub(logger);

    // Fill sensor hub with sensors
    for (SensorConfig cfg : parser.sensors()) {
        hub.createSensor(cfg);
    }

    // Start all sensors
    hub.start();

    // Define log file and open it
    std::ofstream logfile_stream;
    logfile_stream.open("sensor_log.txt");

    // Run 100*interval ms long
    for (int i = 100; i > 0; i--) {
        std::this_thread::sleep_for(interval);
        logger->duo( logfile_stream );
    }

    // Stop all sensors 
    std::string msg = "Waiting for sensors to stop...\n";
    logger->screen(msg);
    hub.stop();

    // Close the log file
    logfile_stream.close();
    
    return 0;
}