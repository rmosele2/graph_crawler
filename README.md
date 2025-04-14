# graph_crawler

This project implements a breadth-first search (BFS) over a movie-actor graph via a public web API. It fetches neighboring nodes using live HTTP requests and outputs all reachable nodes within a given depth.

# requirements
C++17 compiler (`g++`)
`libcurl` installed
`rapidjson` library

# compilation
make

make clean

./graph_crawler "<starting_node>" <depth>