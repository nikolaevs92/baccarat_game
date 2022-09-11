#include <random>
#include <iostream>

class RNG {
public:
    int GetRand() {
        return rand() % 64;
    }
};

int GetDieRoll(RNG & random) {
    int rngRand = random.GetRand();
    if ((rngRand / 6) > 9 ) {
        return GetDieRoll(random);
    } 
    return (rngRand % 6) + 1;
}

int main() {
    RNG random;
    for (int i = 0; i <10; i++) {
        std::cout << GetDieRoll(random);
    }
    return 0;
}