#ifndef DISTRIBUTION_HH
#define DISTRIBUTION_HH

#include <algorithm>

#include <blitz/array.h>
#include <gsl/gsl_cdf.h>

#include "statistics.hh"

namespace arma {

	namespace stats {

		/// \brief Normal distribution.
		template <class T>
		struct Gaussian {

			Gaussian(T m, T sigma):
			_mean(m),
			_sigma(sigma)
			{}

			T
			quantile(T f) {
				return gsl_cdf_gaussian_Pinv(f, _sigma) + _mean;
			}

		private:
			T _mean;
			T _sigma;
		};

		/// \brief Weibull distribution.
		template <class T>
		struct Weibull {

			Weibull(T a, T b):
			_a(a),
			_b(b)
			{}

			T
			quantile(T f) {
				return gsl_cdf_weibull_Pinv(f, _a, _b);
			}

		private:
			T _a; //< lambda
			T _b; //< k
		};

		enum struct Characteristic {
			Wave_height,
			Wave_length,
			Crest_length,
			Wave_period,
			Wave_slope,
			Threedimensionality
		};

		/// \brief Determines Weibull distribution shape parameter
		/// for different wave characteristics.
		template <Characteristic c, class T>
		struct Weibull_shape {};

		template <class T>
		struct Weibull_shape<Characteristic::Wave_height, T> {
			constexpr static const T shape = 2.0;
		};

		template <class T>
		struct Weibull_shape<Characteristic::Wave_length, T> {
			constexpr static const T shape = 2.3;
		};

		template <class T>
		struct Weibull_shape<Characteristic::Crest_length, T> {
			constexpr static const T shape = 2.3;
		};

		template <class T>
		struct Weibull_shape<Characteristic::Wave_period, T> {
			constexpr static const T shape = 3.0;
		};

		template <class T>
		struct Weibull_shape<Characteristic::Wave_slope, T> {
			constexpr static const T shape = 2.5;
		};

		template <class T>
		struct Weibull_shape<Characteristic::Threedimensionality, T> {
			constexpr static const T shape = 2.5;
		};

		/// \brief Distribution function of wave height, period, length etc.
		template <class T, Characteristic C>
		struct Wave_characteristic_distribution : public Weibull<T> {
			Wave_characteristic_distribution(T mean)
			    : Weibull<T>(mean / std::tgamma(T(1) + T(1) / shape), shape) {}
			constexpr static const T shape = Weibull_shape<C, T>::shape;
		};

		template <class T>
		using Wave_heights_dist =
		    Wave_characteristic_distribution<T, Characteristic::Wave_height>;

		template <class T>
		using Wave_lengths_dist =
		    Wave_characteristic_distribution<T, Characteristic::Wave_length>;

		template <class T>
		using Crest_lengths_dist =
		    Wave_characteristic_distribution<T, Characteristic::Crest_length>;

		template <class T>
		using Wave_periods_dist =
		    Wave_characteristic_distribution<T, Characteristic::Wave_period>;

		template <class T>
		using Wave_slopes_dist =
		    Wave_characteristic_distribution<T, Characteristic::Wave_slope>;

		template <class T>
		using Threedimensionality_dist =
		    Wave_characteristic_distribution<T, Characteristic::Threedimensionality>;

	}

}

#endif // DISTRIBUTION_HH
