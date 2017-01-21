/**
 *  Project: The Stock Libraries
 *
 *  File: timers.hpp
 *  Created: Nov 21, 2012
 *
 *  Author: Abhinav Sarje <abhinav.sarje@gmail.com>
 */

#ifndef _TIMERS_HPP_
#define _TIMERS_HPP_

namespace stock {

class Timer {		// an abstract class
	protected:
		double start_;
		double stop_;
		double elapsed_;	// in lowest resolution of respective timers used
		bool is_running_;
		bool is_paused_;

	public:
		virtual ~Timer() { }

		virtual void reset() = 0;

		virtual void start() = 0;
		virtual void stop() = 0;
		virtual double lap() = 0;			// lap in lowest resolution
		virtual void pause() = 0;
		virtual void resume() = 0;

		virtual double elapsed_sec() = 0;	// in seconds		10^0
		virtual double elapsed_msec() = 0;	// in miliseconds	10^3
		virtual double elapsed_usec() = 0;	// in microseconds	10^6
		virtual double elapsed_nsec() = 0;	// in nanoseconds	10^9
}; // class Timer

} // namespace stock


#endif // _TIMERS_HPP_
