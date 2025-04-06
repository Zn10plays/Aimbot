#include <vector>
#include <Arduino.h>

struct CommandData {
    std::string command;
    int x;
    int y;
    int xerror;
    int yerror;
    long long time; // Use long long for large timestamps
    long recieved_time; // Use long long for large timestamps
};

// Define the pipeMessage function to match the arguments used in comms.cpp
void pipeMessage(CommandData* source, CommandData* destination) {
    destination->command = source->command;
    destination->x = source->x;
    destination->y = source->y;
    destination->xerror = source->xerror;
    destination->yerror = source->yerror;
    destination->time = source->time;
}

void parseMessage(const std::string& message, CommandData* returnDest) {
    CommandData data;
    size_t start = 0, end;
    std::vector<std::string> tokens;

    // Tokenize the message using ',' as a delimiter
    while ((end = message.find(',', start)) != std::string::npos) {
        tokens.push_back(message.substr(start, end - start));
        start = end + 1;
    }
    tokens.push_back(message.substr(start)); // Last token

    // Assign values to struct fields
    if (tokens.size() == 6) {
        data.command = tokens[0];
        data.x = std::stoi(tokens[1]);
        data.y = std::stoi(tokens[2]);
        data.xerror = std::stof(tokens[3]);
        data.yerror = std::stof(tokens[4]);
        data.time = std::stoll(tokens[5]);
    }

    data.recieved_time = millis(); // Get the current time in milliseconds

    pipeMessage(&data, returnDest);
}

float max(float a, float b) {
    return (a > b) ? a : b;
}
float min(float a, float b) {
    return (a < b) ? a : b;
}