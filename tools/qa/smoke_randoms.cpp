// smoke_sub.cpp: generate smoke tests for subtraction
//
// Copyright (C) 2017 Stillwater Supercomputing, Inc.
//
// This file is part of the universal numbers project, which is released under an MIT Open Source license.

#include "stdafx.h"
#include <chrono>
#include <ctime>

#include <posit>
#include "../tests/posit_test_helpers.hpp"
#include "qa_helpers.hpp"

using namespace std;

template<size_t nbits, size_t es>
int GenerateSmokeTests(bool bReportIndividualTestCases, std::string& cmd, unsigned nrOfRandoms = 10) {
	int nrOfFailedTestCases = 0;
	if (cmd == "add") {
		nrOfFailedTestCases = sw::qa::SmokeTestRandoms<nbits, es>("random smoke testing", sw::qa::OPCODE_ADD, nrOfRandoms);
	}
	else if (cmd == "sub") {
		nrOfFailedTestCases = sw::qa::SmokeTestRandoms<nbits, es>("random smoke testing", sw::qa::OPCODE_SUB, nrOfRandoms);
	}
	else if (cmd == "mul") {
		nrOfFailedTestCases = sw::qa::SmokeTestRandoms<nbits, es>("random smoke testing", sw::qa::OPCODE_MUL, nrOfRandoms);
	}
	else if (cmd == "div") {
		nrOfFailedTestCases = sw::qa::SmokeTestRandoms<nbits, es>("random smoke testing", sw::qa::OPCODE_DIV, nrOfRandoms);
	}
	
	return nrOfFailedTestCases;
}

// Generate smoke tests for different posit configurations
// Usage: qa_smoke_randoms 16/24/32/48/64
int main(int argc, char** argv)
try {
	typedef std::numeric_limits< double > dbl;
	//cerr << "double max digits " << dbl::max_digits10 << endl;

	unsigned nrOfRandoms = 10;

	int posit_size = 32;  // default
	std::string cmd = "add";
	if (argc == 2) {
		posit_size = std::stoi(argv[1]);
	}
	else if (argc == 3) {
		posit_size = std::stoi(argv[1]);
		cmd = argv[2];
	}
	else if (argc == 4) {
		posit_size = std::stoi(argv[1]);
		cmd = argv[2];
		nrOfRandoms = std::stoi(argv[3]);
	}
	cerr << "Generating random smoke tests for posits of size " << posit_size << " and command " << cmd << endl;

	bool bReportIndividualTestCases = true;
	int nrOfFailedTestCases = 0;


	switch (posit_size) {
	case 16:
		nrOfFailedTestCases = GenerateSmokeTests<16, 1>(bReportIndividualTestCases, cmd, nrOfRandoms);
		break;
	case 24:
		nrOfFailedTestCases = GenerateSmokeTests<24, 1>(bReportIndividualTestCases, cmd, nrOfRandoms);
		break;
	case 32:
		nrOfFailedTestCases = GenerateSmokeTests<32, 2>(bReportIndividualTestCases, cmd, nrOfRandoms);
		break;
	case 48:
		nrOfFailedTestCases = GenerateSmokeTests<48, 2>(bReportIndividualTestCases, cmd, nrOfRandoms);
		break;
	case 64:
		nrOfFailedTestCases = GenerateSmokeTests<64, 3>(bReportIndividualTestCases, cmd, nrOfRandoms);
		break;
	default:
		nrOfFailedTestCases = 1;
	}

	return (nrOfFailedTestCases > 0 ? EXIT_FAILURE : EXIT_SUCCESS);
}
catch (char const* msg) {
	cerr << msg << endl;
	return EXIT_FAILURE;
}
catch (...) {
	cerr << "Caught unknown exception" << endl;
	return EXIT_FAILURE;
}
