#ifndef FOURIER_HH
#define FOURIER_HH

#include <gsl/gsl_fft_complex.h>
#include <gsl/gsl_fft_complex_float.h>
#include <gsl/gsl_fft_real.h>
#include <gsl/gsl_fft_real_float.h>
#include <blitz/array.h>

namespace arma {

	enum struct Fourier_domain { Real, Complex };

	template <class T, Fourier_domain D>
	struct Fourier_workspace;

	template <>
	struct Fourier_workspace<double, Fourier_domain::Complex> {

		typedef gsl_fft_complex_workspace workspace_type;

		explicit Fourier_workspace(size_t n)
		    : _workspace(gsl_fft_complex_workspace_alloc(n)) {}
		~Fourier_workspace() { gsl_fft_complex_workspace_free(_workspace); }

		operator const workspace_type*() const { return _workspace; }
		operator workspace_type*() { return _workspace; }

		size_t
		size() const noexcept {
			return _workspace->n;
		}

	private:
		workspace_type* _workspace;
	};

	template <>
	struct Fourier_workspace<float, Fourier_domain::Complex> {

		typedef gsl_fft_complex_workspace_float workspace_type;

		explicit Fourier_workspace(size_t n)
		    : _workspace(gsl_fft_complex_workspace_float_alloc(n)) {}
		~Fourier_workspace() {
			gsl_fft_complex_workspace_float_free(_workspace);
		}

		operator const workspace_type*() const { return _workspace; }
		operator workspace_type*() { return _workspace; }

		size_t
		size() const noexcept {
			return _workspace->n;
		}

	private:
		workspace_type* _workspace;
	};

	template <>
	struct Fourier_workspace<double, Fourier_domain::Real> {

		typedef gsl_fft_real_workspace workspace_type;

		explicit Fourier_workspace(size_t n)
		    : _workspace(gsl_fft_real_workspace_alloc(n)) {}
		~Fourier_workspace() { gsl_fft_real_workspace_free(_workspace); }

		operator const workspace_type*() const { return _workspace; }
		operator workspace_type*() { return _workspace; }

		size_t
		size() const noexcept {
			return _workspace->n;
		}

	private:
		workspace_type* _workspace;
	};

	template <>
	struct Fourier_workspace<float, Fourier_domain::Real> {

		typedef gsl_fft_real_workspace_float workspace_type;

		explicit Fourier_workspace(size_t n)
		    : _workspace(gsl_fft_real_workspace_float_alloc(n)) {}
		~Fourier_workspace() { gsl_fft_real_workspace_float_free(_workspace); }

		operator const workspace_type*() const { return _workspace; }
		operator workspace_type*() { return _workspace; }

		size_t
		size() const noexcept {
			return _workspace->n;
		}

	private:
		workspace_type* _workspace;
	};

	template <class T, Fourier_domain D>
	struct Fourier_wavetable;

	template <>
	struct Fourier_wavetable<double, Fourier_domain::Complex> {

		typedef gsl_fft_complex_wavetable wavetable_type;

		explicit Fourier_wavetable(size_t n)
		    : _wavetable(gsl_fft_complex_wavetable_alloc(n)) {}
		~Fourier_wavetable() { gsl_fft_complex_wavetable_free(_wavetable); }

		operator const wavetable_type*() const { return _wavetable; }
		operator wavetable_type*() { return _wavetable; }

		size_t
		size() const noexcept {
			return _wavetable->n;
		}

	private:
		wavetable_type* _wavetable;
	};

	template <>
	struct Fourier_wavetable<float, Fourier_domain::Complex> {

		typedef gsl_fft_complex_wavetable_float wavetable_type;

		explicit Fourier_wavetable(size_t n)
		    : _wavetable(gsl_fft_complex_wavetable_float_alloc(n)) {}
		~Fourier_wavetable() {
			gsl_fft_complex_wavetable_float_free(_wavetable);
		}

		operator const wavetable_type*() const { return _wavetable; }
		operator wavetable_type*() { return _wavetable; }

		size_t
		size() const noexcept {
			return _wavetable->n;
		}

	private:
		wavetable_type* _wavetable;
	};

	template <>
	struct Fourier_wavetable<double, Fourier_domain::Real> {

		typedef gsl_fft_real_wavetable wavetable_type;

		explicit Fourier_wavetable(size_t n)
		    : _wavetable(gsl_fft_real_wavetable_alloc(n)) {}
		~Fourier_wavetable() { gsl_fft_real_wavetable_free(_wavetable); }

		operator const wavetable_type*() const { return _wavetable; }
		operator wavetable_type*() { return _wavetable; }

		size_t
		size() const noexcept {
			return _wavetable->n;
		}

	private:
		wavetable_type* _wavetable;
	};

	template <>
	struct Fourier_wavetable<float, Fourier_domain::Real> {

		typedef gsl_fft_real_wavetable_float wavetable_type;

		explicit Fourier_wavetable(size_t n)
		    : _wavetable(gsl_fft_real_wavetable_float_alloc(n)) {}
		~Fourier_wavetable() { gsl_fft_real_wavetable_float_free(_wavetable); }

		operator const wavetable_type*() const { return _wavetable; }
		operator wavetable_type*() { return _wavetable; }

		size_t
		size() const noexcept {
			return _wavetable->n;
		}

	private:
		wavetable_type* _wavetable;
	};

	template <class T, Fourier_domain D>
	struct Basic_fourier_transform;

	template <>
	struct Basic_fourier_transform<double, Fourier_domain::Complex> {

		typedef Fourier_wavetable<double, Fourier_domain::Complex>
		    wavetable_type;
		typedef Fourier_workspace<double, Fourier_domain::Complex>
		    workspace_type;

		explicit Basic_fourier_transform(size_t n)
		    : _wavetable(n), _workspace(n) {}

		template <class T>
		void
		forward(T* rhs, size_t stride) {
			gsl_fft_complex_forward(rhs, stride, _wavetable.size(),
			                          _wavetable, _workspace);
		}

		template <class T>
		void
		backward(T* rhs, size_t stride) {
			gsl_fft_complex_backward(rhs, stride, _wavetable.size(),
			                                   _wavetable, _workspace);
		}

		size_t
		size() const noexcept {
			return _wavetable.size();
		}

	private:
		wavetable_type _wavetable;
		workspace_type _workspace;
	};

	template <>
	struct Basic_fourier_transform<float, Fourier_domain::Complex> {

		typedef Fourier_wavetable<float, Fourier_domain::Complex>
		    wavetable_type;
		typedef Fourier_workspace<float, Fourier_domain::Complex>
		    workspace_type;

		explicit Basic_fourier_transform(size_t n)
		    : _wavetable(n), _workspace(n) {}

		template <class T>
		void
		forward(T* rhs, size_t stride) {
			gsl_fft_complex_float_forward(rhs, stride, _wavetable.size(),
			                                _wavetable, _workspace);
		}

		template <class T>
		void
		backward(T* rhs, size_t stride) {
			gsl_fft_complex_float_backward(
			    rhs, stride, _wavetable.size(), _wavetable, _workspace);
		}

		size_t
		size() const noexcept {
			return _wavetable.size();
		}

	private:
		wavetable_type _wavetable;
		workspace_type _workspace;
	};

	template <>
	struct Basic_fourier_transform<double, Fourier_domain::Real> {

		typedef Fourier_wavetable<double, Fourier_domain::Real> wavetable_type;
		typedef Fourier_workspace<double, Fourier_domain::Real> workspace_type;

		explicit Basic_fourier_transform(size_t n)
		    : _wavetable(n), _workspace(n) {}

		template <class T>
		void
		forward(T* rhs, size_t stride) {
			gsl_fft_real_forward(rhs, stride, _wavetable.size(), _wavetable,
			                       _workspace);
		}

		template <class T>
		void
		backward(T* rhs, size_t stride) {
			gsl_fft_real_backward(rhs, stride, _wavetable.size(),
			                                _wavetable, _workspace);
		}

		size_t
		size() const noexcept {
			return _wavetable.size();
		}

	private:
		wavetable_type _wavetable;
		workspace_type _workspace;
	};

	template <>
	struct Basic_fourier_transform<float, Fourier_domain::Real> {

		typedef Fourier_wavetable<float, Fourier_domain::Real> wavetable_type;
		typedef Fourier_workspace<float, Fourier_domain::Real> workspace_type;

		explicit Basic_fourier_transform(size_t n)
		    : _wavetable(n), _workspace(n) {}

		template <class T>
		void
		forward(T* rhs, size_t stride) {
			gsl_fft_real_float_transform(rhs, stride, _wavetable.size(),
			                             _wavetable, _workspace);
		}

		template <class T>
		void
		backward(T* rhs, size_t stride) {
			gsl_fft_real_float_backward(
			    rhs, stride, _wavetable.size(), _wavetable, _workspace);
		}

		size_t
		size() const noexcept {
			return _wavetable.size();
		}

	private:
		wavetable_type _wavetable;
		workspace_type _workspace;
	};

	template <class T, int N, Fourier_domain D>
	struct Fourier_transform {

		typedef blitz::TinyVector<int, N> shape_type;

		Fourier_transform() = default;

		explicit
		Fourier_transform(const shape_type& shape) {
			init(shape);
		}

		void
		init(const shape_type& shp) {
			if (blitz::any(shp != shape())) {
				_transforms.clear();
				for (int i = 0; i < N; ++i) {
					_transforms.emplace_back(shp(i));
				}
			}
		}

		shape_type
		shape() const noexcept {
			shape_type result;
			for (int i = 0; i < N; ++i) {
				result(i) = _transforms[i].size();
			}
			return result;
		}

		template <class X>
		blitz::Array<X, N>
		forward(blitz::Array<X, N> rhs) {
			blitz::Array<X, N> result(rhs.copy());
			for (int i = 0; i < N; ++i) {
				_transforms[i].forward(result.data(), result.stride(i));
			}
			return result;
		}

		template <class X>
		blitz::Array<X, N>
		backward(blitz::Array<X, N> rhs) {
			blitz::Array<X, N> result(rhs.copy());
			for (int i = 0; i < N; ++i) {
				_transforms[i].backward(result.data(), result.stride(i));
			}
			return result;
		}

		friend std::ostream&
		operator<<(std::ostream& out, const Fourier_transform& rhs) {
			return out << "shape=" << rhs.shape();
		}

	private:
		std::vector<Basic_fourier_transform<T, D>> _transforms;
	};
}

#endif // FOURIER_HH
