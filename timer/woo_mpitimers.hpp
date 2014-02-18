/***
  *  Project: WOO Timer Library
  *
  *  File: woo_mpitimers.hpp
  *  Created: Nov 21, 2012
  *  Modified: Tue 18 Feb 2014 10:54:41 AM PST
  *
  *  Author: Abhinav Sarje <asarje@lbl.gov>
  */

#include "wootimers.hpp"
#include <mpi.h>

#ifndef _WOOMPITIMERS_HPP_
#define _WOOMPITIMERS_HPP_

namespace woo {

class MPITimer : public WooTimer {
	// use MPI_Wtime
	// lowest resolution is sec
	private:

	public:
		MPITimer() { start_ = 0.0; stop_ = 0.0; elapsed_ = 0.0; is_running_ = false; is_paused_ = false; }
		~MPITimer() { }

		void reset() { start_ = 0.0; stop_ = 0.0; elapsed_ = 0.0; is_running_ = false; is_paused_ = false; }

		void start() {
			reset();
			start_ = MPI_Wtime();
			is_running_ = true;
		} // start()

		void stop() {
			if(!is_running_) {
				std::cerr << "error: timer has not yet started" << std::endl;
				return;
			} // if
			if(!is_paused_) stop_ = MPI_Wtime();
			is_running_ = false;
			elapsed_ = stop_ - start_;
		} // stop()

		double lap() {
			if(!is_running_) {
				std::cerr << "error: timer has not yet started" << std::endl;
				return 0.0;
			} // if
			double temp1 = MPI_Wtime();
			double temp2 = temp1 - start_;
			elapsed_ += temp2;
			start_ = temp1;
			return temp2;
		} // lap()

		void pause() {
			if(!is_running_) {
				std::cerr << "error: timer is not running" << std::endl;
				return;
			} // if
			if(is_paused_) {
				std::cerr << "warning: timer is already paused. ignoring" << std::endl;
				return;
			} // if
			stop_ = MPI_Wtime();
			is_paused_ = true;
		} // pause()

		void resume() {
			if(!is_running_) {
				std::cerr << "error: timer is not running" << std::endl;
				return;
			} // if
			if(!is_paused_) {
				std::cerr << "warning: timer is not paused. ignoring" << std::endl;
				return;
			} // if
			double temp1 = MPI_Wtime();
			double temp2 = temp1 - stop_;
			start_ += temp2;
			is_paused_ = false;
		} // resume()

		double elapsed_sec() { return elapsed_; }
		double elapsed_msec() { return elapsed_ * 1e3; }
		double elapsed_usec() { return elapsed_ * 1e6; }
		double elapsed_nsec() { return elapsed_ * 1e9; }
}; // class MPITimer

} // namespace woo


#endif // _WOOMPITIMERS_HPP_
