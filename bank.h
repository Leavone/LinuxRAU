#pragma once

class Cell {
private:
        int cur_balance;
        int min_balance;
        int max_balance;
        bool is_frozen;

public:
        Cell() : cur_balance(0), min_balance(0), max_balance(1000000), is_frozen(false) {}
};

class Bank{
public:
        Bank(int N) {
                size = N;
                for(int i = 0; i < size; ++i) {
                        cells[i] = Cell();
                }
        }
private:
        size_t size;
        Cell cells[];
};
