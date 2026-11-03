#include <iostream>
#include <vector>

using namespace std;

class Solution 
{
public:
    vector<int> countBits(int n) 
    {
        vector<int> ans(n + 1, 0);
        
        for (int i = 1; i <= n; ++i)
        {
            ans[i] = ans[i >> 1] + (i & 1);
        }
        
        return ans;
    }
};

int main() 
{
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    int n;
    if (cin >> n) 
    {
        Solution solver;
        vector<int> result = solver.countBits(n);
        
        cout << "[";
        for (size_t i = 0; i < result.size(); ++i)
        {
            cout << result[i];
            if (i + 1 < result.size()) 
            {
                cout << ", ";
            }
        }
        cout << "]\n";
    }

    return 0;
}
