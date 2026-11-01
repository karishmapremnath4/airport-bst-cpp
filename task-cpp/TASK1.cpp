// EECE 7205 Mini Project - Airport Code Lookup Service with a BST
// Task 1: load the data, build the tree, measure its shape.
// Karishma Premnath, Mithila Krishna Shashikala
// Data: OpenFlights.org, Open Database License.

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iomanip>

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

bool loadAirports(const string &fileName, vector<Airport> &records,
                  int &skipped, int &totalLines)
{
    ifstream in(fileName);

    if (!in)
    {
        cout << "Error: cannot open " << fileName << '\n';
        return false;
    }

    records.clear();
    skipped = 0;
    totalLines = 0;

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
        totalLines++;

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

void showFields(const string &label, const string &line)
{
    vector<string> f = splitLine(line);
    cout << label << "\n";
    cout << "    name = [" << f[1] << "]\n";
    cout << "    iata = [" << f[4] << "]   icao = [" << f[5] << "]\n";
}

string getRawLine(const string &fileName, int ln)
{
    ifstream in(fileName);
    string line;
    int i = 0;
    while (getline(in, line))
    {
        i++;
        if (i == ln) return line;
    }
    return "";
}

// Counts distinct countries with a linear search, so no sorting is needed.
int countCountries(vector<Airport> &records)
{
    vector<string> seen;
    int n = (int)records.size();
    for (int i = 0; i < n; i++)
    {
        bool found = false;
        int m = (int)seen.size();
        for (int j = 0; j < m; j++)
        {
            if (seen[j] == records[i].country) { found = true; break; }
        }
        if (!found) seen.push_back(records[i].country);
    }
    return (int)seen.size();
}

int countInCountry(vector<Airport> &records, string country)
{
    int total = 0;
    int n = (int)records.size();
    for (int i = 0; i < n; i++)
    {
        if (records[i].country == country) total++;
    }
    return total;
}

int main(int argc, char* argv[])
{
    string fileName = "data/airports.dat";
    if (argc > 1) fileName = argv[1];

    // ---------------- Task 1(a): load the data ----------------
    cout << "===== Task 1(a): load the data =====\n";

    vector<Airport> records;
    int skipped = 0;
    int totalLines = 0;

    if (!loadAirports(fileName, records, skipped, totalLines))
        return 1;

    int n = (int)records.size();
    cout << "Loaded " << n << " airports ("
         << skipped << " lines skipped: no IATA code).\n";

    cout << "\nLoader checked against the facts in the handout:\n";
    cout << "  lines in total         " << totalLines
         << "      expected 7698\n";
    cout << "  3-letter IATA codes    " << n
         << "      expected 6072\n";
    cout << "  skipped (no code)      " << skipped
         << "      expected 1626\n";
    cout << "  countries covered      " << countCountries(records)
         << "       expected 235\n";
    cout << "  in the United States   " << countInCountry(records, "United States")
         << "      expected 1251\n";

    cout << "\nsplitLine on the two names that contain a comma:\n";
    showFields("  line 633:", getRawLine(fileName, 633));
    showFields("  line 654:", getRawLine(fileName, 654));

    cout << "\nOne stored record:\n";
    for (int i = 0; i < n; i++)
    {
        if (records[i].iata == "BOS")
        {
            cout << "  " << records[i].toString() << "\n";
            break;
        }
    }

    // ---------------- Task 1(b): build the tree ----------------
    cout << "\n===== Task 1(b): build the BST in file order =====\n";

    AirportBST tree;
    int inserted   = 0;
    int duplicates = 0;

    for (int i = 0; i < n; i++)
    {
        if (tree.insert(records[i].iata, records[i]))
            inserted++;
        else
        {
            duplicates++;
            cout << "  duplicate not inserted: " << records[i].iata
                 << "  (" << records[i].name << ")\n";
        }
    }

    cout << "Keys inserted: " << inserted << "\n";
    cout << "Duplicates:    " << duplicates << "\n";
    cout << "Root:          " << tree.rootKey() << "\n";
    cout << "Nodes in tree: " << tree.size() << "\n";

    // ---------------- Task 1(c): measure the tree ----------------
    cout << "\n===== Task 1(c): shape of the tree =====\n";
    cout << "Height:        " << tree.height() << "\n";
    cout << "Average depth: " << fixed << setprecision(1)
         << tree.averageDepth() << "\n";

    return 0;
}
