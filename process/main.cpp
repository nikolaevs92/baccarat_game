#include <iostream>
#include <string>
#include <vector>
#include <sstream>


void Process(std::istream &input, std::ostream &output) {
    std::vector<std::string> result;
    std::string line, trash, first, second, fourth;
    while(std::getline(input, line)) {
        std::istringstream is(line);
        is >> first >> trash >> second >> trash >> trash >> trash >> fourth;
        result.push_back(first + " " + second + fourth);
    }

    for (const auto &line : result) {
        output << line << std::endl;
    }
}

int main() {
    Process(std::cin, std::cout);
    return 0;
}