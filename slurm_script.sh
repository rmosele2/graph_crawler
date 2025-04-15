#!/bin/bash
#SBATCH --job-name=graphcrawler
#SBATCH --partition=Centaurus
#SBATCH --time=00:05:00
#SBATCH --mem-per-cpu=4g

# Load any necessary modules
# module load gcc
# module load curl

# Change to the directory where the executable is located
cd $HOME/ITCS4145/graph_crawler

# Run a few example BFS traversals
./graph_crawler "Mel Gibson" 3
./graph_crawler "Tom Hanks" 2
./graph_crawler "Liam Neeson" 1

