#ifndef VECTOR_N_HH
#define VECTOR_N_HH

#include <blitz/array.h>

/**
@file domain-specific types and templates
*/

namespace arma {

	template <class T, size_t N>
	using Vector = blitz::TinyVector<T, N>;

	typedef Vector<int, 3> Shape3D;
	typedef Vector<int, 2> Shape2D;
	typedef Vector<int, 1> Shape1D;

	template <class T>
	using Vec3D = Vector<T, 3>;
	template <class T>
	using Vec2D = Vector<T, 2>;
	template <class T>
	using Vec1D = Vector<T, 1>;

	template <class T>
	using Array4D = blitz::Array<T, 4>;
	template <class T>
	using Array3D = blitz::Array<T, 3>;
	template <class T>
	using Array2D = blitz::Array<T, 2>;
	template <class T>
	using Array1D = blitz::Array<T, 1>;

	typedef blitz::RectDomain<1> Domain1D;
	typedef blitz::RectDomain<2> Domain2D;
	typedef blitz::RectDomain<3> Domain3D;
}

#endif // VECTOR_N_HH
