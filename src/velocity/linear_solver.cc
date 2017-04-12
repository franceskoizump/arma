#include "linear_solver.hh"
#include "physical_constants.hh"
#include "derivative.hh"
#include "interpolate.hh"

#include <stdexcept>
#include <cmath>

template <class T>
void
arma::velocity::Linear_solver<T>::precompute(const Discrete_function<T,3>& zeta) {
	_fft.init(Shape2D(zeta.extent(1), zeta.extent(2)));
	_zeta_t.resize(zeta.shape());
}

template <class T>
void
arma::velocity::Linear_solver<T>::precompute(
	const Discrete_function<T,3>& zeta,
	const int idx_t
) {
	using blitz::Range;
	_zeta_t(idx_t, Range::all(), Range::all()) = -derivative<0,T>(
		zeta,
		zeta.grid().delta(),
		idx_t
	);
}

template <class T>
arma::Array2D<T>
arma::velocity::Linear_solver<T>::compute_velocity_field_2d(
	const Discrete_function<T,3>& zeta,
	const Shape2D arr_size,
	const T z,
	const int idx_t
) {
	using blitz::all;
	using blitz::isfinite;
	using blitz::Range;
	/**
	1. Compute window function.
	\f[
	\mathcal{W}(u, v) =
		4\pi \frac{ \cosh\left(|\vec{k}|(z + h)\right) }
				{ |\vec{k}|\cosh\left(|\vec{k}|h\right) }
	\f]
	*/
	const Domain<T,2> wngrid(this->_wnmax, arr_size);
	Array2D<T> mult = low_amp_window_function(wngrid, z);
	if (!all(isfinite(mult))) {
		std::clog << "Infinite/NaN multiplier. Try to increase minimal z "
			"coordinate at which velocity potential is calculated, or "
			"decrease water depth. Here z="
			<< z << ",depth=" << this->_depth << '.' << std::endl;
		throw std::runtime_error("bad multiplier");
	}
	/// 2. Compute \f$\zeta_t\f$.
	Array2D<Cmplx> phi(arr_size);
	phi = _zeta_t(idx_t, Range::all(), Range::all());
	/**
	3. Compute Fourier transforms.
	\f[
	\phi(x,y,z,t) =
		\text{Re}\left\{
			\mathcal{F}_{x,y}^{-1}\left\{
				\mathcal{W}(u, v) \mathcal{F}_{u,v}\left\{\zeta_t\right\}
			\right\}
		\right\}
	\f]
	*/
	return blitz::real(_fft.backward(_fft.forward(phi) *= mult)).copy();
}

template <class T>
arma::Array2D<T>
arma::velocity::Linear_solver<T>::low_amp_window_function(
	const Domain<T,2>& wngrid,
	const T z
) {
	using std::cosh;
	using std::isfinite;
	using blitz::length;
	using constants::_2pi;
	const T h = this->_depth;
	Array2D<T> result(wngrid.num_points());
	const int nx = wngrid.num_points(0);
	const int ny = wngrid.num_points(1);
	for (int i=0; i<nx; ++i) {
		for (int j=0; j<ny; ++j) {
			const T l = _2pi<T> * length(wngrid({i,j}));
			const T numerator = cosh(l*(z + h));
			const T denominator = l*cosh(l*h);
			result(i, j) = _2pi<T> * T(2) * numerator / denominator;
		}
	}
	// replace infinite value with values from neighbouring points
	result(0,0) = T(0);
	int n = 0;
	if (isfinite(result(1,1))) {
		result(0,0) += result(1,1);
		++n;
	}
	result(0,0) /= n;
	for (int i=1; i<nx; ++i) {
		if (!isfinite(result(i,0))) {
			result(i,0) = interpolate({i-1,1}, {i,1}, {i-1,2}, result, {i,0});
		}
	}
	for (int j=1; j<ny; ++j) {
		if (!isfinite(result(0,j))) {
			result(0,j) = interpolate({1,j-1}, {1,j}, {2,j-1}, result, {0,j});
		}
	}
	return result;
}

template class arma::velocity::Linear_solver<ARMA_REAL_TYPE>;