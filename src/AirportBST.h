#ifndef AIRPORTBST_H
#define AIRPORTBST_H

#include <string>
#include <vector>
#include <iostream>
#include "Airport.h"
using namespace std;

class AirportBST
{
private:
    class Node
    {
    public:
        string  key;
        Airport data;
        Node*   left;
        Node*   right;

        Node(string k, Airport d)
        {
            key   = k;
            data  = d;
            left  = nullptr;
            right = nullptr;
        }
    };

    Node* root;
    int   count;

    // Level order walk. nodes grows while the loop runs.
    void collect(vector<Node*> &nodes, vector<int> &depths)
    {
        if (root == nullptr) return;

        nodes.push_back(root);
        depths.push_back(0);

        for (int i = 0; i < (int)nodes.size(); i++)
        {
            Node* node = nodes[i];
            int   d    = depths[i];

            if (node->left != nullptr)
            {
                nodes.push_back(node->left);
                depths.push_back(d + 1);
            }
            if (node->right != nullptr)
            {
                nodes.push_back(node->right);
                depths.push_back(d + 1);
            }
        }
    }


    // Task 3(a). Returns the new root of this subtree, as in the lecture notes.
    Node* removeNode(Node* node, string key, string &caseUsed)
    {
        if (node == nullptr) return nullptr;

        if (key < node->key)
        {
            node->left = removeNode(node->left, key, caseUsed);
        }
        else if (key > node->key)
        {
            node->right = removeNode(node->right, key, caseUsed);
        }
        else
        {
            if (node->left == nullptr && node->right == nullptr)
            {
                caseUsed = "D1: a leaf";
                delete node;
                count--;
                return nullptr;
            }
            if (node->left == nullptr)
            {
                caseUsed = "D2: only a right child, " + node->right->key;
                Node* child = node->right;
                delete node;
                count--;
                return child;
            }
            if (node->right == nullptr)
            {
                caseUsed = "D2: only a left child, " + node->left->key;
                Node* child = node->left;
                delete node;
                count--;
                return child;
            }

            Node* s = node->right;
            while (s->left != nullptr) s = s->left;
            caseUsed = "D3: two children, successor " + s->key;
            node->key  = s->key;
            node->data = s->data;
            string ignored = "";
            node->right = removeNode(node->right, s->key, ignored);
        }
        return node;
    }

    bool isBST(Node* node, bool hasLo, string lo, bool hasHi, string hi)
    {
        if (node == nullptr) return true;
        if (hasLo && node->key <= lo) return false;
        if (hasHi && node->key >= hi) return false;
        return isBST(node->left,  hasLo, lo, true, node->key)
            && isBST(node->right, true, node->key, hasHi, hi);
    }


    // Task 5(a). Pruned walk: a subtree that cannot hold a key in
    // [from, to] is never entered.
    void rangeNode(Node* node, string from, string to,
                   vector<Airport> &out, int &visited)
    {
        if (node == nullptr) return;

        visited++;

        if (node->key > from)
            rangeNode(node->left, from, to, out, visited);

        if (from <= node->key && node->key <= to)
            out.push_back(node->data);

        if (node->key < to)
            rangeNode(node->right, from, to, out, visited);
    }


    // Task 6. Middle element becomes the root, then the same on each half.
    Node* buildBalancedNode(vector<Airport> &a, int lo, int hi)
    {
        if (lo > hi) return nullptr;

        int mid = (lo + hi) / 2;
        Node* node = new Node(a[mid].iata, a[mid]);
        count++;
        node->left  = buildBalancedNode(a, lo, mid - 1);
        node->right = buildBalancedNode(a, mid + 1, hi);
        return node;
    }

public:
    AirportBST()
    {
        root  = nullptr;
        count = 0;
    }

    int size() { return count; }

    string rootKey()
    {
        if (root == nullptr) return "(empty)";
        return root->key;
    }

    // Returns false if the key is already in the tree (I4).
    bool insert(string key, Airport data)
    {
        Node* newNode = new Node(key, data);

        if (root == nullptr)
        {
            root = newNode;
            count++;
            return true;
        }

        Node* cur = root;
        while (true)
        {
            if (key < cur->key)
            {
                if (cur->left == nullptr)
                {
                    cur->left = newNode;
                    count++;
                    return true;
                }
                cur = cur->left;
            }
            else if (key > cur->key)
            {
                if (cur->right == nullptr)
                {
                    cur->right = newNode;
                    count++;
                    return true;
                }
                cur = cur->right;
            }
            else
            {
                delete newNode;
                return false;
            }
        }
    }


    // Task 2(a). Walks one path from the root. path collects every code
    // compared, so path.size() is the number of comparisons.
    bool find(string target, vector<string> &path, Airport &record)
    {
        path.clear();
        Node* cur = root;

        while (cur != nullptr)
        {
            path.push_back(cur->key);
            if (target == cur->key)
            {
                record = cur->data;
                return true;
            }
            else if (target < cur->key)
                cur = cur->left;
            else
                cur = cur->right;
        }
        return false;
    }

    // Task 2(b). Smallest key is the leftmost node, largest is the rightmost.
    string minKey()
    {
        if (root == nullptr) return "(empty)";
        Node* cur = root;
        while (cur->left != nullptr) cur = cur->left;
        return cur->key;
    }

    string maxKey()
    {
        if (root == nullptr) return "(empty)";
        Node* cur = root;
        while (cur->right != nullptr) cur = cur->right;
        return cur->key;
    }

    // Task 2(b). Picks a leaf and the deepest node out of this tree,
    // so the test cases are real codes rather than guesses.
    string aLeafKey()
    {
        vector<Node*> nodes;
        vector<int>   depths;
        collect(nodes, depths);

        int n = (int)nodes.size();
        for (int i = 0; i < n; i++)
        {
            if (nodes[i]->left == nullptr && nodes[i]->right == nullptr)
                return nodes[i]->key;
        }
        return "(none)";
    }

    string deepestKey()
    {
        vector<Node*> nodes;
        vector<int>   depths;
        collect(nodes, depths);

        int best = 0;
        int n = (int)depths.size();
        for (int i = 0; i < n; i++)
        {
            if (depths[i] > depths[best]) best = i;
        }
        return nodes[best]->key;
    }


    // Task 3(a). caseUsed reports which deletion case was applied.
    bool remove(string key, string &caseUsed)
    {
        caseUsed = "";
        int before = count;
        root = removeNode(root, key, caseUsed);
        return count < before;
    }

    // Task 3(b). One real code from this tree for each deletion case.
    // kind: 0 leaf, 1 only a left child, 2 only a right child, 3 two children
    string findCase(int kind)
    {
        vector<Node*> nodes;
        vector<int>   depths;
        collect(nodes, depths);

        int n = (int)nodes.size();
        for (int i = 0; i < n; i++)
        {
            bool hasLeft  = nodes[i]->left  != nullptr;
            bool hasRight = nodes[i]->right != nullptr;

            if (kind == 0 && !hasLeft && !hasRight) return nodes[i]->key;
            if (kind == 1 &&  hasLeft && !hasRight) return nodes[i]->key;
            if (kind == 2 && !hasLeft &&  hasRight) return nodes[i]->key;
            // skip the root for kind 3, the root is tested as its own case
            if (kind == 3 &&  hasLeft &&  hasRight && i > 0) return nodes[i]->key;
        }
        return "(none)";
    }

    // Task 3(b). Prints the first few levels, root first.
    void levelOrderTop(int levels)
    {
        if (root == nullptr) { cout << "  (empty)\n"; return; }

        vector<Node*> nodes;
        vector<int>   depths;
        collect(nodes, depths);

        int n = (int)nodes.size();
        for (int d = 0; d < levels; d++)
        {
            cout << "  level " << d << ":";
            for (int i = 0; i < n; i++)
            {
                if (depths[i] == d) cout << " " << nodes[i]->key;
            }
            cout << "\n";
        }
    }

    // Task 4(c), used here after every delete.
    bool validate()
    {
        return isBST(root, false, "", false, "");
    }


    // Task 4(a). Iterative inorder, so a deep tree cannot overflow the stack.
    void inorder(vector<string> &out)
    {
        out.clear();
        vector<Node*> stack;
        Node* cur = root;

        while (cur != nullptr || stack.size() > 0)
        {
            while (cur != nullptr)
            {
                stack.push_back(cur);
                cur = cur->left;
            }
            cur = stack.back();
            stack.pop_back();
            out.push_back(cur->key);
            cur = cur->right;
        }
    }

    // Task 4(b). Preorder is the order that rebuilds the identical tree.
    void preorder(vector<string> &out)
    {
        out.clear();
        if (root == nullptr) return;

        vector<Node*> stack;
        stack.push_back(root);

        while (stack.size() > 0)
        {
            Node* node = stack.back();
            stack.pop_back();
            out.push_back(node->key);
            if (node->right != nullptr) stack.push_back(node->right);
            if (node->left  != nullptr) stack.push_back(node->left);
        }
    }

    // Task 4(b). Level order of the whole tree, used to compare shapes.
    void levelOrder(vector<string> &out)
    {
        out.clear();
        vector<Node*> nodes;
        vector<int>   depths;
        collect(nodes, depths);

        int n = (int)nodes.size();
        for (int i = 0; i < n; i++)
        {
            out.push_back(nodes[i]->key);
        }
    }

    void clear()
    {
        vector<Node*> nodes;
        vector<int>   depths;
        collect(nodes, depths);

        int n = (int)nodes.size();
        for (int i = 0; i < n; i++)
        {
            delete nodes[i];
        }
        root = nullptr;
        count = 0;
    }


    // Task 5(a). Returns the airports with a code from `from` to `to`,
    // and the number of nodes the query looked at.
    void range(string from, string to, vector<Airport> &out, int &visited)
    {
        out.clear();
        visited = 0;
        rangeNode(root, from, to, out, visited);
    }


    // Task 4(a), records rather than just keys. Used to get the sorted array.
    void inorderRecords(vector<Airport> &out)
    {
        out.clear();
        vector<Node*> stack;
        Node* cur = root;

        while (cur != nullptr || stack.size() > 0)
        {
            while (cur != nullptr)
            {
                stack.push_back(cur);
                cur = cur->left;
            }
            cur = stack.back();
            stack.pop_back();
            out.push_back(cur->data);
            cur = cur->right;
        }
    }

    // Task 6. Rebuilds this tree balanced from a sorted array of records.
    void buildBalanced(vector<Airport> &sortedRecords)
    {
        clear();
        root = buildBalancedNode(sortedRecords, 0, (int)sortedRecords.size() - 1);
    }

    // Task 6. The rebalance command: take this tree's own keys in order
    // and rebuild it with the middle element as the root.
    void rebalance()
    {
        vector<Airport> sorted;
        inorderRecords(sorted);
        buildBalanced(sorted);
    }

    // Edges, so an empty tree is -1 and one node is 0.
    int height()
    {
        if (root == nullptr) return -1;

        vector<Node*> nodes;
        vector<int>   depths;
        collect(nodes, depths);

        int maxDepth = 0;
        int n = (int)depths.size();
        for (int i = 0; i < n; i++)
        {
            if (depths[i] > maxDepth) maxDepth = depths[i];
        }
        return maxDepth;
    }

    double averageDepth()
    {
        if (count == 0) return 0.0;

        vector<Node*> nodes;
        vector<int>   depths;
        collect(nodes, depths);

        long long total = 0;
        int n = (int)depths.size();
        for (int i = 0; i < n; i++)
        {
            total = total + depths[i];
        }
        return (double)total / (double)n;
    }
};

#endif
