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
    SamplesLogger( Timestamp program_start ) :
        program_start_( program_start ) {
    }

    virtual void duo( std::ofstream& ostream ) 
    {
        std::string log = formatted();
        screen( log );
        file( ostream, log );
    }

    virtual void file( std::ofstream& ofs, std::string& s ) 
    {
        ofs << s;
    }

    virtual void screen( std::string& s ) 
    {
        std::cout << s;
    }

    virtual void screen() 
    {
        std::cout << formatted();
    }

    virtual std::string formatted() {
        std::stringstream srm;
        std::map<std::string, Sample> samples;

        srm  << std::setprecision(5);

        unqueue( samples );
        for( auto const& [producer, spl] : samples ) {
            std::chrono::duration<double> elapsed_seconds = spl.getTimestamp() - program_start_;
            srm << "[" << elapsed_seconds.count() << "s] '" << producer << "': " << spl.getValue() << std::endl;
        }
        return srm.str();
    }

    virtual void queue( std::string producer, Sample spl ) 
    {
        const std::lock_guard<std::mutex> lock( mx_ );
        samples_.insert( {producer, spl} );
    }

    virtual void unqueue( std::map<std::string, Sample>& samples_copy ) 
    {
        const std::lock_guard<std::mutex> lock( mx_ );
        samples_copy = samples_;
        samples_.clear();
    }

};