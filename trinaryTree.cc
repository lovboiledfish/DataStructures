/*
 * =================================================================
 * File name: trinaryTree.cc
 * Functionality:
 *   to implement a tri-nary tree data structure with insertion
 *   and deletion operations
 * Compiler:  G++
 *
 * Version:   1.0
 * Author:    if this code works, it's Feiyi Zhan, otherwise,
 *            I have no idea who wrote it.
 * Date:      02/26/2014
 * =================================================================
 */

#include <deque>
#include <stdlib.h>
#include <iostream>

#define MAX_NODE_NUM 1000

using namespace std;

// data structure of Tri-Nary tree node
typedef struct TreeNode {
    int val;
    TreeNode *left;
    TreeNode *middle;
    TreeNode *right;
    TreeNode *parent;

    // member functions
    TreeNode():
        val(0), left(NULL), middle(NULL), right(NULL), parent(NULL) {}
    TreeNode& operator=(TreeNode &rhs) {
        val    = rhs.val;
        left   = rhs.left;
        middle = rhs.middle;
        right  = rhs.right;
        parent = rhs.parent;

        return *this;
    }
} TreeNode;

// data structure of Tri-Nary tree
class trinaryTree {

    // function members
    public:
        trinaryTree(int size = MAX_NODE_NUM):
            _root(NULL), _curNode(0) {
            // use continuous heap space to contain nodes
            // for optimization of insertion and destruction
	    // at the price of space overhead of "parent"
	    // pointer and slower deletion

            _poolSize = (size > 0)? size : MAX_NODE_NUM;
            _nodePool = new TreeNode[_poolSize];
            if (!_nodePool) {
                cerr << "Not enough space!" << endl;
                exit(0);
            }
        }
        ~trinaryTree() {
            delete _nodePool;
        }
        TreeNode* getRoot();
        bool      insertNode(int value);
        void      deleteNode(int value);
    private:
        bool      _insertNode(TreeNode *&node, TreeNode *prt, int value);
        TreeNode* _deleteNode(TreeNode *node, int value);
        void      _disposal(TreeNode *node);
        TreeNode* _findSuccessor(TreeNode *node);

    // data members
    private:
        TreeNode* _root;
        TreeNode* _nodePool;
        int       _curNode;
        int       _poolSize;
};

inline TreeNode* trinaryTree::getRoot() {
    return _root;
}

inline bool trinaryTree::insertNode(int value) {
    return _insertNode(_root, NULL, value);
}

/*
 * Function name: _insertNode
 * Usage: insert new node into the tree
 *    Input:  the value of the node
 *    Output: a boolean value indicating if the insertion is successful
 */
bool trinaryTree::_insertNode(TreeNode *&node, TreeNode *prt, int value) {
    if (!node) {
        // when no more heap space to take, return false
        if (_curNode == _poolSize) return false;

        // take unused node from node pool
        node = &_nodePool[_curNode++];
        node->val = value;

        // set parent
        if (prt)
	    node->parent = prt;
        
	return true;
    }
    
    if (node->val < value) {
        return _insertNode(node->right, node, value);
    }
    else if (node->val > value) {
        return _insertNode(node->left, node, value);
    }
    else {
        return _insertNode(node->middle, node, value);
    }
}

inline void trinaryTree::deleteNode(int value) {
    _root = _deleteNode(_root, value);
}

/*
 * Function Name: _deleteNode
 * Usage: delete a node with a certain value in the tree
 *    Input:  a TreeNode pointer which is the current search position
 *            and a integer naming the value of the node to be deleted
 *    Output: a new TreeNode pointer which is to replace the old one
 */
TreeNode* trinaryTree::_deleteNode(TreeNode *node, int value) {

    if (!node)
        return node;

    if (node->val == value) {
        if (!node->left && !node->right && !node->middle) {
            // when three child nodes are all NULL, simply discard the
	    // node by calling deletion function

            _disposal(node);
	    return NULL;
        }
        if (node->middle) {
            // the middle child will never have left or right child
            // due to the insertion algorithm

            node->middle = _deleteNode(node->middle, value);
        }
        else {
            if (node->left && node->right) {
                // when both left and right child exist, replace the current
                // node with its successor

                TreeNode *successor_node = _findSuccessor(node->right);
                node->val = successor_node->val;
                node->right = _deleteNode(node->right, successor_node->val);
            }
            else if (node->left) {
                // when right child is NULL, replace the current node with
                // its left child

                TreeNode *left = node->left;
		left->parent = node->parent;
		_disposal(node);
                return (left == &_nodePool[_curNode])? node : left;
            }
            else {
                // when left child is NULL, replace the current node with
                // its right child

                TreeNode *right = node->right;
		right->parent = node->parent;
		_disposal(node);
		return (right == &_nodePool[_curNode])? node : right;
            }
        }
    }
    else if (node->val < value)
        node->right = _deleteNode(node->right, value);
    else
        // node->val > value
        node->left = _deleteNode(node->left, value);

    return node;
}

/*
 * Function name: _disposal
 * Usage: dispose deleted node apart from the tree
 *    Input: a TreeNode pointer which is to be disposed
 *    Output: void
 */
void trinaryTree::_disposal(TreeNode *node) {
    // delete node by exchanging it with the last node in the
    // pool, changing its parent's pointer, and decreasing the
    // write pointer of node pool by 1

    if (node->parent) {
        if (node->parent->left == node)
            node->parent->left = NULL;
        else if (node->parent->middle == node)
            node->parent->middle = NULL;
        else
            node->parent->right = NULL;
    }

    *node = _nodePool[--_curNode];
    if (node->parent) {
        if (node->parent->left == &_nodePool[_curNode]) {
            node->parent->left = node;
	}
        else if (node->parent->middle == &_nodePool[_curNode]) {
            node->parent->middle = node;
	}
        else {
            node->parent->right = node;
	}
    }
    if (node->left)
        node->left->parent = node;
    if (node->middle)
	node->middle->parent = node;
    if (node->right)
	node->right->parent = node;

    _nodePool[_curNode].left = NULL;
    _nodePool[_curNode].middle = NULL;
    _nodePool[_curNode].right = NULL;
}

TreeNode* trinaryTree::_findSuccessor(TreeNode *node) {
    if (node->left == NULL)
        return node;
    else
        return _findSuccessor(node->left);
}

/* =========================================================
 * tests start from here
 * =========================================================
 */
void printTree(TreeNode *root, trinaryTree &tree) {
    // print tree using BFS
    if (!root) return;

    deque<TreeNode *> q;
    int cnt[2];
    cnt[0] = 1;
    cnt[1] = 0;
    bool toggle = 0;
    q.push_back(root);
    TreeNode *cur;
    while (!q.empty()) {
        cur = q.front();
        cout << cur->val << " ";
        q.pop_front();
        --cnt[toggle];
        if (cur->left) {
            ++cnt[!toggle];
            q.push_back(cur->left);
        }
        if (cur->middle) {
            ++cnt[!toggle];
            q.push_back(cur->middle);
        }
        if (cur->right) {
            ++cnt[!toggle];
            q.push_back(cur->right);
        }
        if (!cnt[toggle]) {
            toggle = !toggle;
            cout << endl;
        }
    }
    cout << endl << endl;
}

int main(int argc, char** argv) {
    trinaryTree tree;
    int arr[] = {5, 4, 9, 5, 7, 2, 2};
    for (int i = 0; i < sizeof(arr)/sizeof(int); ++i)
        tree.insertNode(arr[i]);
    printTree(tree.getRoot(), tree);
    tree.deleteNode(4);
    printTree(tree.getRoot(), tree);
    tree.deleteNode(5);
    printTree(tree.getRoot(), tree);
    tree.deleteNode(9);
    printTree(tree.getRoot(), tree);
    tree.deleteNode(5);
    printTree(tree.getRoot(), tree);
    tree.insertNode(5);
    printTree(tree.getRoot(), tree);

    return 0;
}
