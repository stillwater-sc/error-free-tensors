// neural_network.cpp example program using posit arithmetic for a neural network simulation
//
// Copyright (C) 2017-2018 Stillwater Supercomputing, Inc.
//
// This file is part of the universal numbers project, which is released under an MIT Open Source license.

#include "common.hpp"

#include <posit>
#include <boost/numeric/mtl/mtl.hpp>

// Turn it off for now
#undef USE_POSIT

template<size_t nbits, size_t es>
sw::unum::posit<nbits, es> Sigmoid(sw::unum::posit<nbits, es>& x, bool derivative = false) {
	if (derivative) return x*(1-x);
	return (1)
}

int main(int argc, char** argv)
try {
	using namespace std;
	using namespace mtl;
	using namespace sw::unum;

	const size_t nbits = 16;
	const size_t es = 1;
	const size_t vecSize = 32;

#ifdef USE_POSIT
	using Ty     = sw::unum::posit<8, 0>;
	using Matrix = mtl::dense2D< Ty >;
	using Vector = mtl::dense_vector< Ty >;
#else
	using Ty     = float;
	using Matrix = mtl::dense2D<float>;
	using Vector = mtl::dense_vector<float>;
#endif

	Matrix X = {
		{0,0,1},
		{0,1,1},
		{1,0,1},
		{1,1,1}
	};

	// output data
	Vector y = { 0, 1, 1, 0 };

	// synapse layers: syn0 = 2*np.random.random((3,4)) - 1
	Matrix synapse_L0(3,4);
	Matrix synapse_L1(4,1);

    constexpr int TRAINING_STEPS     = 50000;
    constexpr int REPORTING_INTERVAL = 10000;

	// training 
	for ( int j = 0; j < TRAINING_STEPS; j++ ) {
		Matrix level_0 = X;
		Matrix level_1 = Sigmoid<nbits, es>(dot(level_0, synapse_L0));
		Matrix Level_2 = Sigmoid<nbits, es>(dot(level_1, synapse_L1));

		Vector l2_error = y - level_2;

		if (j % REPORTING_INTERVAL == 0) {
			cout << "Error: " << Mean(Abs(l2_error)) << '\n';
		}

		l2_delta = l2_error * Sigmoid<nbits, es>(l2, true);
		l1_error = l2_delta.dot(Transpose(synapse_L1));
		l1_delta = l1_error * Sigmoid<nbits, es>(l1, true);

		// update weights
		synapse_L1 += dot(Transpose(l1), l2_delta);
		synapse_L0 += dot(Transpose(l0), l1_delta);
	}

	cout << "Output after training\n";
	cout << l2 << endl;

	return EXIT_SUCCESS;
}
catch (char const* msg) {
	std::cerr << msg << std::endl;
	return EXIT_FAILURE;
}
catch (...) {
	std::cerr << "Caught unknown exception" << std::endl;
	return EXIT_FAILURE;
}