#include "parallel_mt.hh"

#include "errors.hh"

#include <random>
#include <chrono>

std::ostream&
arma::prng::operator<<(std::ostream& out, const parallel_mt& rhs) {
	return out << "aaa=" << rhs._config.aaa
			   << ",mm=" << rhs._config.mm
			   << ",nn=" << rhs._config.nn
			   << ",rr=" << rhs._config.rr
			   << ",ww=" << rhs._config.ww
			   << ",wmask=" << rhs._config.wmask
			   << ",umask=" << rhs._config.umask
			   << ",lmask=" << rhs._config.lmask
			   << ",shift0=" << rhs._config.shift0
			   << ",shift1=" << rhs._config.shift1
			   << ",shiftB=" << rhs._config.shiftB
			   << ",shiftC=" << rhs._config.shiftC
			   << ",maskB=" << rhs._config.maskB
			   << ",maskC=" << rhs._config.maskC
			   << ",i=" << rhs._config.i;
}

std::vector<arma::prng::parallel_mt>
arma::prng::read_parallel_mts(const char* filename, size_t n, bool noseed) {
	std::ifstream in(filename);
	if (!in.is_open()) {
		throw std::runtime_error("bad file");
	}
	// generate seeds
	std::random_device dev;
	std::seed_seq seq{{
		noseed ? 0 : clock_seed(),
		noseed ? 0 : clock_type::rep(dev())
	}};
	std::vector<parallel_mt::result_type> seeds(n);
	seq.generate(seeds.begin(), seeds.end());
	// read generator configurations
	std::vector<parallel_mt> mts;
	std::istream_iterator<mt_config> first(in), last;
	size_t i = 0;
	while (first != last && i < n) {
		mts.emplace_back(*first, seeds[i]);
		++first;
		++i;
	}
	if (mts.size() < n) {
		throw PRNG_error("bad number of MT configs", mts.size(), n);
	}
	return mts;
}

