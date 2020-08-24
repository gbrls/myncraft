//
// Created by gabriel on 8/23/20.
//
#include <bits/stdc++.h>

using namespace std;

set<int> G[8];

vector<int> ans;

void print_coord(int n) {
    int x = n&1, y = n&2, z = n&4;
    printf("X + %s, Y + %s, Z + %s, 0, 0, 0, 0, 10,\n", x?"SZ":"0", y?"SZ":"0", z?"SZ":"0");
}

vector<int> gen_code(int n) {
    if(n==0) {
        vector<int> v;
        v.push_back(1);
        v.push_back(0);
        return v;
    }

    auto prev = gen_code(n-1);
    vector<int> vec;

    for(auto e : prev) {
        vec.push_back((e<<1)|1);
    }

    reverse(prev.begin(), prev.end());

    for(auto e : prev) {
        vec.push_back((e<<1)|0);
    }

    return vec;
}

int main(void) {

    vector<int> code = gen_code(2);

    for(auto i : code) {
        print_coord(i);
    }

    return 0;
}

