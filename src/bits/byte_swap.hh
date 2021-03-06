#ifndef BITS_BYTE_SWAP_HH
#define BITS_BYTE_SWAP_HH

#include <algorithm>

#define SYSX_GCC_VERSION_AT_LEAST(major, minor) \
	((__GNUC__ > major) || (__GNUC__ == major && __GNUC_MINOR__ >= minor))

#if SYSX_GCC_VERSION_AT_LEAST(4, 3)
	#define ARMA_HAVE_BUILTIN_SWAP16
	#define ARMA_HAVE_BUILTIN_SWAP32
	#define ARMA_HAVE_BUILTIN_SWAP64
#endif

namespace arma {

	namespace bits {

		template<class T>
		inline T
		byte_swap (T n) noexcept {
			union {
				T x;
				unsigned char y[sizeof(T)];
			} tmp;
			tmp.x = n;
			std::reverse(tmp.y, tmp.y + sizeof(T));
			return tmp.x;
		}

		template<>
		constexpr uint8_t
		byte_swap<uint8_t>(uint8_t n) noexcept {
			return n;
		}

		template<>
		constexpr uint16_t
		byte_swap<uint16_t>(uint16_t n) noexcept {
			#if defined(ARMA_HAVE_BUILTIN_SWAP16)
			return __builtin_bswap16(n);
			#else
			return ((n & 0xff00)>>8) | ((n & 0x00ff)<<8);
			#endif
		}

		template<>
		constexpr uint32_t
		byte_swap<uint32_t>(uint32_t n) noexcept {
			#if defined(ARMA_HAVE_BUILTIN_SWAP32)
			return __builtin_bswap32(n);
			#else
			return
				((n & UINT32_C(0xff000000)) >> 24) |
				((n & UINT32_C(0x00ff0000)) >> 8) |
				((n & UINT32_C(0x0000ff00)) << 8) |
				((n & UINT32_C(0x000000ff)) << 24);
			#endif
		}

		template<>
		constexpr uint64_t
		byte_swap<uint64_t>(uint64_t n) noexcept {
			#if defined(ARMA_HAVE_BUILTIN_SWAP64)
			return __builtin_bswap64(n);
			#else
			return
				((n & UINT64_C(0xff00000000000000)) >> 56) |
				((n & UINT64_C(0x00ff000000000000)) >> 40) |
				((n & UINT64_C(0x0000ff0000000000)) >> 24) |
				((n & UINT64_C(0x000000ff00000000)) >> 8) |
				((n & UINT64_C(0x00000000ff000000)) << 8) |
				((n & UINT64_C(0x0000000000ff0000)) << 24) |
				((n & UINT64_C(0x000000000000ff00)) << 40) |
				((n & UINT64_C(0x00000000000000ff)) << 56);
			#endif
		}

		constexpr bool
		is_network_byte_order() noexcept {
			#if defined(WORDS_BIGENDIAN) \
				|| __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
			return true;
			#else
			return false;
			#endif
		}

		template<class Int, bool integral>
		struct byte_swap_chooser {};

		template<class Int>
		struct byte_swap_chooser<Int, true> {

			inline void
			to_network_format() noexcept {
				if (!is_network_byte_order()) {
					_intval = byte_swap<Int>(_intval);
				}
			}

			inline void
			to_host_format() noexcept {
				this->to_network_format();
			}

			constexpr bool
			operator==(const byte_swap_chooser& rhs) const noexcept {
				return _intval == rhs._intval;
			}

		private:
			Int _intval;
		};

		template<class Arr>
		struct byte_swap_chooser<Arr, false> {

			inline void
			to_network_format() noexcept {
				if (!is_network_byte_order()) {
					std::reverse(_arr, _arr + sizeof(Arr));
				}
			}

			inline void
			to_host_format() noexcept {
				this->to_network_format();
			}

			inline bool
			operator==(const byte_swap_chooser& rhs) const noexcept {
				return std::equal(_arr, _arr + sizeof(Arr), rhs._arr);
			}

		private:
			Arr _arr;
		};

		template<size_t bytes>
		struct Integral: public byte_swap_chooser<uint8_t[bytes],false> {};
		template<> struct Integral<1>: public byte_swap_chooser<uint8_t,true> {};
		template<> struct Integral<2>: public byte_swap_chooser<uint16_t,true> {};
		template<> struct Integral<4>: public byte_swap_chooser<uint32_t,true> {};
		template<> struct Integral<8>: public byte_swap_chooser<uint64_t,true> {};

		template<class T, class Ch=char>
		union Bytes {

			typedef Ch value_type;
			typedef bits::Integral<sizeof(T)> integral_type;
			typedef value_type* iterator;
			typedef const value_type* const_iterator;
			typedef std::size_t size_type;

			constexpr
			Bytes() noexcept:
				_val{} {}

			constexpr
			Bytes(const Bytes& rhs) noexcept:
				_val(rhs._val) {}

			constexpr
			Bytes(T rhs) noexcept:
				_val(rhs) {}

			template<class It>
			Bytes(It first, It last) noexcept {
				std::copy(first, last, _bytes);
			}

			inline void
			to_network_format() noexcept {
				_intval.to_network_format();
			}

			inline void
			to_host_format() noexcept {
				_intval.to_host_format();
			}

			inline
			operator T&() noexcept {
				return _val;
			}

			constexpr
			operator const T&() const noexcept {
				return _val;
			}

			constexpr value_type
			operator[](size_type idx) const noexcept {
				return _bytes[idx];
			}

			constexpr bool
			operator==(const Bytes& rhs) const noexcept {
				return _intval == rhs._intval;
			}

			constexpr bool
			operator!=(const Bytes& rhs) const noexcept {
				return !operator==(rhs);
			}

			inline iterator
			begin() noexcept {
				return _bytes;
			}

			inline iterator
			end() noexcept {
				return _bytes + sizeof(T);
			}

			constexpr const_iterator
			begin() const noexcept {
				return _bytes;
			}

			constexpr const_iterator
			end() const noexcept {
				return _bytes + sizeof(T);
			}

			constexpr const T&
			value() const noexcept {
				return _val;
			}

			inline T&
			value() noexcept {
				return _val;
			}

			static constexpr size_type
			size() noexcept {
				return sizeof(T);
			}

		private:
			T _val;
			integral_type _intval;
			value_type _bytes[sizeof(T)];

			static_assert(
				sizeof(decltype(_val)) == sizeof(decltype(_intval)),
				"bad integral type"
			);
		};

		template<class T>
		constexpr Bytes<T>
		make_bytes(T rhs) noexcept {
			return Bytes<T>(rhs);
		}

		template<class T>
		T to_network_format(Bytes<T> n) noexcept {
			n.to_network_format();
			return n.value();
		}

		template<class T>
		T to_host_format(Bytes<T> n) noexcept {
			n.to_host_format();
			return n.value();
		}

		template<class T>
		constexpr T
		to_network_format(T n) noexcept {
			return bits::is_network_byte_order() ? n : bits::byte_swap<T>(n);
		}

		template<class T>
		constexpr T
		to_host_format(T n) noexcept {
			return bits::is_network_byte_order() ? n : bits::byte_swap<T>(n);
		}

	}

}

#endif // BITS_BYTE_SWAP_HH

