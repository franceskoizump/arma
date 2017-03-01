#ifndef VELOCITY_POTENTIAL_FIELD_HH
#define VELOCITY_POTENTIAL_FIELD_HH

#include "types.hh"

namespace arma {

	template<class T>
	class Velocity_potential_field {

	public:
		Velocity_potential_field() = default;
		Velocity_potential_field(const Velocity_potential_field&) = default;
		Velocity_potential_field(Velocity_potential_field&&) = default;
		virtual ~Velocity_potential_field() = default;

		/**
		\param[in] zeta      ocean wavy surface
		\param[in] subdomain region of zeta
		\param[in] z         a coordinate \f$z\f$ in which to compute velocity
		                     potential
		\param[in] idx_t     a time point in which to compute velocity potential,
		                     specified as index of zeta
		*/
		virtual Array2D<T>
		operator()(
			Array3D<T>& zeta,
			const Domain3D& subdomain,
			const T z,
			const int idx_t
		) = 0;

		Array2D<T>
		operator()(Array3D<T>& zeta, Array3D<T>& eps) {
			return operator()(zeta, eps, zeta.domain());
		}

	};

}

#endif // VELOCITY_POTENTIAL_FIELD_HH