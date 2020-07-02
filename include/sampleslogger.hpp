#include <iostream>
#include <deque>
#include <mutex>
#include <vector>
#include <algorithm>

#include "./sensor.hpp"

using namespace std::chrono_literals;

class SamplesLogger {
protected:
    std::mutex mx_;
    std::deque<std::pair<std::string, Sample>> samples_;
    Timestamp program_start_;

    static bool sort_by_timestamp(
            std::pair<std::string, Sample> & lval, 
            std::pair<std::string, Sample> & rval) 
    {
        return (lval.second.getTimestamp() < rval.second.getTimestamp());
    }

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
        std::cout << s << std::flush;
    }

    virtual void screen() 
    {
        std::cout << formatted();
    }

    virtual std::string formatted() {
        std::stringstream srm;
        std::deque<std::pair<std::string, Sample>> samples;
        std::chrono::duration<double> elapsed_seconds;

        srm  << std::fixed << std::setprecision(4);

        unqueue( samples );
        std::sort( samples.begin(), samples.end(), sort_by_timestamp );
        
        for( auto const& [producer, spl] : samples ) {
            elapsed_seconds = spl.getTimestamp() - program_start_;
            srm << "[" << elapsed_seconds.count() << "s] '" << producer << "': " << spl.getValue() << std::endl;
        }
        return srm.str();
    }

    virtual void queue( std::string producer, Sample spl ) 
    {
        const std::lock_guard<std::mutex> lock( mx_ );
        samples_.push_back( {producer, spl} );
    }

    virtual void unqueue( std::deque<std::pair<std::string, Sample>>& samples_copy ) 
    {
        const std::lock_guard<std::mutex> lock( mx_ );
        samples_copy = samples_;
        samples_.clear();
    }

};