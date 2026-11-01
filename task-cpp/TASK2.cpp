// EECE 7205 Mini Project - Airport Code Lookup Service with a BST
// Task 2: search, with the path and the comparison count.
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

// Task 2(a). Prints the search path, the comparison count and the airport.
void runFind(AirportBST &tree, string code, string label)
{
    vector<string> path;
    Airport record;
    bool found = tree.find(code, path, record);

    cout << "> find " << code << "   (" << label << ")\n";

    cout << "  Path: ";
    int n = (int)path.size();
    for (int i = 0; i < n; i++)
    {
        cout << path[i];
        if (i < n - 1) cout << " -> ";
        if ((i + 1) % 10 == 0 && i < n - 1) cout << "\n        ";
    }
    if (!found) cout << " -> (NULL)";
    cout << "\n";

    cout << "  Comparisons: " << n << "\n";
    if (found)
        cout << "  " << record.toString() << "\n";
    else
        cout << "  not found\n";
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
    int n = (int)records.size();
    for (int i = 0; i < n; i++)
    {
        tree.insert(records[i].iata, records[i]);
    }

    cout << "Loaded " << tree.size() << " airports. Height " << tree.height() << ".\n";

    // ---------------- Task 2(a): find CODE ----------------
    cout << "\n===== Task 2(a): find CODE, with path and comparisons =====\n\n";

    runFind(tree, "BOS", "the example from the handout");

    // ---------------- Task 2(b): every search case ----------------
    cout << "===== Task 2(b): every search case =====\n\n";

    runFind(tree, tree.rootKey(),    "the root");
    runFind(tree, tree.aLeafKey(),   "a leaf");
    runFind(tree, tree.deepestKey(), "a code deep in the tree");
    runFind(tree, "XYZ",             "a code that does not exist");
    runFind(tree, "AA",              "smaller than every key");
    runFind(tree, "ZZZ",             "larger than every key");

    cout << "Minimum code: " << tree.minKey() << "\n";
    cout << "Maximum code: " << tree.maxKey() << "\n";

    return 0;
}
