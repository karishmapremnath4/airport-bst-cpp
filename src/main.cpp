// EECE 7205 Mini Project - Airport Code Lookup Service with a BST
// The lookup service: load the data, then answer commands at a prompt.
// Karishma Premnath, Mithila Krishna Shashikala
// Data: OpenFlights.org, Open Database License.

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iomanip>
#include "Airport.h"
#include "AirportBST.h"

using namespace std;

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


// Task 4(b). Rebuilds the tree from a file of codes, one per line, in
// preorder. Each code is looked up in the records already in memory.
bool loadPreorder(AirportBST &tree, const string &fileName,
                  vector<Airport> &records, int &missing)
{
    ifstream in(fileName);
    if (!in)
    {
        cout << "Error: cannot open " << fileName << '\n';
        return false;
    }

    tree.clear();
    missing = 0;

    string code;
    int n = (int)records.size();
    while (getline(in, code))
    {
        if (code.size() == 0) continue;

        bool found = false;
        for (int i = 0; i < n; i++)
        {
            if (records[i].iata == code)
            {
                tree.insert(code, records[i]);
                found = true;
                break;
            }
        }
        if (!found) missing++;
    }
    in.close();
    return true;
}

// An airport file has commas. A preorder file is one code per line.
bool looksLikeAirportFile(const string &fileName)
{
    ifstream in(fileName);
    string line;
    while (getline(in, line))
    {
        if (line.size() == 0) continue;
        return line.find(',') != string::npos;
    }
    return false;
}

void showHelp()
{
    cout << "  load FILE        read the airport file and build the tree\n";
    cout << "  find CODE        look up one airport, with the search path\n";
    cout << "  range FROM TO    every airport with a code between the two\n";
    cout << "  remove CODE      delete an airport\n";
    cout << "  add              add QQQ, a made-up airport\n";
    cout << "  list [n]         codes in alphabetical order, first n of them\n";
    cout << "  save FILE        write the codes in preorder\n";
    cout << "  validate         check the BST property\n";
    cout << "  rebalance        rebuild the tree balanced\n";
    cout << "  help             this list\n";
    cout << "  quit             exit\n";
}

int main(int argc, char* argv[])
{
    AirportBST tree;
    vector<Airport> records;

    cout << "Airport code lookup service. Type help for the commands.\n";

    if (argc > 1)
    {
        int skipped = 0;
        if (loadAirports(argv[1], records, skipped))
        {
            int duplicates = 0;
            for (int i = 0; i < (int)records.size(); i++)
            {
                if (!tree.insert(records[i].iata, records[i])) duplicates++;
            }
            cout << "Loaded " << tree.size() << " airports ("
                 << skipped << " lines skipped: no IATA code). Duplicates: "
                 << duplicates << "\n";
        }
    }

    string line;
    cout << "> ";
    while (getline(cin, line))
    {
        stringstream ss(line);
        string cmd;
        ss >> cmd;

        if (cmd == "quit" || cmd == "exit")
        {
            break;
        }
        else if (cmd == "help")
        {
            showHelp();
        }
        else if (cmd == "load")
        {
            string f;
            ss >> f;

            if (!looksLikeAirportFile(f))
            {
                // a preorder file: rebuild from the records already loaded
                if (records.size() == 0)
                {
                    cout << "Load an airport file first, "
                         << "then a preorder file can be rebuilt from it.\n";
                }
                else
                {
                    int missing = 0;
                    if (loadPreorder(tree, f, records, missing))
                    {
                        cout << "Rebuilt " << tree.size()
                             << " airports from preorder";
                        if (missing > 0) cout << " (" << missing << " unknown codes)";
                        cout << ".\n";
                        cout << "Height: " << tree.height()
                             << "   Average depth: " << fixed << setprecision(1)
                             << tree.averageDepth() << "\n";
                    }
                }
            }
            else
            {
                int skipped = 0;
                if (loadAirports(f, records, skipped))
                {
                    tree.clear();
                    int duplicates = 0;
                    for (int i = 0; i < (int)records.size(); i++)
                    {
                        if (!tree.insert(records[i].iata, records[i])) duplicates++;
                    }

                    cout << "Loaded " << tree.size() << " airports ("
                         << skipped << " lines skipped: no IATA code). Duplicates: "
                         << duplicates << "\n";
                    cout << "Height: " << tree.height()
                         << "   Average depth: " << fixed << setprecision(1)
                         << tree.averageDepth() << "\n";
                }
            }
        }
        else if (cmd == "find")
        {
            string code;
            ss >> code;

            vector<string> path;
            Airport record;
            bool found = tree.find(code, path, record);

            cout << "Path: ";
            int n = (int)path.size();
            for (int i = 0; i < n; i++)
            {
                cout << path[i];
                if (i < n - 1) cout << " -> ";
                if ((i + 1) % 12 == 0 && i < n - 1) cout << "\n      ";
            }
            if (!found) cout << " -> (NULL)";
            cout << "   (comparisons: " << n << ")\n";

            if (found) cout << record.toString() << "\n";
            else       cout << "not found\n";
        }
        else if (cmd == "range")
        {
            string from, to;
            ss >> from >> to;

            vector<Airport> out;
            int visited = 0;
            tree.range(from, to, out, visited);

            cout << (int)out.size() << " airports, " << visited
                 << " nodes visited\n";
            for (int i = 0; i < (int)out.size() && i < 5; i++)
                cout << "  " << out[i].toString() << "\n";
            if ((int)out.size() > 5)
                cout << "  ... " << ((int)out.size() - 5) << " more\n";
        }
        else if (cmd == "remove")
        {
            string code;
            ss >> code;

            string caseUsed;
            if (tree.remove(code, caseUsed))
                cout << "Deleted " << code << " (case " << caseUsed << ")"
                     << "   validate: " << (tree.validate() ? "OK" : "FAILED")
                     << "\n";
            else
                cout << code << " not in the tree\n";
        }
        else if (cmd == "add")
        {
            Airport mine("Karishma Premnath Airport", "Boston", "United States",
                         "QQQ", "KQQQ", 42.3601, -71.0589);
            if (tree.insert(mine.iata, mine))
                cout << "Added QQQ   nodes: " << tree.size()
                     << "   validate: " << (tree.validate() ? "OK" : "FAILED")
                     << "\n";
            else
                cout << "QQQ already in the tree\n";
        }
        else if (cmd == "list")
        {
            int howMany = 20;
            ss >> howMany;

            vector<string> out;
            tree.inorder(out);

            int n = (int)out.size();
            for (int i = 0; i < howMany && i < n; i++)
                cout << out[i] << (((i + 1) % 15 == 0) ? "\n" : " ");
            cout << "\n" << n << " codes in total\n";
        }
        else if (cmd == "save")
        {
            string f;
            ss >> f;

            vector<string> out;
            tree.preorder(out);

            ofstream g(f);
            for (int i = 0; i < (int)out.size(); i++) g << out[i] << "\n";
            g.close();
            cout << "wrote " << (int)out.size() << " codes in preorder to " << f << "\n";
        }
        else if (cmd == "validate")
        {
            cout << "validate: " << (tree.validate() ? "OK" : "FAILED")
                 << "   nodes: " << tree.size()
                 << "   height: " << tree.height() << "\n";
        }
        else if (cmd == "rebalance")
        {
            tree.rebalance();
            cout << "Height: " << tree.height()
                 << "   Average depth: " << fixed << setprecision(1)
                 << tree.averageDepth() << "\n";
        }
        else if (cmd.size() > 0)
        {
            cout << "unknown command: " << cmd << "   (try help)\n";
        }

        cout << "> ";
    }

    cout << "\n";
    return 0;
}
