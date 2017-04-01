#ifndef ACF_HH
#define ACF_HH

#include <functional>
#include <unordered_map>
#include <string>
#include "types.hh"
#include "params.hh"
#include "grid.hh"
#include "validators.hh"

/// @file
/// Mini-database of ACF approximations.

namespace arma {

	template <class T>
	Array3D<T>
	standing_wave_ACF(const Vec3D<T>& delta, const Shape3D& acf_size) {

		// guessed
		T alpha = 0.06;
		T beta = 0.8;
		T gamm = 5.0;

		// from mathematica
//		T alpha = 2.31906, beta = -5.49873, gamm = 0.0680413;

		Array3D<T> acf(acf_size);
		blitz::firstIndex t;
		blitz::secondIndex x;
		blitz::thirdIndex y;
		acf = gamm * blitz::exp(-alpha * (2 * t * delta[0] + x * delta[1] +
		                                  y * delta[2])) *
		      blitz::cos(2 * beta * t * delta[0]) *
		      blitz::cos(beta * x * delta[1]) *
		      blitz::cos(0 * beta * y * delta[2]);
		return acf;
	}

	template <class T>
	Array3D<T>
	propagating_wave_ACF(const Vec3D<T>& delta, const Shape3D& acf_size) {

		// guessed
//		T alpha = 1.5;
//		T gamm = 1.0;

		// from mathematica
		T alpha = 0.42, beta = -1.8, gamm = 5.0;

		Array3D<T> acf(acf_size);
		blitz::firstIndex i;
		blitz::secondIndex j;
		blitz::thirdIndex k;

		acf = gamm * blitz::exp(-alpha *
		                        (i * delta[0] + j * delta[1] + k * delta[2])) *
		      blitz::cos(-beta * i * delta[0] + beta * j * delta[1] +
		                 0*beta * k * delta[2]);
		return acf;
	}

	/// Helper class to init ACF by name.
	template<class T>
	class ACF_wrapper {

		typedef std::function<Array3D<T>(const Vec3D<T>&, const Shape3D&)>
		    ACF_function;

		Array3D<T>& _acf;

		static ACF_function
		get_acf_function(std::string func) {
			auto result = acf_functions.find(func);
			if (result == acf_functions.end()) {
				std::clog << "Bad ACF function name: \"" << func << '\"' << std::endl;
				throw std::runtime_error("bad ACF function name");
			}
			return result->second;
		}

		/// Map of names to ACF functions.
		static const std::unordered_map<std::string, ACF_function>
		    acf_functions;

	public:

		explicit
		ACF_wrapper(Array3D<T>& acf):
		_acf(acf)
		{}

		friend std::istream&
		operator>>(std::istream& in, ACF_wrapper& rhs) {
			std::string func;
			Grid<T,3> grid;
			sys::parameter_map params({
			    {"grid", sys::make_param(grid, validate_grid<T,3>)},
			    {"func", sys::make_param(func)},
			}, true);
			in >> params;
			ACF_function acf_func = get_acf_function(func);
			rhs._acf.resize(grid.size());
			rhs._acf = acf_func(grid.delta(), grid.size());
			return in;
		}

	};

	template <class T>
	const std::unordered_map<
	    std::string, std::function<Array3D<T>(const Vec3D<T>&, const Shape3D&)>>
	    ACF_wrapper<T>::acf_functions = {
	        {"standing_wave", standing_wave_ACF<T>},
	        {"propagating_wave", propagating_wave_ACF<T>}};
}

#endif // ACF_HH
