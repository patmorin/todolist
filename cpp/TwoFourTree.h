/**
 * An implementation of 2-4 trees
 */
#ifndef __TWOFOUR_TREE_H
#define __TWOFOUR_TREE_H

#include <stdexcept>
#include <cstdlib>
#include <cstdint>
#include <cassert>

namespace ods {

template <class T>
class TwoFourTree {
protected:

	struct Node {
		uint8_t type;  // tells the number of keys
		T keys[4];
		Node *children[5];
	};

	Node *root;
	size_t n;

	Node *add(T x, Node *u);
	Node *split(Node *u);

	Node *newNode() {
		return (Node*)calloc(1, sizeof(Node));
	}

	void printIt(Node *u);

public:
	TwoFourTree() { root = newNode(); n = 0; }

	bool add(T x);
	bool remove(T x);
	T find(T x);
	size_t size() { return n; }

	void printIt();

};


template<class T>
bool TwoFourTree<T>::add(T x) {
	Node *w;
	try {
		w = add(x, root);
	}
	catch (const std::invalid_argument& e) {
		return false; // trying to insert duplicate
	}
    if (w != NULL) {
		Node *r = newNode();
		r->keys[0] = root->keys[1];
		root->keys[1] = (T)0;
		r->children[0] = root;
		r->children[1] = w;
		root = r;
		root->type = 1;
    }
    n++;
    return true;
}

template<class T>
void TwoFourTree<T>::printIt() {
	printIt(root);
	std::cout << std::endl;
}

template<class T>
void TwoFourTree<T>::printIt(Node *u) {
	if (u == NULL) return;
	int i;
	// std::cout << "<";
	for (i = 0; i < u->type; i++) {
		printIt(u->children[i]);
		std::cout << u->keys[i];
		//if (i != u->type - 1)
		    std::cout << ",";
	}
	printIt(u->children[i]);
	// std::cout << ">";
}


//std::cout << "<" << u->keys[0] << ","
//		  << u->keys[1] << ","
//		  << u->keys[2] << ","
//		  << u->keys[3] << ">";

template<class T>
typename TwoFourTree<T>::Node* TwoFourTree<T>::add(T x, Node *u) {
	int i;
    if (u->children[0] == NULL) { // u is a leaf
    	for (i = 0; i < u->type && u->keys[i] < x; i++);
    	if (i < u->type && u->keys[i] == x) throw std::invalid_argument("dup");
    	std::copy_backward(&u->keys[i], &u->keys[u->type], &u->keys[u->type+1]);
    	u->keys[i] = x;
    	u->type++;
    } else {
    	for (i = 0; i < u->type && u->keys[i] < x; i++);
    	if (i < u->type && u->keys[i] == x) throw std::invalid_argument("dup");
    	Node *w = add(x, u->children[i]);
    	if (w != NULL) { // our child was split, adopt it's new sibling
			std::copy_backward(&u->keys[i], &u->keys[u->type],
							   &u->keys[u->type+1]);
			std::copy_backward(&u->children[i+1], &u->children[u->type+1],
					&u->children[u->type+2]);
			u->keys[i] = u->children[i]->keys[1];
			u->children[i]->keys[1] = (T)0;
			u->children[i+1] = w;
			u->type++;
    	}
    }
	return u->type == 4 ? split(u) : NULL;
}

template<class T>
typename TwoFourTree<T>::Node* TwoFourTree<T>::split(Node *u) {
	assert(u->type == 4);
	Node *w = newNode();
	std::copy(&u->keys[2], &u->keys[4], w->keys);
	std::fill(&u->keys[2], &u->keys[4], (T)0);
	std::copy(&u->children[2], &u->children[5], w->children);
	std::fill(&u->children[2], &u->children[5], (Node*)NULL);
	u->type = 1;
	w->type = 2;
	return w;
}

template<class T>
T TwoFourTree<T>::find(T x) {
	Node *u = root;
	while (u != NULL) {
		int i;
    	for (i = 0; i < u->type && u->keys[i] < x; i++);
		if (i < u->type && u->keys[i] == x)
			return u->keys[i];
		u = u->children[i];
	}
	return (T)0;
}

} /* namespace ods */


#endif /* __TWOFOUR_TREE_H */
