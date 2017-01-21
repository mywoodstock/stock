/**
 *  Project: The Stock Libraries
 *
 *  File: typedefs.hpp
 *  Created: Jul 08, 2012
 *
 *  Author: Abhinav Sarje <abhinav.sarje@gmail.com>
 *
 *  Copyright (c) 2012-2017 Abhinav Sarje
 *  Distributed under the Boost Software License.
 *  See accompanying LICENSE file.
 */

#ifndef __TYPEDEFS_HPP__
#define __TYPEDEFS_HPP__

#include <vector>
#include <complex>

namespace stock {

#ifdef DOUBLEP	// double precision
	typedef double						real_t;
#else			// single precision
	typedef float						real_t;
#endif

	typedef std::complex<real_t>		complex_t;

	typedef std::vector<real_t> 		real_vec_t;
	typedef std::vector<complex_t>		complex_vec_t;

} // namespace stock


#endif /* __TYPEDEFS_HPP__ */
