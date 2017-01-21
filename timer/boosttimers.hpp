/**
 *  Project: The Stock Libraries
 *
 *  File: boosttimers.hpp
 *  Created: Nov 21, 2012
 *
 *  Author: Abhinav Sarje <abhinav.sarje@gmail.com>
 *
 *  Copyright (c) 2012-2017 Abhinav Sarje
 *  Distributed under the Boost Software License.
 *  See accompanying LICENSE file.
 */

#include "timers.hpp"
#include <boost/timer/timer.hpp>

#ifndef __BOOSTTIMERS_HPP__
#define __BOOSTTIMERS_HPP__

namespace stock {

class BoostTimer : public Timer {
	// use Boost's chrono
	// lowest resolution is nsec!!! not really!!
	private:
		boost::timer::cpu_timer timer_;

	public:
		BoostTimer() : timer_() { start_ = 0.0; stop_ = 0.0; elapsed_ = 0.0; is_running_ = false; }
		~BoostTimer() { }

		void reset() { start_ = 0.0; stop_ = 0.0; elapsed_ = 0.0; is_running_ = false; }

		void start() {
			reset();
			boost::timer::cpu_times const elapsed_time(timer_.elapsed());
			boost::timer::nanosecond_type const elapsed(elapsed_time.system + elapsed_time.user);
			start_ = elapsed;
			is_running_ = true;
		} // start()

		void stop() {
			if(!is_running_) {
				std::cerr << "error: timer is not running" << std::endl;
				return;
			} // if
			boost::timer::cpu_times const elapsed_time(timer_.elapsed());
			boost::timer::nanosecond_type const elapsed(elapsed_time.system + elapsed_time.user);
			stop_ = elapsed;
			is_running_ = false;
			elapsed_ = stop_ - start_;
		} // stop()

		double lap() {
			if(!is_running_) {
				std::cerr << "error: timer is not running" << std::endl;
				return 0.0;
			} // if
			boost::timer::cpu_times const elapsed_time1(timer_.elapsed());
			boost::timer::nanosecond_type const elapsed1(elapsed_time1.system + elapsed_time1.user);
			double temp1 = elapsed1;
			double temp2 = temp1 - start_;
			elapsed_ += temp2;
			start_ = temp1;
			return temp2;
		} // lap()

		double elapsed_sec() { return elapsed_ / 1e9; }
		double elapsed_msec() { return elapsed_ / 1e6; }
		double elapsed_usec() { return elapsed_ / 1e3; }
		double elapsed_nsec() { return elapsed_; }

}; // class BoostTimer

} // namespace stock


#endif // __BOOSTTIMERS_HPP__
