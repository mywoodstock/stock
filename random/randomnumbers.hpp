/**
 *  Project: The Stock Libraries
 *
 *  File: randomnumbers.hpp
 *  Created: Aug 25, 2013
 *
 *  Author: Abhinav Sarje <abhinav.sarje@gmail.com>
 *
 *  Copyright (c) 2012-2017 Abhinav Sarje
 *  Distributed under the Boost Software License.
 *  See accompanying LICENSE file.
 */

#ifndef __RANDOM_NUMBERS__
#define __RANDOM_NUMBERS__

namespace stock {

	class RandomNumberGenerator {	// an abstract class
		protected:
			double min_;	// the minimum value for a given random number generator
			double max_;	// the maximum value for a given random number generator

			double last_;	// stores the last generated random number

		public:
			virtual ~RandomNumberGenerator() { }

			virtual void reset() = 0;				// reset the random number generator
			virtual void reset(unsigned int) = 0;	// reset the random number generator with seed

			//virtual double min() = 0;					// return the min number
			//virtual double max() = 0;					// return the max number

			virtual double rand() = 0;				// returns a random number in [0,1]
			virtual double rand_last() = 0;			// returns the last generated random number
	}; // class RandomNumberGenerator

} // namespace stock

#endif // __RANDOM_NUMBERS__
