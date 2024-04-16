#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cmath>

struct Node {
    int id;
    int backoff;
    int collisionCount;
    bool isTransmitting;
    int transmissionEndTime;
};

// Function to calculate the backoff using the pseudorandom number generator
int calculateBackoff(int nodeId, int ticks, int R) {
    return (nodeId + ticks) % R;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <inputfile>" << std::endl;
        return 1;
    }

    std::ifstream inputFile(argv[1]);
    if (!inputFile) {
        std::cerr << "Error: Could not open input file." << std::endl;
        return 1;
    }

    int N, L, M, T;
    std::vector<int> R_values;
    char param;
    int R;

    // Read input parameters
    while (inputFile >> param) {
        switch (param) {
            case 'N':
                inputFile >> N;
                break;
            case 'L':
                inputFile >> L;
                break;
            case 'M':
                inputFile >> M;
                break;
            case 'R':
                while (inputFile >> R) {
                    R_values.push_back(R);
                    if (inputFile.peek() == '\n') break;
                }
                break;
            case 'T':
                inputFile >> T;
                break;
            default:
                std::cerr << "Unknown parameter: " << param << std::endl;
                return 1;
        }
    }
    inputFile.close(); // Close the file after reading

    std::vector<Node> nodes(N);
    int successfulTransmissionTicks = 0;

    // Initialize nodes
    for (int i = 0; i < N; i++) {
        nodes[i].id = i;
        nodes[i].backoff = calculateBackoff(i, 0, R_values[0]);
        nodes[i].collisionCount = 0;
        nodes[i].isTransmitting = false;
        nodes[i].transmissionEndTime = -1;
    }

    // Simulation loop
    for (int globalTime = 0; globalTime < T; ++globalTime) {
        std::vector<int> readyNodes;
        
        // Decrease backoff and collect ready nodes
        for (auto& node : nodes) {
            if (!node.isTransmitting && node.transmissionEndTime <= globalTime) {
                if (node.backoff == 0) {
                    readyNodes.push_back(node.id);
                } else {
                    node.backoff--;
                }
            }
        }

        // Check for transmission or collision
        if (readyNodes.size() == 1) {
            // Successful transmission
            int nodeId = readyNodes.front();
            nodes[nodeId].isTransmitting = true;
            nodes[nodeId].transmissionEndTime = globalTime + L;
            successfulTransmissionTicks += L; // Count successful transmission ticks
        } else if (readyNodes.size() > 1) {
            // Collision handling
            for (int id : readyNodes) {
                Node& node = nodes[id];
                node.collisionCount++;
                int R_index = std::min(node.collisionCount, (int)R_values.size()) - 1;
                node.backoff = calculateBackoff(id, globalTime, R_values[R_index]);
            }
        }

        // Check for end of transmission
        for (auto& node : nodes) {
            if (node.isTransmitting && globalTime >= node.transmissionEndTime) {
                node.isTransmitting = false;
                node.collisionCount = 0;
                node.backoff = calculateBackoff(node.id, globalTime, R_values[0]);
            }
        }
    }

    // Calculate the utilization rate
    double utilizationRate = static_cast<double>(successfulTransmissionTicks) / T;

    // Write the utilization rate to an output file
    std::ofstream outputFile("output.txt");
    if (outputFile.is_open()) {
        outputFile.precision(2);
        outputFile << std::fixed << utilizationRate << std::endl;
        outputFile.close();
    } else {
        std::cerr << "Error: Could not write to output file." << std::endl;
        return 1;
    }

    return 0;
}
