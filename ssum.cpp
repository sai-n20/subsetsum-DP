

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

struct ssum_elem {
	unsigned int x;
	std::string name;
};

class ssum_instance {
	unsigned int target = 0;
	std::vector<std::vector<bool>> feasible;
	// feasible[i][x] = TRUE if there is a subset
	//  of a[0..i] adding up to x; FALSE otherwise
	//  (once instacne has been solved)
	//
	int done = false; // flag indicating if dp has been run or
					  //   not on this instance

public:
	std::vector<ssum_elem> elems;

	// Function:  read_elems
	// Description:  reads elements from standard-input;
	//   Format:  sequence of <number> <name> pairs where
	//      <number> is non-negative int and
	//      <name> is a string associated with element

	void read_elems(std::istream& stream) {
		ssum_elem e;
		elems.clear();
		// while(std::cin >> e.x && std::cin >> e.name) {
		while (stream >> e.x && stream >> e.name) {
			elems.push_back(e);
		}
		done = false;
	}

	unsigned long long int make_count_table(unsigned int tgt, unsigned int num) {

		std::vector<std::vector<unsigned long long int>> distinctSubsets;
		distinctSubsets = std::vector<std::vector<unsigned long long int>>(num, std::vector<unsigned long long int>(tgt + 1, 0));

		//Sum of 0 is possible with empty set {}
		for (int i = 0; i < num; i++) {
			distinctSubsets[i][0] = 1;
		}

		for (int x = 0; x <= tgt; x++) {
			if (elems.at(0).x == x)
				distinctSubsets[0][x] = 1;
		}
		for (int i = 1; i < num; i++) {
			for (int x = 1; x <= tgt; x++) {

				unsigned long long int includingCurrentValue = 0;
				unsigned long long int excludingCurrentValue = 0;

				if (elems.at(i).x <= x) {
					includingCurrentValue = distinctSubsets[i - 1][x - elems.at(i).x];
				}
				excludingCurrentValue = distinctSubsets[i - 1][x];
				distinctSubsets[i][x] = includingCurrentValue + excludingCurrentValue;
			}
		}
		return distinctSubsets[num - 1][tgt];
	}


	unsigned int smallest_size(unsigned int tgt, unsigned int num) {

		std::vector<std::vector<unsigned int>> minCount;
		minCount = std::vector<std::vector<unsigned int>>(num, std::vector<unsigned int>(tgt + 1, 42949672));

		//To solve this trivially incase target matches input array amount
		for (int i = 0; i < num; i++) {
			if (elems.at(i).x == tgt) { return 1; }
			minCount[i][0] = 0;
		}
		for (int x = 1; x <= tgt; x++) {
			if (elems[0].x == x)
				minCount[0][x] = 1;
		}

		for (int i = 1; i < num; i++) {
			for (int x = 1; x <= tgt; x++) {

				unsigned int includingCurrentValue = 42949672;
				unsigned int excludingCurrentValue = 42949672;

				if (i > 0) {
					excludingCurrentValue = minCount[i - 1][x];
				}

				if (elems[i].x <= x) {
					includingCurrentValue = 1 + minCount[i][x - elems[i].x];
				}

				minCount[i][x] = std::min(includingCurrentValue, excludingCurrentValue);
			}
		}
		return minCount[num - 1][tgt];
	}

	unsigned int distinctSmallestSubsetCounter = 0;


	// Function:  solve
	// Desc:  populates dynamic programming table of
	//    calling object for specified target sum.
	//    Returns true/false depending on whether the
	//    target sum is achievalble or not.
	//    Table remains intact after call.
	//    Object can be reused for alternative target sums.

	bool solve(unsigned int tgt) {
		unsigned int n = elems.size();
		unsigned int i, x;

		if (target == tgt && done)
			return feasible[n - 1][tgt];

		target = tgt;
		feasible = std::vector<std::vector<bool>>(n, std::vector<bool>(target + 1, false));

		// leftmost column (column zero) is all TRUE because
		//    a target sum of zero is always acheivable (via the
		//    empty set).
		for (i = 0; i < n; i++)
			feasible[i][0] = true;

		for (x = 1; x <= target; x++) {
			if (elems[0].x == x)
				feasible[0][x] = true;
			// otherwise, feasible[0][x] remains false
		}
		for (i = 1; i < n; i++) {
			for (x = 1; x <= tgt; x++) {
				if (feasible[i - 1][x])
					feasible[i][x] = true;
				else if (x >= elems[i].x && feasible[i - 1][x - elems[i].x])
					feasible[i][x] = true;
			}
			// otherwise, feasible[0][x] remains false
		}
		done = true;
		return feasible[n - 1][target];
	}
}; // end class

/**
* usage:  ssum  <target> < <input-file>
*
*
* input file format:
*
*     sequence of non-negative-int, string pairs
*
*     example:

	12 alice
	9  bob
	22 cathy
	12 doug

* such a file specifies a collection of 4 integers: 12, 9, 22, 12
* "named" alice, bob, cathy and doug.
*/
int main(int argc, char* argv[]) {
	unsigned int test = 0;
	unsigned int target;
	ssum_instance ssi;
	if (argc != 2) {
		fprintf(stderr, "One cmd-line arg expected: target sum\n");
		return 0;
	}
	if (sscanf(argv[1], "%u", &target) != 1) {
		fprintf(stderr, "Bad argument '%s'\n", argv[1]);
		fprintf(stderr, "   Expected unsigned integer\n");
		return 0;
	}

	ssi.read_elems(std::cin);
	if (ssi.solve(target)) {
		std::cout << "The target sum of " << target << " is FEASIBLE!";
		std::cout << "\n\nNumber of distinct solutions: " << ssi.make_count_table(target, ssi.elems.size());
		unsigned int smallSize = ssi.smallest_size(target, ssi.elems.size());
		std::cout << "\nSize of smallest subset: " << smallSize << "\n";
		std::cout << "\nAmount of smallest subsets: " << ssi.distinctSmallestSubsetCounter;
	}
	else {
		std::cout << "SORRY!  Apparently, the target sum of " << target << " is NOT achievable\n";
	}
}
