// posit.cpp: Test the run-time selection of posit formats
//
// Copyright (C) 2017 Stillwater Supercomputing, Inc.
//
// This file is part of the universal numbers project, which is released under an MIT Open Source license.

#include "common.hpp"

#include <posit>

#include <boost/variant.hpp>

#include "../../utilities/es_select.hpp"
#include "../../utilities/nbits_select.hpp"
#include "../../utilities/nested_apply_visitor.hpp"


struct print_es_variant 
  : public boost::static_visitor<void>
{
    template <std::size_t ES>
    void operator()(const es_tag<ES>&) const
    {
		std::cout << "es = " << ES << std::endl;
    }
    
};

struct print_nbits_variant 
  : public boost::static_visitor<void>
{
    template <std::size_t NBITS>
    void operator()(const nbits_tag<NBITS>&) const
    {
		std::cout << "nbits = " << NBITS << std::endl;
    }
    
};

template <std::size_t Nbits, std::size_t ES>
struct dummy_posit
{

	void whoami() const
	{
		std::cout << "I am dummy_posit<" << Nbits << ", " << ES << ">.\n";
	}

};

struct posit_test1
{
	template <std::size_t Nbits, std::size_t ES>
	void operator()() const
	{
		if (ES >= Nbits) {
			std::cerr << "Are you insane? Nbits must be larger than ES.\n";
			throw "Stupid test";
		}

		dummy_posit<Nbits, ES> dp;
		dp.whoami();
	}

};

double _global_value_to_assign = 0.0;

struct posit_dispatcher
{
	posit_dispatcher() : _value(_global_value_to_assign) {}

    template <std::size_t nbits, std::size_t es>
    void operator()() const
    {
        if (es+2 > nbits) {
            std::cerr << "Size of posit defined by nbits must be larger than es+2.\n";
            throw "Invalid posit configuration";
        }
            
		// from namespace sw::unum::
        sw::unum::posit<nbits, es> p(_value);
		std::cout << spec_to_string(p) << std::endl;
		std::cout << components_to_string(p) << std::endl;
    }
    
	double _value;
};

int main(int argc, char** argv)
try {
	using namespace std;
	using namespace sw::unum;

	_global_value_to_assign = 1.0;
	int errorNr = 0;

    cout << "Usage: posit nbits es float-value\n";
   
    nbits_variant nbitsv = nbits_tag<8>{};               // init to avoid trouble without cmd line args
    if (argc > 1)
        nbitsv = nbits_select(size_t(stoull(argv[1])));
    
    boost::apply_visitor(print_nbits_variant{}, nbitsv);
     
    es_variant esv = es_tag<0>{};                           // init to avoid trouble without cmd line args
    if (argc > 2)
        esv = es_select(size_t(stoull(argv[2])));
    
    boost::apply_visitor(print_es_variant{}, esv);
#if 0 
	if (argc > 3)
		_global_value_to_assign = stod(argv[3]);

    
    // And now it all boils down to this:
	nested_apply_visitor(posit_dispatcher{}, nbitsv, esv);
#endif

	return (errorNr > 0 ? EXIT_FAILURE : EXIT_SUCCESS);
}
catch (unsupported_nbits_variant& err) {
	std::cerr << err.what() << std::endl;
	return EXIT_FAILURE;
}
catch (...) {
	std::cerr << "Caught unknown exception" << std::endl;
	return EXIT_FAILURE;
}
