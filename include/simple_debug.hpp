#ifndef SIMPLE_DEBUG_HPP
#define SIMPLE_DEBUG_HPP

#if SIMPLE_DEBUG
#define DEBUG_MSG(msg) std::cout << msg << std::endl;

#else
#define DEBUG_MSG(msg)

#endif  //SIMPLE_DEBUG
#endif  // SIMPLE_DEBUG_HPP