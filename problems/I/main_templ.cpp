#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

// clang++ -O2 main.cpp -o i_problem

/*
#include <iostream>
#include <vector>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    long long n; int k; cin >> n >> k;
    vector<long long> d(k);
    for(int i = 0; i < k; i++) cin >> d[i];

    vector<long long> st(k+1), en(k+1), le(k+1, 0LL);
    if(k == 0) {
        st[0] = 1; 
        en[0] = n;
        le[0] = en[0] - st[0] + 1;
    } else {
        st[0] = 1; 
        en[0] = d[0] - 1; 
        if(en[0] >= st[0]) le[0] = en[0] - st[0] + 1;
        for(int i = 1; i < k; i++) {
            st[i] = d[i-1] + 1; 
            en[i] = d[i] - 1;
            if(en[i] >= st[i]) le[i] = en[i] - st[i] + 1;
        }
        st[k] = d[k-1] + 1; 
        en[k] = n;
        if(en[k] >= st[k]) le[k] = en[k] - st[k] + 1;
    }

    vector<long long> pref(k+1);
    pref[0] = le[0];
    for(int i = 1; i <= k; i++) pref[i] = pref[i - 1] + le[i];

    int q; cin >> q;
    while(q--) {
        long long p; cin >> p;
        if(p > pref[k]) {
            cout << -1 << "\n";
            continue;
        }
        int low = 0, high = k;
        while(low < high) {
            int mid = (low + high) / 2;
            if(pref[mid] >= p) high = mid; 
            else low = mid + 1;
        }
        long long before = (low == 0 ? 0LL : pref[low - 1]);
        cout << st[low] + (p - before) - 1 << "\n";
    }
    return 0;
}
*/

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    // Read n (up to 10^18) and k (up to 10^5)
    long long n;
    int k;
    cin >> n >> k;

    // Read the k deleted numbers (sorted in ascending order)
    vector<long long> deleted(k);
    for (int i = 0; i < k; i++) {
        cin >> deleted[i];
    }

    // We will create up to k+1 blocks of remaining numbers.
    // Block i goes from start[i] to end[i] (inclusive).
    // Example:
    //   Block 0: [1, deleted[0] - 1]
    //   Block 1: [deleted[0] + 1, deleted[1] - 1]
    //   ...
    //   Block k: [deleted[k-1] + 1, n]
    //
    // We'll then compute the lengths of these blocks and build
    // a prefix sum array for quick queries.

    // We'll store the block starts, ends, and lengths.
    // We have k+1 blocks.
    vector<long long> blockStart(k+1), blockEnd(k+1), blockLen(k+1, 0LL);

    // Block 0
    if (k == 0) {
        // If there are no deleted numbers,
        // there's only one block from 1 .. n
        blockStart[0] = 1;
        blockEnd[0]   = n;
        blockLen[0]   = (blockEnd[0] - blockStart[0] + 1);
    } else {
        blockStart[0] = 1;
        blockEnd[0]   = deleted[0] - 1;
        if (blockEnd[0] >= blockStart[0]) {
            blockLen[0] = blockEnd[0] - blockStart[0] + 1;
        }

        // Middle blocks
        for (int i = 1; i < k; i++) {
            blockStart[i] = deleted[i - 1] + 1;
            blockEnd[i]   = deleted[i] - 1;
            if (blockEnd[i] >= blockStart[i]) {
                blockLen[i] = blockEnd[i] - blockStart[i] + 1;
            }
        }

        // Last block
        blockStart[k] = deleted[k - 1] + 1;
        blockEnd[k]   = n;
        if (blockEnd[k] >= blockStart[k]) {
            blockLen[k] = blockEnd[k] - blockStart[k] + 1;
        }
    }

    // Build prefix sums of blockLen
    // prefix[i] = blockLen[0] + blockLen[1] + ... + blockLen[i]
    vector<long long> prefix(k+1, 0LL);
    prefix[0] = blockLen[0];
    for (int i = 1; i <= k; i++) {
        prefix[i] = prefix[i - 1] + blockLen[i];
    }

    // Now answer q queries
    int q;
    cin >> q;
    while (q--) {
        long long p;
        cin >> p;

        // If p is larger than total remaining numbers,
        // there's no p-th remaining number
        if (p > prefix[k]) {
            cout << -1 << "\n";
            continue;
        }

        // Otherwise, we locate which block p belongs to.
        // We binary search for the smallest i such that prefix[i] >= p.
        int low = 0, high = k;
        while (low < high) {
            int mid = (low + high) / 2;
            if (prefix[mid] >= p) {
                high = mid;
            } else {
                low = mid + 1;
            }
        }
        int blockIndex = low;

        // The p-th remaining number is in blockIndex-th block.
        // We find how many we had before this block:
        long long before = (blockIndex == 0 ? 0LL : prefix[blockIndex - 1]);
        // We need offset from blockStart[blockIndex]
        long long offset = p - before; 
        // So the answer is:
        long long ans = blockStart[blockIndex] + offset - 1;

        cout << ans << "\n";
    }

    return 0;
}