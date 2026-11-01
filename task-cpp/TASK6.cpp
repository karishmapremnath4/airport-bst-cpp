// EECE 7205 Mini Project - Airport Code Lookup Service with a BST
// Task 6: the shape experiment.
// Karishma Premnath, Mithila Krishna Shashikala
// Data: OpenFlights.org, Open Database License.

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iomanip>
#include <chrono>
#include <cstdlib>
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


// Task 6(b). Fisher-Yates, seeded so the run is reproducible.
void shuffle(vector<Airport> &a)
{
    for (int i = (int)a.size() - 1; i > 0; i--)
    {
        int j = rand() % (i + 1);
        Airport tmp = a[i];
        a[i] = a[j];
        a[j] = tmp;
    }
}

// Task 6(c). The same list of codes is searched in every tree.
double timeSearches(AirportBST &tree, vector<string> &queries)
{
    vector<string> path;
    Airport record;

    chrono::steady_clock::time_point start = chrono::steady_clock::now();

    int n = (int)queries.size();
    for (int i = 0; i < n; i++)
    {
        tree.find(queries[i], path, record);
    }

    chrono::steady_clock::time_point stop = chrono::steady_clock::now();
    return chrono::duration<double, milli>(stop - start).count();
}

void reportRow(string label, AirportBST &tree, vector<string> &queries)
{
    double ms = timeSearches(tree, queries);
    cout << "  " << left << setw(24) << label
         << right << setw(8) << tree.height()
         << setw(16) << fixed << setprecision(1) << tree.averageDepth()
         << setw(14) << setprecision(1) << ms << " ms\n";
}

// The longest stretch of codes that arrive already in increasing order.
// A stretch of length k inserted in order builds a chain of k nodes.
int longestIncreasingRun(vector<Airport> &a, int &startAt)
{
    int best = 1, cur = 1, curStart = 0;
    startAt = 0;

    int n = (int)a.size();
    for (int i = 0; i < n - 1; i++)
    {
        if (a[i + 1].iata > a[i].iata)
        {
            cur++;
            if (cur > best) { best = cur; startAt = curStart; }
        }
        else
        {
            cur = 1;
            curStart = i + 1;
        }
    }
    return best;
}

// How many codes begin with the given letter, and how many of those
// belong to the given country.
void countPrefix(vector<Airport> &a, char letter, string country,
                 int &total, int &inCountry)
{
    total = 0;
    inCountry = 0;

    int n = (int)a.size();
    for (int i = 0; i < n; i++)
    {
        if (a[i].iata.size() > 0 && a[i].iata[0] == letter)
        {
            total++;
            if (a[i].country == country) inCountry++;
        }
    }
}

int main(int argc, char* argv[])
{
    string fileName = "data/airports.dat";
    if (argc > 1) fileName = argv[1];

    vector<Airport> records;
    int skipped = 0;

    if (!loadAirports(fileName, records, skipped))
        return 1;

    int n = (int)records.size();

    // ---- 1. file order -------------------------------------------------
    AirportBST fileOrder;
    for (int i = 0; i < n; i++)
    {
        fileOrder.insert(records[i].iata, records[i]);
    }

    // the sorted array comes from an inorder walk, which is already sorted
    vector<Airport> sortedRecords;
    fileOrder.inorderRecords(sortedRecords);

    // ---- 2. sorted order: a chain --------------------------------------
    AirportBST sortedOrder;
    for (int i = 0; i < n; i++)
    {
        sortedOrder.insert(sortedRecords[i].iata, sortedRecords[i]);
    }

    // ---- 3. balanced from the sorted array -----------------------------
    AirportBST balanced;
    balanced.buildBalanced(sortedRecords);

    // ---- 4. random shuffle (bonus) -------------------------------------
    srand(42);
    vector<Airport> shuffled = sortedRecords;
    shuffle(shuffled);

    AirportBST randomOrder;
    for (int i = 0; i < n; i++)
    {
        randomOrder.insert(shuffled[i].iata, shuffled[i]);
    }

    // ---- 10,000 random searches, the same ones for every tree ----------
    vector<string> queries;
    for (int i = 0; i < 10000; i++)
    {
        queries.push_back(records[rand() % n].iata);
    }

    cout << "\n===== Task 6: the shape experiment =====\n\n";

    cout << "  " << left << setw(24) << "Build order"
         << right << setw(8) << "Height"
         << setw(16) << "Average depth"
         << setw(17) << "10,000 searches" << "\n";
    cout << "  " << string(63, '-') << "\n";

    reportRow("File order",            fileOrder,   queries);
    reportRow("Sorted order",          sortedOrder, queries);
    reportRow("Balanced (middle rule)", balanced,   queries);
    reportRow("Random shuffle (bonus)", randomOrder, queries);

    // ---- the rebalance command -----------------------------------------
    AirportBST live;
    for (int i = 0; i < n; i++)
    {
        live.insert(records[i].iata, records[i]);
    }

    cout << "\n> load data/airports.dat\n";
    cout << "Height: " << live.height()
         << "   Average depth: " << fixed << setprecision(1)
         << live.averageDepth() << "\n";

    live.rebalance();

    cout << "> rebalance\n";
    cout << "Height: " << live.height()
         << "   Average depth: " << fixed << setprecision(1)
         << live.averageDepth() << "\n";

    // ---- figures used in the written explanation -----------------------
    int startAt = 0;
    int run = longestIncreasingRun(records, startAt);

    int shufStart = 0;
    int shufRun = longestIncreasingRun(shuffled, shufStart);

    int yTotal = 0, yCanada = 0;
    countPrefix(records, 'Y', "Canada", yTotal, yCanada);

    cout << "\n  longest already-increasing run, file order : " << run
         << "  (" << records[startAt].iata << " to "
         << records[startAt + run - 1].iata << ", "
         << records[startAt].country << ")\n";
    cout << "  longest already-increasing run, shuffled   : " << shufRun << "\n";
    cout << "  codes beginning with Y                     : " << yTotal
         << ", of which " << yCanada << " in Canada\n";


    return 0;
}
