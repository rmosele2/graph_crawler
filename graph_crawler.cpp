#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <unordered_set>
#include <tuple>
#include <curl/curl.h> // For making HTTP requests
#include "rapidjson/document.h" // For parsing JSON data

using namespace std;

// Callback function to write CURL response into a string
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, string* output) {
    size_t totalSize = size * nmemb;
    output->append((char*)contents, totalSize);
    return totalSize;
}

// Function to get neighbors of a node from the web API
vector<string> getNeighbors(const string& node) {
    CURL* curl = curl_easy_init();
    string readBuffer;
    vector<string> neighbors;

    if (curl) {
        string url = "http://hollywood-graph-crawler.bridgesuncc.org/neighbors/" + node;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        rapidjson::Document d;
        d.Parse(readBuffer.c_str());
        if (d.HasMember("neighbors") && d["neighbors"].IsArray()) {
            for (auto& v : d["neighbors"].GetArray()) {
                neighbors.push_back(v.GetString());
            }
        }
    }
    return neighbors;
}

// BFS traversal that prints all nodes within a given depth
void bfs(const string& start, int maxDepth) {
    queue<pair<string, int>> q;
    unordered_set<string> visited;

    q.push({start, 0});
    visited.insert(start);

    while (!q.empty()) {
        auto [current, depth] = q.front();
        q.pop();

        cout << current << " (Depth: " << depth << ")\n";

        if (depth >= maxDepth) continue;

        for (const string& neighbor : getNeighbors(current)) {
            if (visited.find(neighbor) == visited.end()) {
                visited.insert(neighbor);
                q.push({neighbor, depth + 1});
            }
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " <starting_node> <depth>" << endl;
        return 1;
    }

    string startingNode = argv[1];
    int depth = stoi(argv[2]);

    if (depth < 0) {
        cerr << "Depth must be a non-negative integer." << endl;
        return 1;
    }

    curl_global_init(CURL_GLOBAL_DEFAULT);   // Initialize CURL globally
    bfs(startingNode, depth);                // Run BFS
    curl_global_cleanup();                   // Clean up CURL

    return 0;
}
