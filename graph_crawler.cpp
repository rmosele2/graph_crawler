#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <unordered_set>
#include <tuple>
#include <chrono>
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

    // URL-encode the node (replace spaces with %20)
    string encodedNode;
    for (char c : node) {
        if (c == ' ')
            encodedNode += "%20";
        else
            encodedNode += c;
    }

    if (curl) {
        string url = "http://hollywood-graph-crawler.bridgesuncc.org/neighbors/" + encodedNode;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_perform(curl);

        // Check if the response is a valid JSON object
        if (readBuffer.empty() || readBuffer[0] != '{') {
            cerr << "Invalid JSON response for node: " << node << endl;
            return neighbors; // return empty vector
        }

        curl_easy_cleanup(curl);

        cout << "Fetching neighbors of: " << node << endl;
        cout << "Response: " << readBuffer << endl;

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

#include <chrono>  // Add this at the top of your file

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

    //Start timing
    auto startTime = chrono::high_resolution_clock::now();

    bfs(startingNode, depth);

    //End timing
    auto endTime = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = endTime - startTime;

    cout << "Traversal completed in " << duration.count() << " seconds." << endl;

    curl_global_cleanup();                   // Cleanup CURL globally

    return 0;
}
// Compile with: g++ -o graph_crawler graph_crawler.cpp -lcurl -ljsoncpp
// Run with: ./graph_crawler <starting_node> <depth>
