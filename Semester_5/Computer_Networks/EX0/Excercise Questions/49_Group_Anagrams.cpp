#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <utility>

using namespace std;

class Solution 
{
public:
    vector<vector<string>> groupAnagrams(vector<string>& strs) 
    {
        unordered_map<string, vector<string>> anagramMap;

        for (auto word : strs) 
        {
            string countKey(26, 0);

            for (char c : word) 
            {
                countKey[c - 'a']++;
            }

            anagramMap[countKey].push_back(word);
        }

        vector<vector<string>> result;
        for (auto pair : anagramMap) 
        {
            result.push_back(move(pair.second));
        }

        return result;
    }
};

int main() 
{
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    int n;
    if (!(cin >> n)) return 0;

    vector<string> strs(n);
    for (int i = 0; i < n; ++i) 
    {
        cin >> strs[i];
    }

    Solution solver;
    vector<vector<string>> result = solver.groupAnagrams(strs);

    for (const auto& group : result) 
    {
        cout << "[";
        for (size_t i = 0; i < group.size(); ++i) {
            cout << group[i];
            if (i + 1 < group.size()) 
            {
                cout << ", ";
            }
        }
        cout << "]\n";
    }

    return 0;
}
