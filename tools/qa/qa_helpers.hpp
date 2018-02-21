#pragma once
// qa_helpers.hpp: definitions of helper functions for QA smoke test generators
//
// Copyright (C) 2017 Stillwater Supercomputing, Inc.
//
// This file is part of the universal numbers project, which is released under an MIT Open Source license.
#include <random>
#include <limits>

namespace sw {
	namespace qa {

		// there are four quadrants, each with two endpoints
		// south-east  -> [minpos -   1.0)
		// north-east  -> (1.0    -   maxpos)
		// north-west  -> [-maxpos - -1.0)
		// south-west  -> (-1.0    - -minpos)

		// on each minpos/maxpos side there are 2^(es+1) patterns that carry special rounding behavior
		// es = 0:   0/minpos                            ->  2 special cases
		// es = 1:   0/minpos, 2 exponent configs        ->  4 special cases
		// es = 2:   0/minpos, 2, 4 exponent configs     ->  8 special cases
		// es = 3:   0/minpos, 2, 4, 8 exponent configs  -> 16 special cases
		// es = 4:   0/minpos, 2, 4, 8, 16 exp configs   -> 32 special cases
		// -> 2^(es+1) special cases
		//
		// plus the region around 1 that puts the most pressure on the conversion algorithm's precision
		// --1, 1, and 1++, so three extra cases per half.
		// Because we need to recognize the -minpos case, which happens to be all 1's, and is the last
		// test case in exhaustive testing, we need to have that test case end up in the last entry
		// of the test case array.

		template<size_t nbits, size_t es>
		struct TestCase {
			sw::unum::posit<nbits, es> a, b, c;
		};
		template<size_t nbits, size_t es>
		int SmokeTestAddition(std::string tag, bool bReportIndividualTestCases) {
			static_assert(nbits >= 16, "Use exhaustive testing for posits smaller than 16");
			static_assert(nbits <= 64, "TODO: smoke test algorithm only works for nbits <= 64");

			constexpr size_t fbits = nbits - 3 - es;
			constexpr size_t enumeration = fbits > 5 ? 5 : fbits;

			int nrOfFailedTests = 0;

			std::vector< TestCase<nbits, es> > test_cases;
			// minpos + minpos = minpos
			TestCase<nbits, es> test;
			test.a = sw::unum::posit<nbits, es>(sw::unum::minpos_value<nbits, es>());
			test.b = sw::unum::posit<nbits, es>(sw::unum::minpos_value<nbits, es>());
			test_cases.push_back(test);
			// all the cases that enumerate the state space of the exponent bits
			for (int i = 0; i < int(1) << (es + 2); i++) {
				test.a++; test.b++;
				test_cases.push_back(test);
			}
			test.a = sw::unum::posit<nbits, es>(sw::unum::maxpos_value<nbits, es>());
			test.b = sw::unum::posit<nbits, es>(sw::unum::maxpos_value<nbits, es>());
			test_cases.push_back(test);
			for (int i = 0; i < int(1) << (es + 2); i++) {
				test.a--; test.b--;
				test_cases.push_back(test);
			}
			test.a = 1.0;
			test.b = 1.0;
			test_cases.push_back(test);
			for (int i = 0; i < int(1) << enumeration; i++) {
				test.a--; test.b++;
				test_cases.push_back(test);
			}
			test.a = 0.5;
			test.b = 2.0;
			test_cases.push_back(test);
			for (int i = 0; i < int(1) << enumeration; i++) {
				test.a--; test.b++;
				test_cases.push_back(test);
			}

			// execute and output the test vector
			sw::unum::posit<nbits, es> pa, pb, padd, pref;
			double da, db;
			std::cout << "posit<" << nbits << "," << es << ">" << std::endl;
			std::cout << std::setw(nbits) << "Operand A  " << " + " << std::setw(nbits) << "Operand B  " << " = " << std::setw(nbits) << "Golden Reference  " << " " << std::setw(nbits/4) << "HEX " << std::endl;
			for (size_t i = 0; i < test_cases.size(); i++) {
				pa = test_cases[i].a;
				da = double(pa);
				pb = test_cases[i].b;
				db = double(pb);
				padd = pa + pb;
				pref = da + db;
				if (padd != pref) {
					if (bReportIndividualTestCases) ReportBinaryArithmeticError("FAIL", "+", pa, pb, pref, padd);
					nrOfFailedTests++;
				}
				else {
					//if (bReportIndividualTestCases) ReportBinaryArithmeticSuccess("PASS", "+", pa, pb, pref, padd);
				}
				std::cout << pa.get() << " + " << pb.get() << " = " << pref.get() << " " << sw::unum::to_hex(pref.get()) << std::endl;
			}
			return nrOfFailedTests;
		}

		template<size_t nbits, size_t es>
		int SmokeTestSubtraction(std::string tag, bool bReportIndividualTestCases) {
			static_assert(nbits >= 16, "Use exhaustive testing for posits smaller than 16");
			static_assert(nbits <= 64, "TODO: smoke test algorithm only works for nbits <= 64");

			constexpr size_t fbits = nbits - 3 - es;
			constexpr size_t enumeration = fbits > 5 ? 5 : fbits;

			int nrOfFailedTests = 0;

			std::vector< TestCase<nbits, es> > test_cases;
			// minpos + minpos = minpos?
			TestCase<nbits, es> test;
			test.a = sw::unum::posit<nbits, es>(sw::unum::minpos_value<nbits, es>());
			test.b = sw::unum::posit<nbits, es>(sw::unum::minpos_value<nbits, es>());
			test_cases.push_back(test);
			for (int i = 0; i < int(1) << (es + 2); i++) {
				test.a++; test.b++;
				test_cases.push_back(test);
			}
			test.a = sw::unum::posit<nbits, es>(sw::unum::maxpos_value<nbits, es>());
			test.b = sw::unum::posit<nbits, es>(sw::unum::maxpos_value<nbits, es>());
			test_cases.push_back(test);
			for (int i = 0; i < int(1) << (es + 2); i++) {
				test.a--; test.b--;
				test_cases.push_back(test);
			}
			test.a = 1.0;
			test.b = 1.0;
			test_cases.push_back(test);
			for (int i = 0; i < int(1) << enumeration; i++) {
				test.a--; test.b++;
				test_cases.push_back(test);
			}
			test.a = 0.5;
			test.b = 2.0;
			test_cases.push_back(test);
			for (int i = 0; i < int(1) << enumeration; i++) {
				test.a--; test.b++;
				test_cases.push_back(test);
			}

			// execute and output the test vector
			std::cout << "posit<" << nbits << "," << es << ">" << std::endl;
			std::cout << std::setw(nbits) << "Operand A  " << " - " << std::setw(nbits) << "Operand B  " << " = " << std::setw(nbits) << "Golden Reference  " << " " << std::setw(nbits / 4) << "HEX " << std::endl;

			sw::unum::posit<nbits, es> pa, pb, psub, pref;
			double da, db;
			for (size_t i = 0; i < test_cases.size(); i++) {
				pa = test_cases[i].a;
				da = double(pa);
				pb = test_cases[i].b;
				db = double(pb);
				psub = pa - pb;
				pref = da - db;
				if (psub != pref) {
					if (bReportIndividualTestCases) ReportBinaryArithmeticError("FAIL", "-", pa, pb, pref, psub);
					nrOfFailedTests++;
				}
				else {
					//if (bReportIndividualTestCases) ReportBinaryArithmeticSuccess("PASS", "+", pa, pb, pref, psub);
				}
				std::cout << pa.get() << " - " << pb.get() << " = " << pref.get() << " " << sw::unum::to_hex(pref.get()) << std::endl;
			}
			return nrOfFailedTests;
		}

		template<size_t nbits, size_t es>
		int SmokeTestMultiplication(std::string tag, bool bReportIndividualTestCases) {
			static_assert(nbits >= 16, "Use exhaustive testing for posits smaller than 16");
			static_assert(nbits <= 64, "TODO: smoke test algorithm only works for nbits <= 64");

			constexpr size_t fbits = nbits - 3 - es;
			constexpr size_t enumeration = fbits > 5 ? 5 : fbits;

			int nrOfFailedTests = 0;

			std::vector< TestCase<nbits, es> > test_cases;
			// minpos * minpos = minpos
			// minpos * maxpos = 1.0
			TestCase<nbits, es> test;
			test.a = sw::unum::posit<nbits, es>(sw::unum::minpos_value<nbits, es>());
			test.b = sw::unum::posit<nbits, es>(sw::unum::minpos_value<nbits, es>());
			test_cases.push_back(test);
			test.b = sw::unum::posit<nbits, es>(sw::unum::maxpos_value<nbits, es>());
			test_cases.push_back(test);
			test.a = sw::unum::posit<nbits, es>(sw::unum::maxpos_value<nbits, es>());
			test_cases.push_back(test);
			test.a = sw::unum::posit<nbits, es>(sw::unum::minpos_value<nbits, es>());
			for (int i = 0; i < int(1) << (es + 2); i++) {
				test.a++; test.b--;
				test_cases.push_back(test);
			}
			test.a = 0.5;
			test.b = 2.0;
			test_cases.push_back(test);
			for (int i = 0; i < int(1) << enumeration; i++) {
				test.a--; test.b++;
				test_cases.push_back(test);
			}

			// execute and output the test vector
			std::cout << "posit<" << nbits << "," << es << ">" << std::endl;
			std::cout << std::setw(nbits) << "Operand A  " << " * " << std::setw(nbits) << "Operand B  " << " = " << std::setw(nbits) << "Golden Reference  " << " " << std::setw(nbits / 4) << "HEX " << std::endl;

			sw::unum::posit<nbits, es> pa, pb, pmul, pref;
			double da, db;
			for (size_t i = 0; i < test_cases.size(); i++) {
				pa = test_cases[i].a;
				da = double(pa);
				pb = test_cases[i].b;
				db = double(pb);
				pmul = pa * pb;
				pref = da * db;
				if (pmul != pref) {
					if (bReportIndividualTestCases) ReportBinaryArithmeticError("FAIL", "*", pa, pb, pref, pmul);
					nrOfFailedTests++;
				}
				else {
					// if (bReportIndividualTestCases) ReportBinaryArithmeticSuccess("PASS", "*", pa, pb, pref, pmul);
				}
				std::cout << pa.get() << " * " << pb.get() << " = " << pref.get() << " " << sw::unum::to_hex(pref.get()) << std::endl;
			}
			return nrOfFailedTests;
		}


		template<size_t nbits, size_t es>
		int SmokeTestDivision(std::string tag, bool bReportIndividualTestCases) {
			static_assert(nbits >= 16, "Use exhaustive testing for posits smaller than 16");
			static_assert(nbits <= 64, "TODO: smoke test algorithm only works for nbits <= 64");

			constexpr size_t fbits = nbits - 3 - es;
			constexpr size_t enumeration = fbits > 5 ? 5 : fbits;

			int nrOfFailedTests = 0;

			std::vector< TestCase<nbits, es> > test_cases;
			// minpos + minpos = minpos?
			TestCase<nbits, es> test;
			test.a = sw::unum::posit<nbits, es>(sw::unum::minpos_value<nbits, es>());
			test.b = sw::unum::posit<nbits, es>(sw::unum::minpos_value<nbits, es>());
			test_cases.push_back(test);
			for (int i = 0; i < int(1) << (es + 1); i++) {
				test.a++; test.b++;
				test_cases.push_back(test);
			}
			test.a = sw::unum::posit<nbits, es>(sw::unum::maxpos_value<nbits, es>());
			test.b = sw::unum::posit<nbits, es>(sw::unum::maxpos_value<nbits, es>());
			test_cases.push_back(test);
			for (int i = 0; i < int(1) << (es + 1); i++) {
				test.a--; test.b--;
				test_cases.push_back(test);
			}
			test.a = 1.0;
			test.b = 1.0;
			test_cases.push_back(test);
			for (int i = 0; i < int(1) << enumeration; i++) {
				test.a--; test.b++;
				test_cases.push_back(test);
			}

			// execute and output the test vector
			std::cout << "posit<" << nbits << "," << es << ">" << std::endl;
			std::cout << std::setw(nbits) << "Operand A  " << " / " << std::setw(nbits) << "Operand B  " << " = " << std::setw(nbits) << "Golden Reference  " << " " << std::setw(nbits / 4) << "HEX " << std::endl;

			sw::unum::posit<nbits, es> pa, pb, pdiv, pref;
			double da, db;
			for (size_t i = 0; i < test_cases.size(); i++) {
				pa = test_cases[i].a;
				da = double(pa);
				pb = test_cases[i].b;
				db = double(pb);
				pdiv = pa / pb;
				pref = da / db;
				if (pdiv != pref) {
					if (bReportIndividualTestCases) ReportBinaryArithmeticError("FAIL", "+", pa, pb, pref, pdiv);
					nrOfFailedTests++;
				}
				else {
					//if (bReportIndividualTestCases) ReportBinaryArithmeticSuccess("PASS", "+", pa, pb, pref, pdiv);
				}
				std::cout << pa.get() << " / " << pb.get() << " = " << pref.get() << " " << sw::unum::to_hex(pref.get()) << std::endl;
			}
			return nrOfFailedTests;
		}



		template<size_t nbits, size_t es>
		int Compare(double input, const sw::unum::posit<nbits, es>& presult, double reference, bool bReportIndividualTestCases) {
			int fail = 0;
			double result = double(presult);
			if (fabs(result - reference) > 0.000000001) {
				fail++;
				if (bReportIndividualTestCases)	ReportConversionError("FAIL", "=", input, reference, presult);
			}

			//if (bReportIndividualTestCases) ReportConversionSuccess("PASS", "=", input, reference, presult);
			// report test cases: input operand -> posit bit pattern
			sw::unum::value<std::numeric_limits< double >::digits> vi(input), vr(reference);
			std::cout.precision(std::numeric_limits< double >::max_digits10);
			std::cout << input << ", " << sw::unum::to_binary(input) << ", " << components(vi) << "\n" << reference << ", " << sw::unum::to_binary(reference) << ", " << components(vr) << "," << presult.get() << std::endl;

			return fail;
		}


		template<size_t nbits, size_t es>
		int SmokeTestConversion(std::string tag, bool bReportIndividualTestCases) {
			//static_assert(nbits >= 16, "Use exhaustive testing for posits smaller than 16");
			static_assert(nbits < 64, "TODO: smoke test algorithm only works for nbits < 64");
			// we are going to generate a test set that consists of all edge case posit configs and their midpoints
			// we do this by enumerating a posit that is 1-bit larger than the test posit configuration

			// there are four quadrants, each with two endpoints
			// south-east  -> [minpos -   1.0)
			// north-east  -> (1.0    -   maxpos)
			// north-west  -> [-maxpos - -1.0)
			// south-west  -> (-1.0    - -minpos)

			// on each minpos/maxpos side there are 2^(es+1) patterns that carry special rounding behavior
			// es = 0:   0/minpos                            ->  2 special cases
			// es = 1:   0/minpos, 2 exponent configs        ->  4 special cases
			// es = 2:   0/minpos, 2, 4 exponent configs     ->  8 special cases
			// es = 3:   0/minpos, 2, 4, 8 exponent configs  -> 16 special cases
			// es = 4:   0/minpos, 2, 4, 8, 16 exp configs   -> 32 special cases
			// -> 2^(es+1) special cases
			//
			// plus the region around 1 that puts the most pressure on the conversion algorithm's precision
			// --1, 1, and 1++, so three extra cases per half.
			// Because we need to recognize the -minpos case, which happens to be all 1's, and is the last
			// test case in exhaustive testing, we need to have that test case end up in the last entry
			// of the test case array.
			constexpr size_t single_quadrant_cases = size_t(1) << (es + 2);
			constexpr size_t cases_around_plusminus_one = 6;
			constexpr size_t cases = cases_around_plusminus_one + 4 * single_quadrant_cases;
			const int64_t STATE_SPACE = uint64_t(1) << (nbits + 1);
			const int64_t HALF = uint64_t(1) << nbits;  // <--- raw bit value of infinite for a posit<nbits+1,es>
														// generate the special patterns
			unsigned long long test_patterns[cases];
			// first patterns around +/- 1
			std::bitset<nbits + 1> raw_bits;
			sw::unum::posit<nbits + 1, es> p;  // need to generate them in the context of the posit that is nbits+1
											   // around 1.0
			p = 1.0; p--; raw_bits = p.get();
			std::cout << "raw bits for  1.0-eps: " << raw_bits << " ull " << raw_bits.to_ullong() << std::endl;
			test_patterns[0] = raw_bits.to_ullong();
			p = 1.0; raw_bits = p.get();
			std::cout << "raw bits for  1.00000: " << raw_bits << " ull " << raw_bits.to_ullong() << std::endl;
			test_patterns[1] = raw_bits.to_ullong();
			p = 1.0; p++; raw_bits = p.get();
			std::cout << "raw bits for  1.0+eps: " << raw_bits << " ull " << raw_bits.to_ullong() << std::endl;
			test_patterns[2] = raw_bits.to_ullong();
			// around -1.0
			p = -1.0; p--; raw_bits = p.get();
			std::cout << "raw bits for -1.0-eps: " << raw_bits << " ull " << raw_bits.to_ullong() << " posit : " << p << std::endl;
			test_patterns[3] = raw_bits.to_ullong();
			p = -1.0; raw_bits = p.get();
			std::cout << "raw bits for -1.00000: " << raw_bits << " ull " << raw_bits.to_ullong() << " posit : " << p << std::endl;
			test_patterns[4] = raw_bits.to_ullong();
			p = -1.0; p++; raw_bits = p.get();
			std::cout << "raw bits for -1.0+eps: " << raw_bits << " ull " << raw_bits.to_ullong() << " posit : " << p << std::endl;
			test_patterns[5] = raw_bits.to_ullong();

			// second are the exponential ranges from/to minpos/maxpos
			// south-east region
			int index = 6;
			for (int64_t i = 0; i < single_quadrant_cases; i++) {
				test_patterns[index++] = i;
			}
			// north-east region
			for (int64_t i = 0; i < single_quadrant_cases; i++) {
				test_patterns[index++] = HALF - single_quadrant_cases + i;
			}
			// north-west region
			for (int64_t i = 0; i < single_quadrant_cases; i++) {
				test_patterns[index++] = HALF + i;
			}
			// south-east region
			for (int64_t i = 0; i < single_quadrant_cases; i++) {
				test_patterns[index++] = STATE_SPACE - single_quadrant_cases + i;
			}

			const int64_t NR_TEST_CASES = cases_around_plusminus_one + 4 * single_quadrant_cases;

			sw::unum::posit<nbits + 1, es> pref, pprev, pnext;

			// execute and output the test vector
			std::cout << "posit<" << nbits << "," << es << ">" << std::endl;

			int nrOfFailedTests = 0;
			double minpos = sw::unum::minpos_value<nbits + 1, es>();
			double eps;
			double da, input;
			sw::unum::posit<nbits, es> pa;
			for (int64_t index = 0; index < NR_TEST_CASES; index++) {
				unsigned long long i = test_patterns[index];
				pref.set_raw_bits(i);
				std::cout << "Test case [" << index << "] = " << i << " b" << pref.get() << "  >>>>>>>>>>>>>>>  Reference Seed value: " << pref << std::endl;

				da = double(pref);
				if (i == 0) {
					eps = minpos / 2.0;
				}
				else {
					eps = da > 0 ? da * 1.0e-9 : da * -1.0e-9;
				}
				if (i % 2) {
					if (i == 1) {
						// special case of projecting to +minpos
						// even the -delta goes to +minpos
						input = da - eps;
						pa = input;
						pnext.set_raw_bits(i + 1);
						nrOfFailedTests += sw::qa::Compare(input, pa, (double)pnext, bReportIndividualTestCases);
						input = da + eps;
						pa = input;
						nrOfFailedTests += sw::qa::Compare(input, pa, (double)pnext, bReportIndividualTestCases);

					}
					else if (i == HALF - 1) {
						// special case of projecting to +maxpos
						input = da - eps;
						pa = input;
						pprev.set_raw_bits(HALF - 2);
						nrOfFailedTests += sw::qa::Compare(input, pa, (double)pprev, bReportIndividualTestCases);
					}
					else if (i == HALF + 1) {
						// special case of projecting to -maxpos
						input = da - eps;
						pa = input;
						pprev.set_raw_bits(HALF + 2);
						nrOfFailedTests += sw::qa::Compare(input, pa, (double)pprev, bReportIndividualTestCases);
					}
					else if (i == STATE_SPACE - 1) {
						// special case of projecting to -minpos
						// even the +delta goes to -minpos
						input = da - eps;
						pa = input;
						pprev.set_raw_bits(i - 1);
						nrOfFailedTests += sw::qa::Compare(input, pa, (double)pprev, bReportIndividualTestCases);
						input = da + eps;
						pa = input;
						nrOfFailedTests += sw::qa::Compare(input, pa, (double)pprev, bReportIndividualTestCases);
					}
					else {
						// for odd values, we are between posit values, so we create the round-up and round-down cases
						// round-down
						input = da - eps;
						pa = input;
						pprev.set_raw_bits(i - 1);
						nrOfFailedTests += sw::qa::Compare(input, pa, (double)pprev, bReportIndividualTestCases);
						// round-up
						input = da + eps;
						pa = input;
						pnext.set_raw_bits(i + 1);
						nrOfFailedTests += sw::qa::Compare(input, pa, (double)pnext, bReportIndividualTestCases);
					}
				}
				else {
					// for the even values, we generate the round-to-actual cases
					if (i == 0) {
						// special case of projecting to +minpos
						input = da + eps;
						pa = input;
						pnext.set_raw_bits(i + 2);
						nrOfFailedTests += sw::qa::Compare(input, pa, (double)pnext, bReportIndividualTestCases);
					}
					else if (i == STATE_SPACE - 2) {
						// special case of projecting to -minpos
						input = da - eps;
						pa = input;
						pprev.set_raw_bits(STATE_SPACE - 2);
						nrOfFailedTests += sw::qa::Compare(input, pa, (double)pprev, bReportIndividualTestCases);
					}
					else {
						// round-up
						input = da - eps;
						pa = input;
						nrOfFailedTests += sw::qa::Compare(input, pa, da, bReportIndividualTestCases);
						// round-down
						input = da + eps;
						pa = input;
						nrOfFailedTests += sw::qa::Compare(input, pa, da, bReportIndividualTestCases);
					}
				}
			}
			return nrOfFailedTests;
		}

		//////////////////////////////////// RANDOMIZED TEST SUITE FOR BINARY OPERATORS ////////////////////////

		// for testing posit configs that are > 14-15, we need a more efficient approach.
		// One simple, brute force approach is to generate randoms.
		// A more white box approach is to focus on the testcases 
		// where something special happens in the posit arithmetic, such as rounding.

		// operation opcodes
		const int OPCODE_NOP = 0;
		const int OPCODE_ADD = 1;
		const int OPCODE_SUB = 2;
		const int OPCODE_MUL = 3;
		const int OPCODE_DIV = 4;
		const int OPCODE_RAN = 5;

		template<size_t nbits, size_t es, typename Ty>
		void execute(int opcode, Ty a, Ty b, sw::unum::posit<nbits, es>& preference, const sw::unum::posit<nbits, es>& pa, const sw::unum::posit<nbits, es>& pb, sw::unum::posit<nbits, es>& presult) {
			Ty reference;
			switch (opcode) {
			default:
			case OPCODE_NOP:
				preference.setToZero();
				presult.setToZero();
				return;
			case OPCODE_ADD:
				presult = pa + pb;
				reference = a + b;
				break;
			case OPCODE_SUB:
				presult = pa - pb;
				reference = a - b;
				break;
			case OPCODE_MUL:
				presult = pa * pb;
				reference = a * b;
				break;
			case OPCODE_DIV:
				presult = pa / pb;
				reference = a / b;
				break;
			}
			preference = reference;
		}

		// generate a random set of operands to test the binary operators for a posit configuration
		// Basic design is that we generate nrOfRandom posit values and store them in an operand array.
		// We will then execute the binary operator nrOfRandom combinations.
		template<size_t nbits, size_t es>
		int SmokeTestRandoms(std::string tag, int opcode, unsigned int nrOfRandoms) {
			static_assert(nbits <= 64, "SmokeTestRandoms only works for nbits <= 64");
			int max_digits10 = std::numeric_limits<double>::max_digits10;
			const size_t SIZE_STATE_SPACE = nrOfRandoms;
			int nrOfFailedTests = 0;
			sw::unum::posit<nbits, es> pa, pb, presult, pref;

			if (opcode == OPCODE_RAN) {
				// TODO: generate a random operator
			}
			std::string operation_string;
			switch (opcode) {
			default:
			case OPCODE_NOP:
				operation_string = "nop";
				break;
			case OPCODE_ADD:
				operation_string = "+";
				break;
			case OPCODE_SUB:
				operation_string = "-";
				break;
			case OPCODE_MUL:
				operation_string = "*";
				break;
			case OPCODE_DIV:
				operation_string = "/";
				break;
			}
			// generate the full state space set of valid posit values
			std::random_device rd;     //Get a random seed from the OS entropy device, or whatever
			std::mt19937_64 eng(rd()); //Use the 64-bit Mersenne Twister 19937 generator and seed it with entropy.
									   //Define the distribution, by default it goes from 0 to MAX(unsigned long long)
			std::uniform_int_distribution<unsigned long long> uniform;
#if VERBOSE
			std::cout << "Size of float     type is: " << 8*sizeof(float) << "bits" << std::endl;
			std::cout << "Size of double    type is: " << 8*sizeof(double) << "bits" << std::endl;
			std::cout << "Size of quadruple type is: " << 8*sizeof(long double) << "bits" << std::endl;
#endif

			if (nbits - es - 1 > 52) {
				std::vector<long double> operand_values(SIZE_STATE_SPACE);
				// inject minpos/maxpos and -minpos/-maxpos in the samples
				presult = 1.0;
				operand_values[0] = (long double)presult;
				presult = -1.0;
				operand_values[1] = (long double)presult;
				presult.set_raw_bits(1);
				operand_values[2] = (long double)presult;
				presult--; presult--;
				operand_values[3] = (long double)presult;
				presult.setToNaR();
				presult++;
				operand_values[4] = (long double)presult;
				presult.setToNaR();
				presult++;
				operand_values[5] = (long double)presult;
				for (uint32_t i = 6; i < SIZE_STATE_SPACE; i++) {
					presult.set_raw_bits(uniform(eng));  // take the bottom nbits bits as posit encoding: works for nbits<=64
					operand_values[i] = (long double)presult;
				}

				/*
				for (uint32_t i = 0; i < SIZE_STATE_SPACE; i++) {
					std::cout << std::setprecision(max_digits10) << operand_values[i] << std::endl;
				}
				*/

#if VERBOSE
				// execute and output the test vector
				std::cout << "posit<" << nbits << "," << es << ">" << std::endl;
				std::cout << std::setw(nbits) << "Operand A  " << " " << operation_string << " " << std::setw(nbits) << "Operand B  " << " = " << std::setw(nbits) << "Golden Reference  " << " " << std::setw(nbits / 4) << "HEX " << std::endl;
#endif

				long double qa, qb;
				unsigned int ia, ib;  // random indices for picking operands to test
				for (unsigned int i = 1; i < nrOfRandoms; i++) {
					ia = (unsigned int)(uniform(eng) % SIZE_STATE_SPACE);
					qa = operand_values[ia];
					pa = qa;
					ib = (unsigned int)(uniform(eng) % SIZE_STATE_SPACE);
					qb = operand_values[ib];
					pb = qb;

					sw::qa::execute<nbits,es,double>(opcode, qa, qb, pref, pa, pb, presult);
					if (presult != pref) {
						nrOfFailedTests++;
						ReportBinaryArithmeticErrorInBinary("FAIL", operation_string, pa, pb, pref, presult);
					}
					else {
						//if (bReportIndividualTestCases) ReportBinaryArithmeticSuccess("PASS", operation_string, pa, pb, preference, presult);
					}
					//std::cout << pa.get() << " " << operation_string << " " << pb.get() << " = " << pref.get() << " " << sw::unum::to_hex(pref.get()) << std::endl;
					std::cout << hex << setw(8) << pa.get().to_ullong() << " " << setw(8) << pb.get().to_ullong() << " " << setw(8) << pref.get().to_ullong() << std::endl;
				}
			}
			else {
				std::vector<double> operand_values(SIZE_STATE_SPACE);
				// inject minpos/maxpos and -minpos/-maxpos in the samples
				presult = 1.0;
				operand_values[0] = (long double)presult;
				presult = -1.0;
				operand_values[1] = (long double)presult;
				presult.set_raw_bits(1);
				operand_values[2] = (long double)presult;
				presult--; presult--;
				operand_values[3] = (long double)presult;
				presult.setToNaR();
				presult++;
				operand_values[4] = (long double)presult;
				presult.setToNaR();
				presult++;
				operand_values[5] = (long double)presult;
				for (uint32_t i = 6; i < SIZE_STATE_SPACE; i++) {
					presult.set_raw_bits(uniform(eng));  // take the bottom nbits bits as posit encoding: works for nbits<=64
					operand_values[i] = (long double)presult;
				}

				/*
				for (uint32_t i = 0; i < SIZE_STATE_SPACE; i++) {
				std::cout << std::setprecision(max_digits10) << operand_values[i] << std::endl;
				}
				*/

#if VERBOSE
				// execute and output the test vector
				std::cout << "posit<" << nbits << "," << es << ">" << std::endl;
				std::cout << std::setw(nbits) << "Operand A  " << " " << operation_string <<  " " << std::setw(nbits) << "Operand B  " << " = " << std::setw(nbits) << "Golden Reference  " << " " << std::setw(nbits / 4) << "HEX " << std::endl;
#endif

				double da, db;
				unsigned int ia, ib;  // random indices for picking operands to test
				for (unsigned int i = 1; i < nrOfRandoms; i++) {
					ia = (unsigned int)(uniform(eng) % SIZE_STATE_SPACE);
					da = operand_values[ia];
					pa = da;
					ib = (unsigned int)(uniform(eng) % SIZE_STATE_SPACE);
					db = operand_values[ib];
					pb = db;
					sw::qa::execute<nbits,es,double>(opcode, da, db, pref, pa, pb, presult);
					if (presult != pref) {
						nrOfFailedTests++;
						ReportBinaryArithmeticErrorInBinary("FAIL", operation_string, pa, pb, pref, presult);
					}
					else {
						//if (bReportIndividualTestCases) ReportBinaryArithmeticSuccess("PASS", operation_string, pa, pb, preference, presult);
					}
					//std::cout << pa.get() << " " << operation_string << " " << pb.get() << " = " << pref.get() << " " << sw::unum::to_hex(pref.get()) << std::endl;
					std::cout << hex << setw(8) << pa.get().to_ullong() << " " << setw(8) << pb.get().to_ullong() << " " << setw(8) << pref.get().to_ullong() << std::endl;
				}
			}
			return nrOfFailedTests;
		}


	}; // namespace qa
};  // namespace sw
