#ifndef LINEAR_VELOCITY_FIELD_HH
#define LINEAR_VELOCITY_FIELD_HH

#include <cmath>
#include <complex>
#include <stdexcept>
#include "velocity_potential_field.hh"
#include "fourier.hh"
#include "grid.hh"
#include "domain.hh"

namespace arma {

	namespace bits {

		template <class T>
		inline T
		div_or_nought(T lhs, T rhs) noexcept {
			T result = lhs / rhs;
			if (!std::isfinite(result)) {
				result = T(0);
			}
			return result;
		}

	}

	/// Linear wave theory formula to compute velocity potential field.
	template<class T>
	class Linear_velocity_potential_field: public Velocity_potential_field<T> {

		Vec2<T> _wnmax;
		T _depth;
		Fourier_transform<std::complex<T>, 2> _fft;
		Domain2<T> _domain;
		static constexpr const T _2pi = T(2) * M_PI;

	public:

		Array4D<T>
		operator()(Array3D<T>& zeta, const Domain3D& subdomain) override {
			using blitz::Range;
			const size3 zeta_size = subdomain.ubound() - subdomain.lbound();
			const size2 arr_size(zeta_size(1), zeta_size(2));
			const int nt = _domain.num_points(0);
			const int nz = _domain.num_points(1);
			Array4D<T> result(blitz::shape(
				nt, nz,
				zeta_size(1), zeta_size(2)
			));
			for (int i=0; i<nt; ++i) {
				for (int j=0; j<nz; ++j) {
					const Vec2<T> p = _domain({i,j});
					const T t = p(0);
					const T z = p(1);
					result(i, j, Range::all(), Range::all()) =
					compute_velocity_field_2d(
						zeta, arr_size,
						z, t
					);
				}
				std::clog << "Finished time slice ["
					<< (i+1) << '/' << nt << ']'
					<< std::endl;
			}
			return result;
		}

		Array2D<T>
		compute_velocity_field_2d(
			Array3D<T>& zeta,
			const size2 arr_size,
			const T z,
			const int idx_t
		) {
			using blitz::all;
			using blitz::isfinite;
			/**
			1. Compute multiplier.
			\f[
			\text{mult}(u, v) =
				-2 \frac{ \cosh\left(|\vec{k}|(z + h)\right) }
				        { |\vec{k}|\cosh\left(|\vec{k}|h\right) }
				=
				-2 \frac{ e^{-|\vec{k}|z} + e^{-|\vec{k}|(z + h)} }
				        { |\vec{k}| \left(1 + e^{-2|\vec{k}|h}\right) }
				=
				-2 \frac{ 1 + e^{-|\vec{k}|h} }
				        { |\vec{k}| \left(1 + e^{-2|\vec{k}|h}\right) e^{|\vec{k}|z} }
			\f]
			*/
			const Domain<T,2> wngrid(_wnmax, arr_size);
			Array2D<T> mult(wngrid.num_points());
			const int nx = wngrid.num_points(0);
			const int ny = wngrid.num_points(1);
			for (int i=0; i<nx; ++i) {
				for (int j=0; j<ny; ++j) {
					const T l = _2pi * blitz::length(wngrid({i,j}));
					const T numerator = T(1) + std::exp(-l*_depth);
					const T explz = std::exp(l*z);
					const T denominator = l*(explz + std::exp(T(-2)*l*(z + _depth)));
					mult(i, j) = T(-2) * bits::div_or_nought(numerator, denominator);
				}
			}
			if (!all(isfinite(mult))) {
				std::clog << __FILE__ << ':' << __LINE__ << ':' << __func__
					<< ": infinite/NaN multiplier. Try to increase minimal z "
					"coordinate at which velocity potential is calculated, or "
					"decrease water depth. Here z="
					<< z << ",depth=" << _depth << '.' << std::endl;
				throw std::runtime_error("bad multiplier");
			}
			/// 2. Compute \f$\zeta_t\f$.
			Array2D<std::complex<T>> phi(arr_size);
			// TODO Implement in a separate function with proper handling of borders.
			for (int i=0; i<nx; ++i) {
				for (int j=0; j<ny; ++j) {
					phi(i, j) = zeta(idx_t-1,i,j)
						- T(2) * zeta(idx_t,i,j)
						+ zeta(idx_t+1,i,j);
				}
			}
			/**
			3. Compute Fourier transforms.
			\f[
			\phi(x,y,z,t) =
				\mathcal{F}_{x,y}^{-1}\left\{
					\text{mult}(u, v) \mathcal{F}_{u,v}\left\{\zeta_t\right\}
				\right\}
			\f]
			*/
			_fft.init(arr_size);
			return blitz::real(_fft.backward(_fft.forward(phi) *= mult));
		}

		friend std::istream&
		operator>>(std::istream& in, Linear_velocity_potential_field& rhs) {
			sys::parameter_map params({
			    {"wnmax", sys::make_param(rhs._wnmax, validate_finite<T,2>)},
			    {"depth", sys::make_param(rhs._depth, validate_finite<T>)},
			    {"domain", sys::make_param(rhs._domain, validate_domain<T,2>)},
			}, true);
			in >> params;
			return in;
		}

		void
		write(std::ostream& out) const override {
			out << "wnmax=" << _wnmax << ','
				<< "depth=" << _depth << ','
				<< "domain=" << _domain;
		}

	};

}

#endif // LINEAR_VELOCITY_FIELD_HH
