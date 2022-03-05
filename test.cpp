// clang-format off
#include <bits/stdc++.h>
using namespace std;

typedef long long ll;

#define endl '\n'
#define INF (int) 1e9
#define BIG_INF (ll) 1e18
#define MOD ((ll)1e9 + 7)

#define rep(x,start,end) for(auto x=(start)-((start)>(end));x!=(end)-((start)>(end));((start)<(end)?x++:x--))
#define all(x) (x).begin(),(x).end()
#define sz(x) (ll)(x).size()

#define read(a,n) vi a(n); rep(i, 0, n) cin >> a[i];
#define print(a,n) rep(i, 0, n) cout << a[i] << ' '; cout << endl;

#define vi vector<int>
#define pii pair<int, int>

#define vll vector<ll>
#define pll pair<ll, ll>
// clang-format on

int main() {
  set<int> a({1, 2, 3, 4}), b({2, 3});

  cout << includes(all(a), all(b)) << " " << includes(all(b), all(a)) << endl;

  return 0;
}