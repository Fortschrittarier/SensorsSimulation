#include <fstream>

#include "include/sensor.hpp"
#include "include/configparser.hpp"
#include "include/sensorhub.hpp"

int main(int argc, char* argv[]) {

    // Save program start time
    using namespace std::chrono_literals;
    Timestamp program_start = std::chrono::system_clock::now();

    // Set interval for continous logging
    std::chrono::duration<int, std::milli> interval;
    interval = std::chrono::duration<int, std::milli>(100);

    // Check config file and parse it
    if( argc < 2 ) {
        std::cout << "Please provide a sensor conig file." << std::endl;
        return -1;
    }
    std::string filePath = argv[1];
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
    std::string logfile = "sensor_log.txt";
    std::ofstream logfile_stream;
    logfile_stream.open( logfile );

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

    msg = "Log file: " + logfile + "\n";
    logger->screen(msg);

    return 0;
}