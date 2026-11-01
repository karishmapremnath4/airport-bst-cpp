// EECE 7205 Mini Project - Airport Code Lookup Service with a BST
// Task 3: delete, covering the three deletion cases.
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


// Loads the file and builds the tree in file order.
void buildTree(AirportBST &tree, vector<Airport> &records)
{
    int n = (int)records.size();
    for (int i = 0; i < n; i++)
    {
        tree.insert(records[i].iata, records[i]);
    }
}

// Task 3(b). One delete, on a freshly built tree so each case is independent.
void runRemove(vector<Airport> &records, string code, string label)
{
    AirportBST tree;
    buildTree(tree, records);

    cout << "> remove " << code << "   (" << label << ")\n";
    cout << "  top three levels before:\n";
    tree.levelOrderTop(3);

    string caseUsed;
    bool ok = tree.remove(code, caseUsed);

    if (ok)
        cout << "  Deleted " << code << " (case " << caseUsed << ")\n";
    else
        cout << "  " << code << " not in the tree\n";

    cout << "  top three levels after:\n";
    tree.levelOrderTop(3);
    cout << "  nodes: " << tree.size()
         << "   validate: " << (tree.validate() ? "OK" : "FAILED") << "\n\n";
}

int main(int argc, char* argv[])
{
    string fileName = "data/airports.dat";
    if (argc > 1) fileName = argv[1];

    vector<Airport> records;
    int skipped = 0;

    if (!loadAirports(fileName, records, skipped))
        return 1;

    AirportBST probe;
    buildTree(probe, records);

    cout << "Loaded " << probe.size() << " airports. Height "
         << probe.height() << ".\n";

    // ---------------- Task 3(a): remove CODE ----------------
    cout << "\n===== Task 3(a): remove CODE =====\n\n";

    runRemove(records, "BOS", "the example from the handout");

    // ---------------- Task 3(b): one code per case, tested ----------------
    string leafCode  = probe.findCase(0);
    string leftOnly  = probe.findCase(1);
    string rightOnly = probe.findCase(2);
    string twoKids   = probe.findCase(3);
    string rootCode  = probe.rootKey();

    cout << "===== Task 3(b): one code from the tree for each case =====\n";
    cout << "  D1 a leaf              : " << leafCode  << "\n";
    cout << "  D2 only a left child   : " << leftOnly  << "\n";
    cout << "  D2 only a right child  : " << rightOnly << "\n";
    cout << "  D3 two children        : " << twoKids   << "\n";
    cout << "  the root               : " << rootCode  << "\n\n";

    runRemove(records, leafCode,  "D1, a leaf");
    runRemove(records, leftOnly,  "D2, only a left child");
    runRemove(records, rightOnly, "D2, only a right child");
    runRemove(records, twoKids,   "D3, two children");
    runRemove(records, rootCode,  "the root");

    // ---------------- Task 3(c): add QQQ, find it, remove it --------------
    cout << "===== Task 3(c): add QQQ, find it, then remove it =====\n\n";

    AirportBST tree;
    buildTree(tree, records);

    Airport mine("Karishma Premnath Airport", "Boston", "United States",
                 "QQQ", "KQQQ", 42.3601, -71.0589);

    cout << "> add QQQ\n";
    bool added = tree.insert(mine.iata, mine);
    cout << "  inserted: " << (added ? "yes" : "no (duplicate)")
         << "   nodes: " << tree.size()
         << "   validate: " << (tree.validate() ? "OK" : "FAILED") << "\n\n";

    vector<string> path;
    Airport found;
    cout << "> find QQQ\n";
    if (tree.find("QQQ", path, found))
    {
        cout << "  " << found.toString() << "\n";
        cout << "  Comparisons: " << (int)path.size() << "\n\n";
    }
    else
    {
        cout << "  not found\n\n";
    }

    string caseUsed;
    cout << "> remove QQQ\n";
    tree.remove("QQQ", caseUsed);
    cout << "  Deleted QQQ (case " << caseUsed << ")\n";
    cout << "  nodes: " << tree.size()
         << "   validate: " << (tree.validate() ? "OK" : "FAILED") << "\n\n";

    cout << "> find QQQ again\n";
    if (tree.find("QQQ", path, found))
        cout << "  still found\n";
    else
        cout << "  not found, as expected\n";

    return 0;
}
