// nbits_select.hpp: select nbits value at run-time
//
// Copyright (C) 2017 Stillwater Supercomputing, Inc.
//
// This file is part of the universal numbers project, which is released under an MIT Open Source license.

#pragma once

#include <type_traits>
#include <stdexcept>

#include <boost/variant.hpp>

template <std::size_t NBITS>
using nbits_tag = std::integral_constant<std::size_t, NBITS>;

/// All possible variants of nbits values.
using nbits_variant = boost::variant<nbits_tag<3>, nbits_tag<4>, nbits_tag<5>, nbits_tag<6>, nbits_tag<7>, nbits_tag<8>, nbits_tag<9>, nbits_tag<10>, nbits_tag<11>, nbits_tag<12>, nbits_tag<13>, nbits_tag<14>, nbits_tag<15>, nbits_tag<16>, nbits_tag<17>, nbits_tag<18>, nbits_tag<19>, nbits_tag<20>, nbits_tag<21>, nbits_tag<22> >;
using standard_variant = boost::variant<nbits_tag<8>, nbits_tag<16>, nbits_tag<32>, nbits_tag<64>>;
using standard_ext_variant = boost::variant<nbits_tag<4>, nbits_tag<8>, nbits_tag<12>, nbits_tag<16>, nbits_tag<20>, nbits_tag<24>, nbits_tag<32>, nbits_tag<40>, nbits_tag<48>, nbits_tag<56>, nbits_tag<64>>;

struct unsupported_nbits_variant
  : std::runtime_error
{
	unsupported_nbits_variant(size_t nbits) : std::runtime_error("nbits-variant: " + std::to_string(nbits) + " not supported") {}
};

/// Return the according variant or throw an exception.
//  Inline to avoid needing cpp file.
inline nbits_variant nbits_select(size_t nbits)
{
    switch (nbits) {
		case 3: return nbits_tag<3>{};
        case 4: return nbits_tag<4>{};
		case 5: return nbits_tag<5>{};
		case 6: return nbits_tag<6>{};
		case 7: return nbits_tag<7>{};
        case 8: return nbits_tag<8>{};
		case 9: return nbits_tag<9>{};
		case 10: return nbits_tag<10>{};
		case 11: return nbits_tag<11>{};
		case 12: return nbits_tag<12>{};
		case 13: return nbits_tag<13>{};
		case 14: return nbits_tag<14>{};
		case 15: return nbits_tag<15>{};
        case 16: return nbits_tag<16>{};
		case 17: return nbits_tag<17>{};
		case 18: return nbits_tag<18>{};
		case 19: return nbits_tag<19>{};
		case 20: return nbits_tag<20>{};
		case 21: return nbits_tag<21>{};
		case 22: return nbits_tag<22>{};

        default: throw unsupported_nbits_variant{nbits}; 
    }
    
}

//  Inline to avoid needing cpp file.
inline standard_variant standard_select(size_t nbits)
{
	switch (nbits) {
	case  8: return nbits_tag<8>{};
	case 16: return nbits_tag<16>{};
	case 32: return nbits_tag<32>{};
	case 64: return nbits_tag<64>{};

	default: throw unsupported_nbits_variant{ nbits };
	}

}

//  Inline to avoid needing cpp file.
inline standard_ext_variant standard_ext_select(size_t nbits)
{
	switch (nbits) {
	case 4: return nbits_tag<4>{};
	case 8: return nbits_tag<8>{};
	case 10: return nbits_tag<12>{};
	case 16: return nbits_tag<16>{};
	case 20: return nbits_tag<20>{};
	case 24: return nbits_tag<24>{};
	case 32: return nbits_tag<32>{};
	case 40: return nbits_tag<40>{};
	case 48: return nbits_tag<48>{};
	case 56: return nbits_tag<56>{};
	case 64: return nbits_tag<64>{};

	default: throw unsupported_nbits_variant{ nbits };
	}

}
