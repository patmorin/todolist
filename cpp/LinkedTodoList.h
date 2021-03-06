/**
 * (c) 2014 Pat Morin, Released under a CC BY 3.0 License:
 *     https://creativecommons.org/licenses/by/3.0/
 *
 * todo.h : A top-down skiplist
 *
 * A top-down skiplist is a variant of a skiplist that stays balanced by
 * using partial rebuilding on the top levels of the list.  Unlike a normal
 * skiplist, it maintains a tight enough structure that only one comparison
 * is necessary at each level.
 *
 * - add(x) and remove(x) run in O(log n) amortized time.
 * - find(x) runs in O(log n) worst-case time and performs
 *   ceiling((1+epsilon)log n) comparisons.
 *
 * This particular implementation is a space hog.  Every element in the
 * structure has its own array of length k=Theta(log n)$.  This avoids the
 * resizing that would otherwise be required when rebuilding levels.
 */
#ifndef FASTWS_LINKEDTODOLIST_H_
#define FASTWS_LINKEDTODOLIST_H_

#include <cmath>
#include <cstring>
#include <cstdlib>
#include <climits>
#include <cassert>

#include <iostream>
using namespace std;

namespace todolist {

template<class T>
class LinkedTodoList {
protected:
	struct NP;

	struct Node {
		T x;         // data
		Node *next;
		Node *down;
	};

	int k;    // there are k+1 lists numbered 0,...,k
	int *n;   // n[i] is the size of the i'th list
	Node **sentinel; // sentinel-next[i] is the first element of list i

	// parameters used to determine lists sizes
	double eps;
	int n0max;
	int *a;

	// FIXME: for profiling information
	int *rebuild_freqs;

	void init(T *data, int n);
	void deleteList(Node *head);
	void rebuild();
	void rebuild(int i);

	Node *newNode(T x, Node *down = NULL, Node *next = NULL);
	void deleteNode(Node *u);

	void sanity();

public:
	LinkedTodoList(double eps0 = .3, T *data = NULL, int n0 = 0);
	virtual ~LinkedTodoList();
	T find(T x);
	bool add(T x);
	int size() {
		return n[k];
	}

	void printOn(std::ostream &out);
};

template<class T>
LinkedTodoList<T>::LinkedTodoList(double eps0, T *data, int n0) {
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
void LinkedTodoList<T>::init(T *data, int n0) {
	// Compute critical values depending on epsilon and n
	n0max = ceil(2. / eps);
	n0max = 1;
	k = max(0.0, ceil(log(n0) / log(2-eps)));

	n = new int[k + 1]();
	sentinel = new Node*[k+1];
	sentinel[k] = newNode((T)0);
	for (int i = k-1; i >= 0; i--)
		sentinel[i] = newNode((T)0, sentinel[i+1]);
	n[k] = n0;
	Node *next = NULL;
	for (int j = n0-1; j >= 0; j--) {
		Node *u = newNode(data[j], NULL, next);
		next = u;
	}
	sentinel[k]->next = next;
	rebuild(k);
}

template<class T>
typename LinkedTodoList<T>::Node* LinkedTodoList<T>::newNode(T x, Node *down,
		Node *next) {
	Node *u = new Node;
	u->down = down;
	u->next = next;
	u->x = x;
	return u;
}

template<class T>
void LinkedTodoList<T>::deleteNode(Node *u) {
	delete u;
}

template<class T>
void LinkedTodoList<T>::deleteList(Node *head) {
	Node *u = head, *next = NULL;
	while (u != NULL) {
		next = u->next;
		deleteNode(u);
		u = next;
	}
}

template<class T>
void LinkedTodoList<T>::rebuild() {
	// delete all but the k'th list
	for (int i = 0; i < k; i++) {
		deleteList(sentinel[i]);
		sentinel[i] = NULL;
	}
	// save these for later
	int n0 = n[k];
	Node *head = sentinel[k]->next;

	// start over with new paramters but the same list
	delete[] n;
	delete sentinel[k];
	delete[] sentinel;
	k = max(0.0, ceil(log(n0) / log(2-eps)));
	n = new int[k + 1]();
	sentinel = new Node*[k+1];
	sentinel[k] = newNode((T)0);
	for (int i = k-1; i >= 0; i--)
		sentinel[i] = newNode((T)0, sentinel[i+1]);
	n[k] = n0;
	sentinel[k]->next = head;
	rebuild(k);
}


template<class T>
void LinkedTodoList<T>::rebuild(int i) {

	rebuild_freqs[i]++;

	for (int j = i - 1; j >= 0; j--) {
		// populate L_j using L_{j+1}
		deleteList(sentinel[j]->next);
		n[j] = 0;
		Node *u = sentinel[j+1]->next;
		Node *prev = sentinel[j];
		bool skipped = false;
		while (u != NULL) {
			if (skipped) {
				prev->next = newNode(u->x, u);
				prev = prev->next;
				n[j]++;
			}
			skipped = !skipped;
			u = u->next;
		}
		prev->next = NULL;
	}

}

template<class T>
T LinkedTodoList<T>::find(T x) {
	Node *u = sentinel[0];
	if (u->next != NULL && u->next->x < x)
		u = u->next;
	while (u->down != NULL) {
		if (u->next != NULL && u->next->x < x)
			u = u->next;
		u = u->down;
	}
	return (u->next == NULL) ? (T)0 : u->next->x;
}

template<class T>
bool LinkedTodoList<T>::add(T x) {
	// do a search for x and keep track of the search path
	Node *path[50]; // FIXME: hard upper-bound
	Node *u = sentinel[0];
	int i;
	for (i = 0; i <= k; i++) {
		if (u->next != NULL && u->next->x < x)
			u = u->next;
		path[i] = u;
		u = u->down;
	}

	// check if x is already here and, if so, abort
	if (path[k]->next != NULL && path[k]->next->x == x)
		return false;

	// insert x everywhere along the search path
	Node *down = NULL;
	for (i = k; i >= 0; i--) {
		Node *w = newNode(x, down, path[i]->next);
		down = w;
		path[i]->next = w;
		n[i]++;
	}

	// check if we need to add another level on the bottom
	if (n[k] > a[k])
		rebuild();

	// do partial rebuilding, if necessary
	if (n[0] > n0max) {
		for (i = 1; n[i] > a[i]; i++);
		assert(i <= k);
		rebuild(i);
	}
	return true;
}

template<class T>
LinkedTodoList<T>::~LinkedTodoList() {
	delete[] n;
	delete[] a;
	delete[] rebuild_freqs;
	for (int i = 0; i <= k; i++)
		deleteList(sentinel[i]);
	delete[] sentinel;
}

template<class T>
void LinkedTodoList<T>::sanity() {
	assert(n[0] <= n0max);
	for (int i = 0; i <= k; i++) {
		Node *u = sentinel;
		for (int j = 0; j < n[i]; j++) {
			assert(u == sentinel || u->x < u->next->x);
			u = u->next[i];
		}
		assert(u->next[i] == NULL);
	}
}

template<class T>
void LinkedTodoList<T>::printOn(std::ostream &out) {
	const int max_print = 50;
	cout << "WSSkiplist: n = " << n[k] << ", k = " << k << endl;
	for (int i = 0; i <= k; i++) {
		cout << "L(" << i << "): ";
		if (n[k] <= max_print) {
			Node *u = sentinel[i]->next;
			for (int j = 0; j < n[i]; j++) {
				cout << u->x << ",";
				u = u->next;
			}
			assert(u == NULL);
		}
		cout << " n(" << i << ") = " << n[i]
		     << " (rebuilt " << rebuild_freqs[i] << " times)" << endl;
	}
}

template<class T>
ostream& operator<<(ostream &out, LinkedTodoList<T> &sl) {
	sl.printOn(out);
	return out;
}

} // fastws namespace

#endif // FASTWS_LINKEDTODOLIST_H_
