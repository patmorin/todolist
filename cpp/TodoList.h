/**
 * (c) 2014 Pat Morin, Released under a CC BY 3.0 License:
 *     https://creativecommons.org/licenses/by/3.0/
 *
 * TodoList.h : A top-down skiplist
 *
 * A fairly straightforward implementation of top-down skiplists.  Each node
 * is represented as an array that contains all its next pointers, like in
 * Pugh's original skiplist implementation.
 *
 * This particular implementation is a space hog.  Every element in the
 * structure has its own array of length k=Theta(log n)$.  This avoids the
 * resizing that would otherwise be required when rebuilding levels.
 */
#ifndef FASTWS_TODOLIST_H_
#define FASTWS_TODOLIST_H_

#include <cmath>
#include <cstring>
#include <cstdlib>
#include <climits>
#include <cassert>

#include <iostream>
using namespace std;

namespace todolist {

/**
 * A dictionary with the working-set property.
 */
template<class T>
class TodoList {
protected:
	struct NP;

	struct Node {
		T x;          // data
		Node *next[]; // a stack of next pointers
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
	TodoList(double eps0 = .3, T *data = NULL, int n0 = 0);
	virtual ~TodoList();
	T find(T x);
	bool add(T x);
	int size() {
		return n[h];
	}

	void printOn(std::ostream &out);
};

template<class T>
TodoList<T>::TodoList(double eps0, T *data, int n0) {
	eps = eps0;

	int kmax = 100; // FIXME: potential limitation here
	rebuild_freqs = new int[kmax+1]();
	double base_a = 2.0-eps;
	a = new int[kmax+1];
	for (int i = 0; i <= kmax; i++)
		a[i] = pow(base_a, i);

	init(data, n0);
}

template<class T>
void TodoList<T>::init(T *data, int n0) {

	// Compute critical values depending on epsilon and n
	n0max = ceil(2. / eps);
	n0max = 1;
	h = max(0.0, ceil(log(n0) / log(2-eps)));

	n = new int[h + 1]();
	n[h] = n0;
	sentinel = newNode();
	Node *prev = sentinel;
	for (int i = 0; i < n0; i++) {
		Node *u = newNode();
		u->x = data[i];
		prev->next[h] = u;
		prev = u;
	}
	rebuild(h);
}

template<class T>
typename TodoList<T>::Node* TodoList<T>::newNode() {
	Node *u = (Node *) malloc(sizeof(Node) + (h + 1) * sizeof(Node*));
	memset(u->next, '\0', (h + 1) * sizeof(Node*));
	return u;
}

template<class T>
void TodoList<T>::deleteNode(Node *u) {
	free(u);
}

template<class T>
void TodoList<T>::rebuild() {
	// time to rebuild --- free everything and start over
	// TODO: Put some padding in so we only do this O(loglog n) times
	T *data = new T[n[h]];
	Node *prev = sentinel;
	Node *u = sentinel->next[h];
	for (int j = 0; j < n[h]; j++) {
		data[j] = u->x;
		deleteNode(prev);
		prev = u;
		u = u->next[h];
	}
	deleteNode(prev);
	int enn = n[h];
	delete[] n;
	init(data, enn);
	delete[] data;
}


template<class T>
void TodoList<T>::rebuild(int i) {

	rebuild_freqs[i]++;

	for (int j = i - 1; j >= 0; j--) {
		// populate L_j using L_{j+1}
		n[j] = 0;
		Node *u = sentinel->next[j + 1];
		Node *prev = sentinel;
		bool skipped = false;
		while (u != NULL) {
			if (skipped) {
				prev->next[j] = u;
				prev = u;
				n[j]++;
				skipped = false;
			} else {
				skipped = true;
			}
			u = u->next[j + 1];
		}
		prev->next[j] = NULL;
	}

}

template<class T>
T TodoList<T>::find(T x) {
	Node *u = sentinel;
	for (int i = 0; i <= h; i++)
		if (u->next[i] != NULL && u->next[i]->x < x)
			u = u->next[i];
	return (u->next[h] == NULL) ? (T)0 : u->next[h]->x;
}

template<class T>
bool TodoList<T>::add(T x) {
	// do a search for x and keep track of the search path
	Node *path[50]; // FIXME: hard upper-bound
	Node *u = sentinel;
	int i;
	for (i = 0; i <= h; i++) {
		if (u->next[i] != NULL && u->next[i]->x < x)
			u = u->next[i];
		path[i] = u;
	}

	// check if x is already here and, if so, abort
	Node *w = u->next[h];
	if (w != NULL && w->x == x)
		return false;

	// insert x everywhere along the search path
	w = newNode();
	w->x = x;
	for (i = h; i >= 0; i--) {
		w->next[i] = path[i]->next[i];
		path[i]->next[i] = w;
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
TodoList<T>::~TodoList() {
	delete[] n;
	delete[] a;
	delete[] rebuild_freqs;
	Node *prev = sentinel;
	while (prev != NULL) {
		Node *u = prev->next[h];
		prev->next[h] = NULL;
		deleteNode(prev);
		prev = u;
	}
}

template<class T>
void TodoList<T>::sanity() {
	assert(n[0] <= n0max);
	for (int i = 0; i <= h; i++) {
		Node *u = sentinel;
		for (int j = 0; j < n[i]; j++) {
			assert(u == sentinel || u->x < u->next->x);
			u = u->next[i];
		}
		assert(u->next[i] == NULL);
	}
}

template<class T>
void TodoList<T>::printOn(std::ostream &out) {
	const int max_print = 50;
	out << "WSSkiplist: n = " << n[h] << ", k = " << h << endl;
	for (int i = 0; i <= h; i++) {
		out << "L(" << i << "): ";
		if (n[h] <= max_print) {
			Node *u = sentinel->next[i];
			for (int j = 0; j < n[i]; j++) {
				out << u->x << ",";
				u = u->next[i];
			}
			assert(u == NULL);
		}
		out << " n(" << i << ") = " << n[i]
		     << " (rebuilt " << rebuild_freqs[i] << " times)" << endl;
	}
}

template<class T>
ostream& operator<<(ostream &out, TodoList<T> &sl) {
	sl.printOn(out);
	return out;
}

} // fastws namespace

#endif // FASTWS_TODOLIST_H_
