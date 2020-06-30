#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include <map>
#include <vector>
#include <string>

#include <yaml-cpp/yaml.h>

// Description of one sensor
class SensorConfig {
public:

    enum class Sort {
        blocking,
        nonBlocking
    };
    
    // Setter
    virtual void name(std::string nm) {
        name_ = nm;
    }
    
    virtual void interval(uint32_t itv) {
        interval_ = itv;
    }
    
    virtual void type(std::string tp) {
        if (tp == "Blocking") {
            type_ = Sort::blocking;
        }
        else if (tp == "NonBlocking") {
            type_ = Sort::nonBlocking;
        }
        else {
            std::string msg = "Unknown type for sensor: " + tp;
            throw std::runtime_error(msg);
        }
    }

    virtual void other(std::string key, std::string value) {
        other_.insert(std::pair<std::string, std::string>(key, value));
    }

    // Getter
    virtual std::string name() const {
        return name_;
    }

    virtual uint32_t interval() const {
        return interval_;
    }

    virtual Sort type() const {
        return type_;
    }

    virtual std::string type_str() const {
        switch (type_)
        {
        case Sort::blocking:
            return "Blocking";
        case Sort::nonBlocking:
            return "NonBlocking";
        default:
            break;
        }
        return "ERROR_NO_TYPE";
    }

    virtual std::map<std::string, std::string> other() const {
        return other_;
    }


    // Make sure sensitive values are valid
    virtual bool valid() {
        if (name_.length() > 0 && 
            interval_ > 0 && 
            ((type_ == Sort::blocking) || (type_ == Sort::nonBlocking))
        ) {
            return true;
        }
        return false;
    }

protected:
    std::string name_;
    uint32_t interval_ = 0;
    Sort type_;
    std::map<std::string, std::string> other_;

};

// This class parses a "sensor config" file and 
// provides the extracted information
class ConfigParser {
public:
    ConfigParser(std::string configPath) {
        root = YAML::LoadFile(configPath);
        duration_ = static_cast<uint32_t>(root["Duration (s)"].as<int>());
        initSensors(root["Sensors"]);
    }

    virtual std::vector<SensorConfig> sensors() const {
        return sensorConfigs;
    }

    virtual uint32_t duration() {
        return duration_;
    }

protected:
    YAML::Node root;
    std::vector<SensorConfig> sensorConfigs;
    uint32_t duration_;

    virtual void initSensors(YAML::Node sensorsNode) {
        // Loop sensors array
        for(size_t i = 0; i < sensorsNode.size(); i++) {
            SensorConfig sensor;
            
            // Loop key-value pairs
            for(YAML::const_iterator kv = sensorsNode[i].begin(); kv != sensorsNode[i].end(); ++kv) {
                std::string key = kv->first.as<std::string>();

                if (key == "Name") {
                    sensor.name( kv->second.as<std::string>() );
                }
                else if (key == "Interval (ms)") {
                    sensor.interval( static_cast<uint32_t>(kv->second.as<int>()) );
                }
                else if (key == "Sensor") {
                    sensor.type( kv->second.as<std::string>() );
                }
                else {
                    sensor.other( key, kv->second.as<std::string>() );
                }
            }

            if (sensor.valid()) {
                sensorConfigs.push_back(sensor);
            }
            else {
                std::cerr << "ERROR: Bad values for sensor number " << i << " (counting from 0)" << std::endl;
                std::cerr << "Scipping sensor " << i << std::endl;
            }
        }
    }
};


#endif  // CONFIGPARSER_HPP