#pragma once
#include <vector>
#include <limits>

struct FenwickNode{

    int maxScore;
    int bestId;
};

class Fenwick{

private:
    int n;
    std::vector<FenwickNode> bit;

public:
    Fenwick(int size);

    void update(int index, int score, int id);
    FenwickNode query(int index);

};