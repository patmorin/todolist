/**
 * An implementation of B-2B trees
 */
#ifndef __B2B_TREE_H
#define __B2B_TREE_H

#include <stdexcept>
#include <cstdlib>
#include <cstdint>
#include <cassert>

namespace ods {

template <int B, class T>
class B2BTree {
protected:

	static const int B2 = 2*B;

	struct Node {
		uint8_t type;  // tells the number of keys
		T keys[B2];
		Node *children[B2+1];
	};

	Node *root;
	size_t n;

	Node *add(T x, Node *u);
	Node *split(Node *u);

	Node *newNode() {
		return (Node*)calloc(1, sizeof(Node));
	}

	void printIt(Node *u);

	int getChild(Node *u, T x);

	void printNode(Node *u) {
		int i;
		std::cout << "<";
		for (i = 0; i < u->type-1; i++)
			std::cout << u->keys[i] << ",";
		std::cout << u->keys[i] << ">";
	}
public:
	B2BTree() { root = newNode(); n = 0; }

	bool add(T x);
	bool remove(T x);
	T find(T x);
	size_t size() { return n; }

	void printIt();

};


// This implementation uses linear search
template<int B, class T>
int B2BTree<B,T>::getChild(Node *u, T x) {
	int i = 0;
	while (i < u->type && u->keys[i] < x) i++;
	return i;
}



template<int B, class T>
bool B2BTree<B,T>::add(T x) {
	Node *w;
	try {
		w = add(x, root);
	}
	catch (const std::invalid_argument& e) {
		return false; // trying to insert duplicate
	}
    if (w != NULL) {
		Node *r = newNode();
		r->keys[0] = root->keys[B-1];
		root->keys[B-1] = (T)0;
		r->children[0] = root;
		r->children[1] = w;
		root = r;
		root->type = 1;
    }
    n++;
    return true;
}

template<int B, class T>
void B2BTree<B,T>::printIt() {
	printIt(root);
	std::cout << std::endl;
}

template<int B, class T>
void B2BTree<B,T>::printIt(Node *u) {
	if (u == NULL) return;
	int i;
	for (i = 0; i < u->type; i++) {
		printIt(u->children[i]);
		std::cout << u->keys[i];
		    std::cout << ",";
	}
	printIt(u->children[i]);
}

template<int B, class T>
typename B2BTree<B,T>::Node* B2BTree<B,T>::add(T x, Node *u) {
    if (u->children[0] == NULL) { // u is a leaf
    	int i = getChild(u, x);
    	if (i < u->type && u->keys[i] == x) throw std::invalid_argument("dup");
    	std::copy_backward(&u->keys[i], &u->keys[u->type], &u->keys[u->type+1]);
    	u->keys[i] = x;
    	u->type++;
    } else {
    	int i = getChild(u, x);
    	if (i < u->type && u->keys[i] == x) throw std::invalid_argument("dup");
    	Node *w = add(x, u->children[i]);
    	if (w != NULL) { // our child was split, adopt it's new sibling
			std::copy_backward(&u->keys[i], &u->keys[u->type],
							   &u->keys[u->type+1]);
			std::copy_backward(&u->children[i+1], &u->children[u->type+1],
					&u->children[u->type+2]);
			u->keys[i] = u->children[i]->keys[B-1];
			u->children[i]->keys[B-1] = (T)0;
			u->children[i+1] = w;
			u->type++;
    	}
    }
	return u->type == B2 ? split(u) : NULL;
}

template<int B, class T>
typename B2BTree<B,T>::Node* B2BTree<B,T>::split(Node *u) {
	assert(u->type == B2);
	Node *w = newNode();
	std::copy(&u->keys[B], &u->keys[B2], w->keys);
	std::fill(&u->keys[B], &u->keys[B2], (T)0);
	std::copy(&u->children[B], &u->children[B2+1], w->children);
	std::fill(&u->children[B], &u->children[B2+1], (Node*)NULL);
	u->type = B-1;
	w->type = B;
	return w;
}

template<int B, class T>
T B2BTree<B,T>::find(T x) {
	T z = (T)0;
	Node *u = root;
	while (u != NULL) {
		int i;
    	for (i = 0; i < u->type && u->keys[i] < x; i++);
		if (i < u->type) {
			if (u->keys[i] == x)
				return u->keys[i];
			z = u->keys[i];
		}
		u = u->children[i];
	}
	return z;
}

} /* namespace ods */


#endif /* __B2B_TREE_H */
