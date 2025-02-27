#include <iostream>
#include <vector>


using namespace std;

int main() {
    int64_t n; int k; cin >> n >> k;
    vector<int64_t> d(k);
    for(int i = 0; i < k; i++) cin >> d[i];

    vector<int64_t> st(k+1), en(k+1), le(k+1, 0LL);

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

    vector<int64_t> prefix(k+1);
    prefix[0] = le[0];
    for(int i = 1; i <= k; i++) prefix[i] = prefix[i - 1] + le[i];

    int q; cin >> q;
    while(q--) {
        int64_t p; cin >> p;
        if(p > prefix[k]) {
            cout << -1 << std::endl;
            continue;
        }
        auto it = std::lower_bound(prefix.begin(), prefix.end(), p);
        int idx = static_cast<int>(it - prefix.begin());

        int64_t before = (idx == 0 ? 0LL : prefix[idx - 1]);
        cout << (st[idx] + (p - before) - 1) << std::endl;
    }
    return 0;
}