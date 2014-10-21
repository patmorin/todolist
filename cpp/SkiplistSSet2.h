/*
 * SkiplistSSet2.h
 *
 *  Created on: 2011-11-28
 *      Author: morin
 */

#ifndef SkiplistSSet2_H_
#define SkiplistSSet2_H_
#include <iostream>
#include <cstdlib>
#include <cstring>

#include "utils.h"

namespace ods {

template<class T>
class SkiplistSSet2 {
protected:
	struct Node;

	struct NX {
			Node *next;
			T xnext;
	};
	struct Node {
		T x;
		int height;     // length of next
		NX nx[];
	};

	T null;

	Node *sentinel;
	int h;
	int n;
	Node** stack;

	Node *newNode(T x, int h);
	void deleteNode(Node *u);
	Node* findPredNode(T x);

public:
	SkiplistSSet2();

	virtual ~SkiplistSSet2();

	T find(T x);
	bool remove(T x);
	bool add(T x);
	int pickHeight();
	void clear();
	int size() { return n;	}
};

template<class T>
typename SkiplistSSet2<T>::Node* SkiplistSSet2<T>::newNode(T x, int h) {
	Node *u = (Node*)malloc(sizeof(Node)+(h+1)*sizeof(NX));
	u->x = x;
	u->height = h;
	memset(u->nx, '\0', (h+1) * sizeof(NX));
	return u;
}

template<class T>
void SkiplistSSet2<T>::deleteNode(Node *u) {
	free(u);
}

template<class T>
typename SkiplistSSet2<T>::Node* SkiplistSSet2<T>::findPredNode(T x) {
	Node *u = sentinel;
	int r = h;
	while (r >= 0) {
		while (u->nx[r].next != NULL && u->nx[r].xnext < x)
			u = u->nx[r].next; // go right in list r
		r--; // go down into list r-1
	}
	return u;
}

template<class T>
SkiplistSSet2<T>::SkiplistSSet2() {
	null = (T)0; // FIXME: requires T has integer constructor
	n = 0;
	sentinel = newNode(null, sizeof(int)*8);
	memset(sentinel->nx, '\0', sizeof(NX*)*sentinel->height);
	stack = new Node*[sentinel->height];
	h = 0;
}

template<class T>
SkiplistSSet2<T>::~SkiplistSSet2() {
	clear();
	deleteNode(sentinel);
	delete[] stack;
}

template<class T>
T SkiplistSSet2<T>::find(T x) {
	Node *u = sentinel;
	int r = h;
	while (r >= 0) {
		while (u->nx[r].next != NULL && u->nx[r].xnext < x)
			u = u->nx[r].next; // go right in list r
		r--; // go down into list r-1
	}
	return u->nx[0].next == NULL ? null : u->nx[0].xnext;
}

template<class T>
bool SkiplistSSet2<T>::remove(T x) {
	bool removed = false;
	Node *u = sentinel, *del;
	int r = h;
	int comp = 0;
	while (r >= 0) {
		while (u->nx[r].next != NULL
               && (comp = compare(u->nx[r].xnext, x)) < 0) {
			u = u->nx[r].next;
		}
		if (u->nx[r].next != NULL && comp == 0) {
			removed = true;
			del = u->nx[r].next;
			u->nx[r].next = u->nx[r].next->nx[r].next;
			if (u == sentinel && u->nx[r].next == NULL)
				h--; // skiplist height has gone down
		}
		r--;
	}
	if (removed) {
		delete del;
		n--;
	}
	return removed;
}

template<class T>
bool SkiplistSSet2<T>::add(T x) {
	Node *u = sentinel;
	int r = h;
	int comp = 0;
	while (r >= 0) {
		while (u->nx[r].next != NULL
               && (comp = compare(u->nx[r].xnext, x)) < 0)
			u = u->nx[r].next;
		if (u->nx[r].next != NULL && comp == 0)
			return false;
		stack[r--] = u;        // going down, store u
	}
	Node *w = newNode(x, pickHeight());
	while (h < w->height)
		stack[++h] = sentinel; // height increased
	for (int i = 0; i <= w->height; i++) {
		w->nx[i].next = stack[i]->nx[i].next;
		w->nx[i].xnext = stack[i]->nx[i].xnext;
		stack[i]->nx[i].next = w;
		stack[i]->nx[i].xnext = x;
	}
	n++;
	return true;
}

template<class T>
int SkiplistSSet2<T>::pickHeight() {
	int z = rand();
	int k = 0;
	int m = 1;
	while ((z & m) != 0) {
		k++;
		m <<= 1;
	}
	return k;
}

template<class T>
void SkiplistSSet2<T>::clear() {
	Node *u = sentinel->nx[0].next;
	while (u != NULL) {
		Node *n = u->nx[0].next;
		deleteNode(u);
		u = n;
	}
	memset(sentinel->nx, '\0', sizeof(NX)*h);
	h = 0;
	n = 0;
}

template<class T>
std::ostream& operator<<(std::ostream &out, const SkiplistSSet2<T> &sl) {
	out << "BinarySearchTree";
	return out;
}


} /* namespace ods */

#endif /* SkiplistSSet2_H_ */
