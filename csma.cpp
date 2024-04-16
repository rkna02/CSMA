#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>

struct Node {
    int id;
    int backoff;
    int collisionCount;
    bool isTransmitting;
    int transmissionEndTime;
};

// Function to calculate the initial backoff using the provided formula
int calculateInitialBackoff(int nodeId, int R) {
    return nodeId % R;
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

    while (inputFile >> param && inputFile >> std::ws) {
        if (param == 'N' || param == 'L' || param == 'M' || param == 'T') {
            inputFile >> (param == 'N' ? N : param == 'L' ? L : param == 'M' ? M : T);
        } else if (param == 'R') {
            while (inputFile >> R) {
                R_values.push_back(R);
                if (inputFile.peek() == '\n' || inputFile.eof()) break;
            }
        }
    }
    inputFile.close();

    std::vector<Node> nodes(N);
    int successfulTransmissionTicks = 0;
    bool channelBusy = false;

    for (int i = 0; i < N; i++) {
        nodes[i].id = i;
        nodes[i].backoff = calculateInitialBackoff(i, R_values[0]);
        nodes[i].collisionCount = 0;
        nodes[i].isTransmitting = false;
        nodes[i].transmissionEndTime = -1;
    }

    for (int time = 0; time < T; ++time) {
        std::vector<int> readyNodes;
        for (Node& node : nodes) {
            if (node.isTransmitting && time == node.transmissionEndTime) {
                node.isTransmitting = false;
                channelBusy = false;
            } else if (!node.isTransmitting && node.backoff == 0 && !channelBusy) {
                readyNodes.push_back(node.id);
            } else if (!node.isTransmitting && !channelBusy) {
                node.backoff--;
            }
        }

        if (readyNodes.size() == 1 && !channelBusy) {
            int nodeId = readyNodes.front();
            nodes[nodeId].isTransmitting = true;
            nodes[nodeId].transmissionEndTime = time + L;
            successfulTransmissionTicks++; // Increment for each successful transmission start
            channelBusy = true;
        } else if (readyNodes.size() > 1) {
            for (int id : readyNodes) {
                Node& node = nodes[id];
                node.collisionCount++;
                if (node.collisionCount > M) {
                    node.collisionCount = 0;
                    node.backoff = calculateInitialBackoff(id, R_values[0]);
                } else {
                    int currentRIndex = std::min(node.collisionCount - 1, (int)R_values.size() - 1);
                    node.backoff = calculateInitialBackoff(id, R_values[currentRIndex]);
                }
            }
        }
    }

    double utilizationRate = (double)successfulTransmissionTicks / T;

    std::ofstream outputFile("output.txt");
    if (outputFile) {
        outputFile.precision(2);
        outputFile << std::fixed << utilizationRate << std::endl;
        outputFile.close();
    } else {
        std::cerr << "Error: Could not write to output file." << std::endl;
        return 1;
    }

    return 0;
}






