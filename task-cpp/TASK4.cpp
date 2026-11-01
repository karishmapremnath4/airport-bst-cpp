// EECE 7205 Mini Project - Airport Code Lookup Service with a BST
// Task 4: traversals and validation.
// Karishma Premnath, Mithila Krishna Shashikala
// Data: OpenFlights.org, Open Database License.

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

class Airport
{
public:
    string name;
    string city;
    string country;
    string iata;        // the BST key
    string icao;
    double lat;
    double lon;

    Airport()
    {
        lat = 0.0;
        lon = 0.0;
    }

    Airport(string name, string city, string country,
            string iata, string icao, double lat, double lon)
    {
        this->name    = name;
        this->city    = city;
        this->country = country;
        this->iata    = iata;
        this->icao    = icao;
        this->lat     = lat;
        this->lon     = lon;
    }

    string toString()
    {
        return iata + "  " + name + ", " + city + ", " + country;
    }
};

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

// Splitting on every comma would cut "Harstad/Narvik Airport, Evenes" in two,
// so a comma only separates fields when we are outside quotes.
vector<string> splitLine(const string &line)
{
    vector<string> fields;
    string field = "";
    bool inQuotes = false;

    for (int i = 0; i < (int)line.size(); i++)
    {
        char c = line[i];
        if (c == '"')
        {
            if (inQuotes && i + 1 < (int)line.size() && line[i + 1] == '"')
            {
                field = field + '"';
                i++;
            }
            else
                inQuotes = !inQuotes;
        }
        else if (c == ',' && !inQuotes)
        {
            fields.push_back(field);
            field = "";
        }
        else
            field = field + c;
    }
    fields.push_back(field);
    return fields;
}

bool loadAirports(const string &fileName, vector<Airport> &records, int &skipped)
{
    ifstream in(fileName);

    if (!in)
    {
        cout << "Error: cannot open " << fileName << '\n';
        return false;
    }

    records.clear();
    skipped = 0;

    string line;
    bool firstLine = true;

    while (getline(in, line))
    {
        if (line.size() == 0) continue;

        if (firstLine)
        {
            firstLine = false;
            if (line.substr(0, 10) == "airport_id") continue;   // .csv header
        }

        vector<string> f = splitLine(line);

        if ((int)f.size() < 9) continue;

        // airports.dat writes a missing code as \N, airports.csv leaves it empty
        if (f[4] == "\\N" || f[4].size() == 0)
        {
            skipped++;
            continue;
        }

        stringstream latStream(f[6]);
        stringstream lonStream(f[7]);
        double lat = 0.0;
        double lon = 0.0;
        latStream >> lat;
        lonStream >> lon;

        Airport a(f[1], f[2], f[3], f[4], f[5], lat, lon);
        records.push_back(a);
    }

    in.close();
    return true;
}


void buildTree(AirportBST &tree, vector<Airport> &records)
{
    int n = (int)records.size();
    for (int i = 0; i < n; i++)
    {
        tree.insert(records[i].iata, records[i]);
    }
}

// Task 4(a). Strictly increasing means the inorder list is sorted.
bool isSorted(vector<string> &out)
{
    int n = (int)out.size();
    for (int i = 0; i < n - 1; i++)
    {
        if (out[i + 1] <= out[i]) return false;
    }
    return true;
}

// Task 4(b). Writes one code per line, in preorder.
bool savePreorder(AirportBST &tree, const string &fileName)
{
    vector<string> out;
    tree.preorder(out);

    ofstream f(fileName);
    if (!f) { cout << "Error: cannot write " << fileName << "\n"; return false; }

    int n = (int)out.size();
    for (int i = 0; i < n; i++)
    {
        f << out[i] << "\n";
    }
    f.close();
    return true;
}

// Task 4(b). Rebuilds a tree from a file of codes, looking each record up.
bool loadPreorder(AirportBST &tree, const string &fileName, vector<Airport> &records)
{
    ifstream f(fileName);
    if (!f) { cout << "Error: cannot read " << fileName << "\n"; return false; }

    tree.clear();

    string code;
    int n = (int)records.size();
    while (getline(f, code))
    {
        if (code.size() == 0) continue;
        for (int i = 0; i < n; i++)
        {
            if (records[i].iata == code)
            {
                tree.insert(code, records[i]);
                break;
            }
        }
    }
    f.close();
    return true;
}

bool sameShape(AirportBST &a, AirportBST &b)
{
    if (a.height() != b.height()) return false;

    vector<string> la, lb;
    a.levelOrder(la);
    b.levelOrder(lb);

    if (la.size() != lb.size()) return false;

    int n = (int)la.size();
    for (int i = 0; i < n; i++)
    {
        if (la[i] != lb[i]) return false;
    }
    return true;
}

void showEnds(vector<string> &out, int howMany)
{
    int n = (int)out.size();
    cout << "  first " << howMany << ":";
    for (int i = 0; i < howMany && i < n; i++) cout << " " << out[i];
    cout << "\n  last  " << howMany << ":";
    for (int i = 0; i < howMany; i++)
    {
        int k = n - howMany + i;
        if (k >= 0) cout << " " << out[k];
    }
    cout << "\n";
}

int main(int argc, char* argv[])
{
    string fileName = "data/airports.dat";
    if (argc > 1) fileName = argv[1];

    vector<Airport> records;
    int skipped = 0;

    if (!loadAirports(fileName, records, skipped))
        return 1;

    AirportBST tree;
    buildTree(tree, records);

    cout << "Loaded " << tree.size() << " airports. Height "
         << tree.height() << ".\n";

    // ---------------- Task 4(a): list ----------------
    cout << "\n===== Task 4(a): list, in alphabetical code order =====\n";

    vector<string> inord;
    tree.inorder(inord);

    cout << "  codes listed: " << (int)inord.size() << "\n";
    showEnds(inord, 10);
    cout << "  strictly increasing: " << (isSorted(inord) ? "YES" : "NO") << "\n";

    ofstream listFile("data/list_inorder.txt");
    for (int i = 0; i < (int)inord.size(); i++)
    {
        for (int j = 0; j < (int)records.size(); j++)
        {
            if (records[j].iata == inord[i])
            {
                listFile << records[j].toString() << "\n";
                break;
            }
        }
    }
    listFile.close();
    cout << "  full list written to data/list_inorder.txt\n";

    // ---------------- Task 4(b): save and load in preorder ----------------
    cout << "\n===== Task 4(b): save in preorder, load it back =====\n";

    vector<string> pre;
    tree.preorder(pre);
    cout << "  preorder codes: " << (int)pre.size() << "\n";
    showEnds(pre, 10);

    cout << "> save data/preorder.txt\n";
    savePreorder(tree, "data/preorder.txt");

    AirportBST reloaded;
    cout << "> load data/preorder.txt\n";
    loadPreorder(reloaded, "data/preorder.txt", records);

    cout << "  original  nodes " << tree.size()
         << "   height " << tree.height() << "\n";
    cout << "  reloaded  nodes " << reloaded.size()
         << "   height " << reloaded.height() << "\n";

    cout << "  top three levels, original:\n";
    tree.levelOrderTop(3);
    cout << "  top three levels, reloaded:\n";
    reloaded.levelOrderTop(3);

    cout << "  same shape (height and full level order): "
         << (sameShape(tree, reloaded) ? "YES" : "NO") << "\n";

    // ---------------- Task 4(c): validate ----------------
    cout << "\n===== Task 4(c): validate with the (lo, hi) method =====\n";

    cout << "  after the build         : "
         << (tree.validate() ? "OK" : "FAILED") << "\n";
    cout << "  reloaded tree           : "
         << (reloaded.validate() ? "OK" : "FAILED") << "\n";

    Airport mine("Karishma Premnath Airport", "Boston", "United States",
                 "QQQ", "KQQQ", 42.3601, -71.0589);
    tree.insert(mine.iata, mine);
    cout << "  after inserting QQQ     : "
         << (tree.validate() ? "OK" : "FAILED") << "\n";

    string caseUsed;
    tree.remove("QQQ", caseUsed);
    cout << "  after removing QQQ      : "
         << (tree.validate() ? "OK" : "FAILED") << "\n";

    tree.remove("BOS", caseUsed);
    cout << "  after removing BOS      : "
         << (tree.validate() ? "OK" : "FAILED") << "\n";

    tree.remove(tree.rootKey(), caseUsed);
    cout << "  after removing the root : "
         << (tree.validate() ? "OK" : "FAILED") << "\n";

    vector<string> after;
    tree.inorder(after);
    cout << "  inorder still sorted    : "
         << (isSorted(after) ? "YES" : "NO")
         << "   (" << (int)after.size() << " codes)\n";

    return 0;
}
