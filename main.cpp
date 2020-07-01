
#include "include/sensor.hpp"
#include "include/configparser.hpp"
#include "include/sensorhub.hpp"

int main() {

    using namespace std::chrono_literals;
    Timestamp program_start = std::chrono::system_clock::now();

    constexpr int precision = 4;
    constexpr int width = precision + 4;
    std::cout << std::fixed << std::setprecision(precision);

    std::chrono::duration<int, std::milli> interval;
    interval = std::chrono::duration<int, std::milli>(100);

/***                                                                       ***/
    std::string filePath = "/home/xif/dev/deepup/yaml/sensor_config.yml";
    ConfigParser parser(filePath);

    std::shared_ptr<SamplesLogger> logger = std::make_shared<SamplesLogger>(program_start);
    SensorHub hub(logger);

    for (SensorConfig cfg : parser.sensors()) {
        hub.createSensor(cfg);
    }

    for (std::shared_ptr<Sensor> snr : hub.sensors()) {
        std::cout << snr->getName() << std::endl;
    }
        std::cout << std::endl;


    hub.start();
    
    for (int i = 10000; i > 0; i--) {
        std::this_thread::sleep_for(interval);
        logger->log();
        // std::cout << t1.sense_blocking() << std::endl;
    }

    hub.stop();
/***                                                                        ***/

    // NonBlockingSensor nb("nb 1", 1000);
    // BlockingSensor b(" b 1", 3500);
    // std::this_thread::sleep_for(1s);

    // nb.start();
    // b.start();
    // std::this_thread::sleep_for(1010ms);
    
    // auto s = nb.sense();
    // std::chrono::duration<double> elapsed_seconds = s.getTimestamp() - program_start;
    // std::cout << "[" << std::setw(width) << elapsed_seconds.count() << "s] '" << nb.getName() << "': " << s.getValue() << std::endl;

    // s = b.sense();
    // elapsed_seconds = s.getTimestamp() - program_start;
    // std::cout << "[" << std::setw(width) << elapsed_seconds.count() << "s] '" << b.getName() << "': " << s.getValue() << std::endl;

    // b.stop();
    // nb.stop();

    return 0;
}