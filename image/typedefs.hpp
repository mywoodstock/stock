/**
 *  Project: The Stock Libraries
 *
 *  File: typedefs.hpp
 *  Created: Jul 08, 2012
 *
 *  Author: Abhinav Sarje <abhinav.sarje@gmail.com>
 */

#ifndef _TYPEDEFS_HPP_
#define _TYPEDEFS_HPP_

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


#endif /* _TYPEDEFS_HPP_ */
