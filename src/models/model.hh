#ifndef MODELS_MODEL_HH
#define MODELS_MODEL_HH

#include "../types.hh"

namespace arma {

	template<class T>
	class Basic_ARMA_model {
	public:

		Basic_ARMA_model() = default;
		Basic_ARMA_model(const Basic_ARMA_model&) = default;
		Basic_ARMA_model(Basic_ARMA_model&&) = default;
		virtual ~Basic_ARMA_model() = default;

		virtual T white_noise_variance() const = 0;
		virtual void validate() const = 0;
		virtual void determine_coefficients() = 0;

		virtual void
		operator()(
			Array3D<T>& zeta,
			Array3D<T>& eps,
			const Domain3D& subdomain
		) = 0;

		void
		operator()(Array3D<T>& zeta, Array3D<T>& eps) {
			operator()(zeta, eps, zeta.domain());
		}

	};

}

#endif // MODELS_MODEL_HH
