#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <iostream>
#include <string>
#include <algorithm>
#include <iterator>

#include <unistd.h>

#include "BinarySearchTree.h"
#include "ScapegoatTree.h"
#include "SkiplistSSet.h"
#include "Treap.h"
#include "SplayTree.h"
#include "RedBlackTree.h"
#include "TodoList.h"
#include "LinkedTodoList.h"
#include "TodoList2.h"
#include "TodoList3.h"
#include "TodoList4.h"

using namespace std;


// class Integer
// A silly class to use for counting comparisons and for simulating classes
// that have more expensive comparisons
class Integer {
protected:
	// used for statistics gathering
	static unsigned long dummy;
	static size_t comparisons;
	static size_t del;

	// the actual integer
	int data;

	void delay() {
		unsigned int tmp = 0;
		for (size_t i = 0; i < del; i++) {
			tmp = (tmp + data) % 733721;
			dummy += tmp;  // just to ensure this doesn't get optimized away
		}
		comparisons++;
	}
public:
	static void setDelay(size_t delay) {
		del = delay;
	}
	static size_t getComparisons() {
		return comparisons;
	}
	static void resetComparisons() {
		comparisons = 0;
	}
	Integer() {
		data = 0;
	}
	Integer(int i) {
		data = i;
	}
	Integer(const Integer &i) {
		data = i.data;
	}
	bool operator <(const Integer &other) {
		delay();
		return data < other.data;
	}
	bool operator >(const Integer &other) {
		// not delaying here means binary search trees are only charged once
		// for a three way comparison
		return data > other.data;
	}
	bool operator ==(const Integer &other) {
		return data == other.data;
	}
	void printOn(ostream &out) {
		out << data;
	}
	operator int() const {
		return data;
	}
};
size_t Integer::dummy, Integer::comparisons, Integer::del;

ostream& operator<<(ostream &out, Integer &ds) {
	ds.printOn(out);
	return out;
}


// A bunch of sequence generators that generate the i'th element in a sequence
// of length n
int rand_data(size_t i, size_t n) {
	return (rand() % (n*5));
}

int rand_search(size_t i, size_t n) {
	return rand() % (n*5) - 2;
}

int sequential_data(size_t i, size_t n) {
	return 5*i;
}

int requential_data(size_t i, size_t n) {
	return 5*(n-i-1);
}

int shuffle_data(size_t i, size_t n) {
	int sn = ceil(sqrt(n));
	return (i*sn + i/sn) % n;
}

template<class Dict>
void build(Dict &d, const char *name, size_t n,
		int (*gen_add)(size_t, size_t)) {
	srand(1);
	Integer::resetComparisons();

	clock_t start = clock();
	for (size_t i = 0; i < n; i++)
		d.add(gen_add(i, n));
	clock_t stop = clock();

	double elapsed = ((double)(stop-start))/CLOCKS_PER_SEC;
	double avg = ((double)Integer::getComparisons()) / n;
	double c = avg * log(2) / log(d.size());
	cout << name << " ADD " << n << " " << elapsed
			<< " " << Integer::getComparisons()
			<< " " << c << endl;
}

template<class Dict>
void search(Dict &d, const char *name, size_t n,
		int (*gen_search)(size_t, size_t)) {
	static int summer;

	Integer::resetComparisons();
	long sum = 0;
	clock_t start = clock();
	for (size_t i = 0; i < 5*n; i++)
		sum += (int)d.find(gen_search(i, n));
	clock_t stop = clock();

	double elapsed = ((double)(stop-start))/CLOCKS_PER_SEC;
	double avg = ((double)Integer::getComparisons()) / (5*n);
	double c = avg * log(2) / log(d.size());

	cout << name << " FIND " << n << " " << elapsed
			<< " " <<  Integer::getComparisons()
			<< " " << c << endl;

	summer += sum; // to make sure this isn't optimized away
}


// Our main speed testing routine
template<class Dict>
void build_and_search(Dict &d, const char *name, size_t n,
		int (*gen_add)(size_t, size_t), int (*gen_search)(size_t, size_t)) {
	build(d, name, n, gen_add);
	search(d, name, n, gen_search);
}

template<class Dict1, class Dict2>
void test_build(Dict1 &d1, Dict2 &d2, int n) {
	srand(1);
	for (int i = 0; i < n; i++) {
		int x = rand() % (5*n);
		assert(d1.add(x) == d2.add(x));
	}
}

template<class Dict1, class Dict2>
void test_search(Dict1 &d1, Dict2 &d2, int n) {
	srand(2);
	for (int i = 0; i < 5*n; i++) {
		int x = rand() % (5*(n+1))-2;
		assert(d1.find(x) == d2.find(x));
	}
}

// Compare the results of performing the same operations on two dictionaries
template<class Dict1, class Dict2>
void test_dicts(Dict1 &d1, Dict2 &d2, int n) {
	test_build(d1, d2, n);
	test_search(d1, d2, n);
}

// Return the index of the smallest value that is greater than or equal
// to x. If no such value exists, then return n0.
// Warning: n0 must be greater than 0.
template<class T>
T binarySearch(T x, T *data, size_t n) {
	T ans = (T)0;
	while (n > 0) {
		size_t m = n/2;
		if (x < data[m]) {
			ans = data[m];
			n = m; // recurse on data[0,...,m-1]
		} else if (x > data[m]) {
			data += m+1; // recurse on data[m+1,...n]
			n -= m+1;
		} else {
			return data[m];
		}
	}
	return ans;
}

template<class T>
class SortedArray {
protected:
	T *data;
	size_t n;
public:
	SortedArray(T *data0, size_t n0) : data(data0), n(n0) {	}
	int size() { return n; }
	T find(T x) { return binarySearch(x, data, n); }
};

void sanity_tests(size_t n) {
	{
		todolist::TodoList4<int> tdl4;
		todolist::TodoList3<int> tdl3;
		test_dicts(tdl4, tdl3, n);
	}
	{
		todolist::TodoList3<int> tdl3;
		ods::Treap1<int> t;
		test_dicts(tdl3, t, n);
	}
	{
		ods::Treap1<int> t;
		ods::SkiplistSSet<int> sl;
		test_dicts(t, sl, n);
	}
	{
		ods::SkiplistSSet<int> sl;
		ods::ScapegoatTree1<int> st;
		test_dicts(sl, st, n);
	}
	{
		ods::ScapegoatTree1<int> st;
		ods::RedBlackTree1<int> rbt;
		test_dicts(st, rbt, n);
	}
	{
		ods::RedBlackTree1<int> rbt;
		todolist::TodoList2<int> tdl2;
		test_dicts(rbt, tdl2, n);
	}
	{
		todolist::TodoList2<int> tdl2;
		todolist::TodoList4<int> tdl4;
		test_dicts(tdl2, tdl4, n);
	}
	{
		srand(1);
		int *data = new int[n];
		ods::Treap1<int> t;
		for (size_t i = 0; i < n; i++) {
			data[i] = rand() % (5*n);
			t.add(data[i]);
		}
		std::sort(data, data+n);
		size_t unique = 0;
		for (size_t i = 0; i < n; i++)
			if (i == 0 || data[i] > data[i-1])
				data[unique++] = data[i];
		ods::BinarySearchTree1<int> bst(data, unique);
		delete[] data;
		assert(t.size() == bst.size());
		test_search(t, bst, n);
	}
	{
		srand(1);
		int *data = new int[n];
		for (size_t i = 0; i < n; i++)
			data[i] = rand() % (5*n);
		std::sort(data, data+n);
		size_t unique = 0;
		for (size_t i = 0; i < n; i++)
			if (i == 0 || data[i] > data[i-1])
				data[unique++] = data[i];
		SortedArray<int> sa(data, unique);
		ods::BinarySearchTree1<int> bst(data, unique);
		test_search(sa, bst, n);
		delete[] data;
	}

}

void usage_error(const char *name) {
	cerr << "Usage: " << name << " <args>+" << endl
		<< "Possible values of <args> are:" << endl
		<< " -<n>        : Do tests on inputs of size n" << endl
		<< " -eps=<eps>  : Set the value of epsilon for todolists and"
					  << " scapegoat trees" << endl
		<< " -sanity     : runs sanity tests to ensure correctness" << endl
		<< " -sequential : use sequential insertions (default is random)"
		<< endl
		<< " -requential : use reverse sequential insertions (default is random)"
		<< endl
		<< " -shuffled   : use shuffled insertions (sqrt(n) groups)" << endl
		<< " -bst        : test static balanced binary search tree" << endl
		<< " -redblack   : test red-black tree (Guibas and Sedgewick)" << endl
		<< " -treap      : test treap (Aragon and Seidel, Vuillemin)" << endl
		<< " -skiplist   : test skip list (Pugh)" << endl
		<< " -scapegoat  : test scapegoat tree (Galperin and Rivest, Andersson)"
		<< endl
		<< " -todolist   : test todolist (version 1)" << endl
		<< " -todolist2  : test todolist (version 2)" << endl
		<< " -todolist3  : test todolist (version 3)" << endl
		<< " -todolist4  : test todolist (version 4)" << endl
		<< " -linkedtodolist : test linked todolist" << endl << endl
		<< "Consult README for a discussion of different todolist versions."
		<< endl << endl
		<< "Example: " << name << " -1000000 -todolist4 -redblack" << endl
	    << " will test a todolist (version 4) and a red-black tree by performing"
		<< endl << " 1000000 random insertions and 5000000 random searches." << endl;
	exit(-1);
}


int main(int argc, char **argv) {
	if (argc < 2)
		usage_error(argv[0]);

	size_t n = 100000;
	int (*gen_data)(size_t, size_t) = rand_data;
	int (*gen_search)(size_t, size_t) = rand_search;
	double epsilon = .2;
	for (int i = 1; i < argc; i++) {
		if (strlen(argv[i]) > 0 && argv[i][0] == '-' && isdigit(argv[i][1])) {
			n = atoi(argv[i]+1);
		} else if (strncmp(argv[i], "-eps=", 5) == 0) {
			epsilon = strtof(argv[i]+5, NULL);
			cout << "I: epsilon = " << epsilon
					<< " for ScapegoatTree, TodoList, and LinkedTodoList" << endl;
		} else if (strncmp(argv[i], "-delay=", 7) == 0) {
			int delay = atoi(argv[i] + 7);
			Integer::setDelay(delay);
			cout << "I: comparison delay set to " << delay << endl;
		} else if (strcmp(argv[i], "-sanity") == 0) {
			cout << "I: Doing sanity tests...";
			cout.flush();
			sanity_tests(n); // FIXME: reset to n when done testing
			cout << "done" << endl;
		} else if (strcmp(argv[i], "-sequential") == 0) {
			cout << "I: Using sequential data" << endl;
			gen_data = sequential_data;
		} else if (strcmp(argv[i], "-requential") == 0) {
			cout << "I: Using reversed sequential data" << endl;
			gen_data = requential_data;
		} else if (strcmp(argv[i], "-shuffled") == 0) {
			cout << "I: Using shuffled data" << endl;
			gen_data = requential_data;
		} else if (strcmp(argv[i], "-bst") == 0) {
			Integer *data = new Integer[n];
			srand(1);
			for (size_t i = 0; i < n; i++)
				data[i] = gen_data(i, n);
			std::sort(data, data+n);
			size_t unique = 0;
			for (size_t i = 0; i < n; i++)
				if (i == 0 || data[i] > data[i-1])
					data[unique++] = data[i];
			SortedArray<Integer> sa(data, unique);
			search(sa, "SortedArray", n, gen_search);
			ods::BinarySearchTree1<Integer> bst(data, unique);
			delete[] data;
			search(bst, "BinarySearchTree", n, gen_search);
		} else if (strcmp(argv[i], "-redblack") == 0) {
			ods::RedBlackTree1<Integer> rbt;
			build_and_search(rbt, "RedBlackTree", n, gen_data, gen_search);
		} else if (strcmp(argv[i], "-treap") == 0) {
			ods::Treap1<Integer> t;
			build_and_search(t, "Treap", n, gen_data, gen_search);
		} else if (strcmp(argv[i], "-skiplist") == 0) {
			ods::SkiplistSSet<Integer> sl;
			build_and_search(sl, "Skiplist", n, gen_data, gen_search);
		} else if (strcmp(argv[i], "-scapegoat") == 0) {
			ods::ScapegoatTree1<Integer> st(1./(2.-epsilon));
			build_and_search(st, "ScapegoatTree", n, gen_data, gen_search);
		} else if (strcmp(argv[i], "-todolist") == 0) {
				todolist::TodoList<Integer> tdl(epsilon);
				build_and_search(tdl, "TodoList", n, gen_data, gen_search);
		} else if (strcmp(argv[i], "-todolist2") == 0) {
				todolist::TodoList2<Integer> tdl2(epsilon);
				build_and_search(tdl2, "TodoList2", n, gen_data, gen_search);
		} else if (strcmp(argv[i], "-todolist3") == 0) {
				todolist::TodoList3<Integer> tdl3(epsilon);
				build_and_search(tdl3, "TodoList3", n, gen_data, gen_search);
		} else if (strcmp(argv[i], "-todolist4") == 0) {
				todolist::TodoList4<Integer> tdl4(epsilon);
				build_and_search(tdl4, "TodoList4", n, gen_data, gen_search);
		} else if (strcmp(argv[i], "-linkedtodolist") == 0) {
			todolist::LinkedTodoList<Integer> ltdl(epsilon);
			build_and_search(ltdl, "LinkedTodoList", n, rand_data, rand_search);
		} else {
			usage_error(argv[0]);
		}
	}
	return 0;
}

