#include <iostream>
#include <string>

using namespace std;

void solve() 
{
    long long x;
    cin >> x;
    string s;
    cin >> s;

    int carlsen = 0;
    int chef = 0;

    for (char c : s) 
    {
        if (c == 'C') 
        {
            carlsen += 2;
        } 
        else if (c == 'N') 
        {
            chef += 2;
        } 
        else 
        {
            carlsen += 1;
            chef += 1;
        }
    }

    if (carlsen > chef) 
    {
        cout << 60 * x << "\n";
    } 
    else if (chef > carlsen) 
    {
        cout << 40 * x << "\n";
    } 
    else 
    {
        cout << 55 * x << "\n";
    }
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    int t;
    cin >> t;
    while (t--) {
        solve();
    }
    return 0;
}
