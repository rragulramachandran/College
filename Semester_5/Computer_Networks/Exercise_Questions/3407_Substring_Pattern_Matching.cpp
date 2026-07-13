#include <iostream>
#include <string>
#include <string_view>

using namespace std;

class Solution 
{
public:
    bool hasMatch(string s, string p) 
    {
        size_t starPos = p.find('*');
        if (starPos == string::npos) 
        {
            return s.find(p) != string::npos;
        }

        string_view prefix = string_view(p).substr(0, starPos);
        string_view suffix = string_view(p).substr(starPos + 1);

        size_t prefixPos = s.find(prefix);
        if (prefixPos == string::npos) 
        {
            return false;
        }

        size_t suffixStartSearchIdx = prefixPos + prefix.length();
        size_t suffixPos = s.find(suffix, suffixStartSearchIdx);

        return suffixPos != string::npos;
    }
};

int main() 
{
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    string s, p;
    if (cin >> s >> p) 
    {
        Solution solver;
        if (solver.hasMatch(s, p)) 
        {
            cout << "true\n";
        } 
        else 
        {
            cout << "false\n";
        }
    }

    return 0;
}
