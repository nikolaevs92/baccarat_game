#include <vector>
#include <iostream>

template<typename Iterator>
void Sort(Iterator begin, Iterator end);

// Very unoptimized, but simple
template<typename Iterator>
void Sort(Iterator begin, Iterator end) {
    for (auto left =  begin; left < end; left++ ) {
        for (auto right = left + 1; right < end; right++) {
            if (!((*left) < (*right))) {
                std::swap(*left, *right);
            }
        }
    }
}

int main(){
    std::vector<int> v{1,2,3};
    Sort(v.begin(), v.end());

    for (auto a : v) {
        std::cout << a << std::endl;
    }
    return 0;
}