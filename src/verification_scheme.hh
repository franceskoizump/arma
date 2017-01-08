#ifndef VERIFICATION_SCHEME_HH
#define VERIFICATION_SCHEME_HH

#include <istream>
#include <ostream>
#include <iostream>
#include <string>
#include <stdexcept>

namespace autoreg {

	enum struct Verification_scheme {
		None = 0,
		Summary = 1,
		Quantile = 2,
		Manual = 3,
	};

	std::istream&
	operator>>(std::istream& in, Verification_scheme& rhs) {
		std::string name;
		in >> std::ws >> name;
		if (name == "none") {
			rhs = Verification_scheme::None;
		} else if (name == "summary") {
			rhs = Verification_scheme::Summary;
		} else if (name == "quantile") {
			rhs = Verification_scheme::Quantile;
		} else if (name == "manual") {
			rhs = Verification_scheme::Manual;
		} else {
			in.setstate(std::ios::failbit);
			std::clog << "Invalid verification scheme: " << name << std::endl;
			throw std::runtime_error("bad varification scheme");
		}
		return in;
	}

	const char*
	to_string(Verification_scheme rhs) {
		switch (rhs) {
			case Verification_scheme::None: return "none";
			case Verification_scheme::Summary: return "summary";
			case Verification_scheme::Quantile: return "quantile";
			case Verification_scheme::Manual: return "manual";
			default: return "UNKNOWN";
		}
	}

	std::ostream&
	operator<<(std::ostream& out, const Verification_scheme& rhs) {
		return out << to_string(rhs);
	}


}

#endif // VERIFICATION_SCHEME_HH
