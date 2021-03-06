/*
 * ScapegoatTree.h
 *
 *  Created on: 2011-11-30
 *      Author: morin
 */

#ifndef SCAPEGOATTREE_H_
#define SCAPEGOATTREE_H_

#include <cmath>

#include "BinarySearchTree.h"

namespace ods {

template<class Node, class T>
class ScapegoatTree : public BinarySearchTree<Node, T> {
protected:
	using BinaryTree<Node>::nil;
	using BinaryTree<Node>::r;
	using BinarySearchTree<Node,T>::n;
	int q;
	double alpha;
	int log_alpha(int q);
	int addWithDepth(Node *u);
	void rebuild(Node *u);
	int packIntoArray(Node *u, Node **a, int i);
	Node *buildBalanced(Node **a, int i, int ns);
public:
	ScapegoatTree(double alpha0=.666);
	virtual ~ScapegoatTree();
	virtual bool add(T x);
	virtual bool remove(T x);
};

template<class T>
class ScapegoatTree1 : public ScapegoatTree<BSTNode1<T>, T> {
public:
	ScapegoatTree1(double alpha0=.666) : ScapegoatTree<BSTNode1<T>, T>(alpha0)
			{ };
};

template<class Node, class T>
inline int ScapegoatTree<Node,T>::log_alpha(int q) {
	return (int)ceil(-log(q)/log(alpha));
}

template<class Node, class T>
inline int ScapegoatTree<Node,T>::addWithDepth(Node *u) {
	Node *w = r;
	if (w == nil) {
		r = u;
		n++; q++;
		return 0;
	}
	bool done = false;
	int d = 0;
	do {
		int res = compare(u->x, w->x);
		if (res < 0) {
			if (w->left == nil) {
				w->left = u;
				u->parent = w;
				done = true;
			} else {
				w = w->left;
			}
		} else if (res > 0) {
			if (w->right == nil) {
				w->right = u;
				u->parent = w;
				done = true;
			}
			w = w->right;
		} else {
			return -1;
		}
		d++;
	} while (!done);
	n++; q++;
	return d;
}



template<class Node, class T> inline
void ScapegoatTree<Node,T>::rebuild(Node *u) {
	int ns = BinaryTree<Node>::size(u);
	Node *p = u->parent;
	Node **a = new Node*[ns];
	packIntoArray(u, a, 0);
	if (p == nil) {
		r = buildBalanced(a, 0, ns);
		r->parent = nil;
	} else if (p->right == u) {
		p->right = buildBalanced(a, 0, ns);
		p->right->parent = p;
	} else {
		p->left = buildBalanced(a, 0, ns);
		p->left->parent = p;
	}
	delete[] a;
}



template<class Node, class T> inline
int ScapegoatTree<Node,T>::packIntoArray(Node *u, Node **a, int i) {
	if (u == nil) {
		return i;
	}
	i = packIntoArray(u->left, a, i);
	a[i++] = u;
	return packIntoArray(u->right, a, i);
}



template<class Node, class T> inline
Node *ScapegoatTree<Node,T>::buildBalanced(Node **a, int i, int ns) {
	if (ns == 0)
		return nil;
	int m = ns / 2;
	a[i + m]->left = buildBalanced(a, i, m);
	if (a[i + m]->left != nil)
		a[i + m]->left->parent = a[i + m];
	a[i + m]->right = buildBalanced(a, i + m + 1, ns - m - 1);
	if (a[i + m]->right != nil)
		a[i + m]->right->parent = a[i + m];
	return a[i + m];
}

template<class Node, class T> inline
ScapegoatTree<Node,T>::ScapegoatTree(double alpha0) {
	alpha = alpha0;
	assert(.5 < alpha && alpha < 1);
	q = 0;
}

template<class Node, class T> inline
ScapegoatTree<Node,T>::~ScapegoatTree() {
	// nothing to do
}

template<class Node, class T> inline
bool ScapegoatTree<Node,T>::add(T x) {
	// first do basic insertion keeping track of depth
	Node *u = new Node;
	u->x = x;
	u->left = u->right = u->parent = nil;
	int d = addWithDepth(u);
	if (d > log_alpha(q)) {
		// depth exceeded, find scapegoat
		Node *w = u->parent;
		int a = BinaryTree<Node>::size(w);
		int b = BinaryTree<Node>::size(w->parent);
		while (a <= alpha*b) {
			w = w->parent;
			a = BinaryTree<Node>::size(w);
			b = BinaryTree<Node>::size(w->parent);
		}
		rebuild(w->parent);
	} else if (d < 0) {
		delete u;
		return false;
	}
	return true;
}

template<class Node, class T> inline
bool ScapegoatTree<Node,T>::remove(T x) {
	if (BinarySearchTree<Node,T>::remove(x)) {
		if (2*n < q) {
			rebuild(r);
			q = n;
		}
		return true;
	}
	return false;
}

} /* namespace ods */
#endif /* SCAPEGOATTREE_H_ */
