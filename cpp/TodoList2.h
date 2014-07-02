/**
 * (c) 2014 Pat Morin, Released under a CC BY 3.0 License:
 *     https://creativecommons.org/licenses/by/3.0/
 *
 * TodoList2.h : A top-down skiplist
 *
 * A TodoList variant that tries to minimize memory accesses by storing a
 * the key value associated with each next pointer.
 */
#ifndef FASTWS_TODOLIST2_H_
#define FASTWS_TODOLIST2_H_

#include <cmath>
#include <cstring>
#include <cstdlib>
#include <climits>
#include <cassert>

#include <iostream>
using namespace std;

namespace fastws {

/**
 * A dictionary with the working-set property.
 */
template<class T>
class TodoList2 {
protected:
	struct NP;

	struct Node;

	struct NX {
		Node *next;
		T xnext;
	};

	struct Node {
		T x;          // data
		NX nx[]; // a stack of next pointers
	};

	int h;    // there are k+1 lists numbered 0,...,k
	int *n;   // n[i] is the size of the i'th list
	Node *sentinel; // sentinel-next[i] is the first element of list i

	// parameters used to determine lists sizes
	double eps;
	int n0max;
	int *a;

	// FIXME: for profiling information
	int *rebuild_freqs;

	void init(T *data, int n);
	void rebuild();
	void rebuild(int i);

	void sanity();

	Node *newNode();
	void deleteNode(Node *u);

public:
	TodoList2(double eps0 = .3, T *data = NULL, int n0 = 0);
	virtual ~TodoList2();
	T find(T x);
	bool add(T x);
	int size() {
		return n[h];
	}

	void printOn(std::ostream &out);
};

template<class T>
TodoList2<T>::TodoList2(double eps0, T *data, int n0) {
	eps = eps0;

	int kmax = 100; // FIXME: potential limitation here
	rebuild_freqs = new int[kmax+1]();
	double base_a = 2.0-eps;
	a = new int[kmax+1];
	for (int i = 0; i <= kmax; i++) {
		a[i] = pow(base_a, i);
		// cout << "a[" << i << "]=" << a[i] << endl;
	}

	init(data, n0);
}

template<class T>
void TodoList2<T>::init(T *data, int n0) {

	// Compute critical values depending on epsilon and n
	n0max = ceil(2. / eps);
	n0max = 1;
	// cout << "n0max = " << n0max << endl;
	h = max(0.0, ceil(log(n0) / log(2-eps)));

	n = new int[h + 1]();
	// cout << "k = " << k << endl;
	n[h] = n0;
	sentinel = newNode();
	Node *prev = sentinel;
	for (int i = 0; i < n0; i++) {
		Node *u = newNode();
		u->x = data[i];
		prev->nx[h].next = u;
		prev->nx[h].xnext = u->x;
		prev = u;
	}
	rebuild(h);
}

template<class T>
typename TodoList2<T>::Node* TodoList2<T>::newNode() {
	Node *u = (Node *) malloc(sizeof(Node) + (h + 1) * sizeof(NX));
	memset(u->nx, '\0', (h + 1) * sizeof(NX));
	return u;
}

template<class T>
void TodoList2<T>::deleteNode(Node *u) {
	free(u);
}

template<class T>
void TodoList2<T>::rebuild() {
	// time to rebuild --- free everything and start over
	// TODO: Put some padding in so we only do this O(loglog n) times
	T *data = new T[n[h]];
	Node *prev = sentinel;
	Node *u = sentinel->nx[h].next;
	for (int j = 0; j < n[h]; j++) {
		data[j] = u->x;
		deleteNode(prev);
		prev = u;
		u = u->nx[h].next;
	}
	deleteNode(prev);
	int enn = n[h];
	delete[] n;
	init(data, enn);
	delete[] data;
}


template<class T>
void TodoList2<T>::rebuild(int i) {

	rebuild_freqs[i]++;

	for (int j = i - 1; j >= 0; j--) {
		// populate L_j using L_{j+1}
		n[j] = 0;
		Node *u = sentinel->nx[j + 1].next;
		Node *prev = sentinel;
		bool skipped = false;
		while (u != NULL) {
			if (skipped) {
				prev->nx[j].next = u;
				prev->nx[j].xnext = u->x;
				prev = u;
				n[j]++;
				skipped = false;
			} else {
				skipped = true;
			}
			u = u->nx[j + 1].next;
		}
		prev->nx[j].next = NULL;
		prev->nx[j].xnext = (T)0;
	}

}

template<class T>
T TodoList2<T>::find(T x) {
	Node *u = sentinel;
	for (int i = 0; i <= h; i++)
		if (u->nx[i].next != NULL && u->nx[i].xnext < x)
			u = u->nx[i].next;
	return (u->nx[h].next == NULL) ? (T)0 : u->nx[h].xnext;
}

template<class T>
bool TodoList2<T>::add(T x) {
	// do a search for x and keep track of the search path
	Node *path[50]; // FIXME: hard upper-bound
	Node *u = sentinel;
	int i;
	for (i = 0; i <= h; i++) {
		if (u->nx[i].next != NULL && u->nx[i].xnext < x)
			u = u->nx[i].next;
		path[i] = u;
	}

	// check if x is already here and, if so, abort
	Node *w = u->nx[h].next;
	if (w != NULL && w->x == x)
		return false;

	// insert x everywhere along the search path
	w = newNode();
	w->x = x;
	for (i = h; i >= 0; i--) {
		w->nx[i] = path[i]->nx[i];
		path[i]->nx[i].next = w;
		path[i]->nx[i].xnext = x;
		n[i]++;
	}

	// check if we need to add another level on the bottom
	if (n[h] > a[h])
		rebuild();

	// do partial rebuilding, if necessary
	if (n[0] > n0max) {
		for (i = 1; n[i] > a[i]; i++);
		assert(i <= h);
		rebuild(i);
	}
	return true;
}

template<class T>
TodoList2<T>::~TodoList2() {
	delete[] n;
	delete[] a;
	delete[] rebuild_freqs;
	Node *prev = sentinel;
	while (prev != NULL) {
		Node *u = prev->nx[h].next;
		prev->nx[h].next = NULL;
		deleteNode(prev);
		prev = u;
	}
}

template<class T>
void TodoList2<T>::sanity() {
	assert(n[0] <= n0max);
	for (int i = 0; i <= h; i++) {
		Node *u = sentinel;
		for (int j = 0; j < n[i]; j++) {
			assert(u == sentinel || u->x < u->nx[i].next->x);
			u = u->nx[i].next;
		}
		assert(u->nx[i].next == NULL);
	}
}

template<class T>
void TodoList2<T>::printOn(std::ostream &out) {
	const int max_print = 50;
	cout << "WSSkiplist: n = " << n[h] << ", k = " << h << endl;
	for (int i = 0; i <= h; i++) {
		cout << "L(" << i << "): ";
		if (n[h] <= max_print) {
			Node *u = sentinel->nx[i].next;
			for (int j = 0; j < n[i]; j++) {
				cout << u->x << ",";
				u = u->nx[i].next;
			}
			assert(u == NULL);
		}
		cout << " n(" << i << ") = " << n[i]
		     << " (rebuilt " << rebuild_freqs[i] << " times)" << endl;
	}
}

template<class T>
ostream& operator<<(ostream &out, TodoList2<T> &sl) {
	sl.printOn(out);
	return out;
}

} // fastws namespace

#endif // FASTWS_TODOLIST2_H_
