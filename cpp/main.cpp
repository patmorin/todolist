#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <iostream>
#include <string>
#include <algorithm>
#include <iterator>
using namespace std;

#include <unistd.h>

#include "ScapegoatTree.h"
#include "SkiplistSSet.h"
#include "Treap.h"
#include "SplayTree.h"
#include "RedBlackTree.h"
#include "TodoList.h"
#include "LinkedTodoList.h"
#include "TodoList2.h"


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

// Our main speed testing routine
template<class Dict>
void build_and_search(Dict &d, const char *name, size_t n,
		int (*gen_add)(size_t, size_t), int (*gen_search)(size_t, size_t)) {
	static int summer;
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

	Integer::resetComparisons();
	long sum = 0;
	start = clock();
	for (size_t i = 0; i < 5*n; i++)
		sum += (int)d.find(gen_search(i, n));
	stop = clock();

	elapsed = ((double)(stop-start))/CLOCKS_PER_SEC;
	avg = ((double)Integer::getComparisons()) / (5*n);
	c = avg * log(2) / log(d.size());

	cout << name << " FIND " << n << " " << elapsed
			<< " " <<  Integer::getComparisons()
			<< " " << c << endl;

	summer += sum; // to make sure this isn't optimized away
}


void test_suite(size_t n, int (*gen_data)(size_t, size_t),
		int (*gen_search)(size_t, size_t)) {
	cout << "Structure Operation n time #comparisons c" << endl;
	{
		fastws::LinkedTodoList<Integer> tdl(.12, NULL, 0);
		build_and_search(tdl, "LinkedTodoList", n, gen_data, gen_search);
		// cout << tdl;
	}
	{
		fastws::TodoList<Integer> tdl(.12, NULL, 0);
		build_and_search(tdl, "TodoList", n, gen_data, gen_search);
		// cout << tdl;
	}
	{
		ods::RedBlackTree1<Integer> rbt;
		build_and_search(rbt, "RedBlackTree", n, gen_data, gen_search);
		int tpl = rbt.tpl();
		cout << "I: total path length = " << tpl << endl;
		double apl = ((double)tpl) / rbt.size();
		cout << "I: average path length = " << apl << endl;
		cout << "I: c_average = " << apl * log(2) / log(n) << endl;
		cout << "#red nodes = " << rbt.reds() << endl;
		int mpl = rbt.mpl();
		cout << "I: maximum path length = " << mpl << endl;
		cout << "I: c_max = " << log(2) * mpl / log(n) << endl;
	}
	{
		ods::Treap1<Integer> t;
		build_and_search(t, "Treap", n,	gen_data, gen_search);
		int tpl = t.tpl();
		cout << "I: total path length = " << tpl << endl;
		double apl = ((double)tpl) / t.size();
		cout << "I: average path length = " << apl << endl;
		cout << "I: c_average = " << apl * log(2) / log(n) << endl;
		int mpl = t.mpl();
		cout << "I: maximum path length = " << mpl << endl;
		cout << "I: c_max = " << log(2) * mpl / log(n) << endl;
	}
	{
		ods::SkiplistSSet<Integer> sl;
		build_and_search(sl, "Skiplist", n, gen_data, gen_search);
	}

}

// Compare the results of performing the same operations on two dictionaries
template<class Dict1, class Dict2>
void test_dicts(Dict1 &d1, Dict2 &d2, int n) {
	srand(1);
	for (int i = 0; i < n; i++) {
		int x = rand() % (5*n);
		assert(d1.add(x) == d2.add(x));
	}

	for (int i = 0; i < 5*n; i++) {
		int x = rand() % (5*(n+1))-2;
		assert(d1.find(x) == d2.find(x));
	}
}


void sanity_tests(size_t n) {
	{
		fastws::TodoList2<int> tdl2;
		ods::Treap1<int> t;
		test_dicts(tdl2, t, n);
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
		fastws::TodoList2<int> tdl2;
		test_dicts(rbt, tdl2, n);
	}
}

void usage_error(const char *name) {
	cerr << "Usage error: you're doing it wrong" << endl;
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
			sanity_tests(n);
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
				fastws::TodoList<Integer> tdl(epsilon);
				build_and_search(tdl, "TodoList", n, gen_data, gen_search);
		} else if (strcmp(argv[i], "-todolist2") == 0) {
				fastws::TodoList2<Integer> tdl2(epsilon);
				build_and_search(tdl2, "TodoList2", n, gen_data, gen_search);
		} else if (strcmp(argv[i], "-linkedtodolist") == 0) {
			fastws::LinkedTodoList<Integer> ltdl(epsilon);
			build_and_search(ltdl, "LinkedTodoList", n, rand_data, rand_search);
		} else {
			usage_error(argv[0]);
		}
	}
	return 0;
}

