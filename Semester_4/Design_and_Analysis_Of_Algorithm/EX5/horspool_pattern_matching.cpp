#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

#define MAX_CHAR 256

// Shift Table Computation
void shiftTable(string p, int table[])
{
    int m = p.length();

    for (int i = 0; i < MAX_CHAR; i++)
    {
        table[i] = m;
    }

    for (int j = 0; j < m - 1; j++)
    {
        table[(unsigned char)p[j]] = m - 1 - j;
    }
}

// Horspool pattern matching
int horsPool(string p, string t)
{
    int m = p.length();
    int n = t.length();
    int table[MAX_CHAR];

    shiftTable(p, table);

    int i = m - 1;
    while (i <= n - 1)
    {
        int k = 0;

        while (k <= m - 1 && p[m - 1 - k] == t[i - k])
        {
            k++;
        }

        if (k == m)
        {
            return i - m + 1;
        }
        else
        {
            i = i + table[(unsigned char)t[i]];
        }
    }
    return -1;
}

int main()
{
    string text, pattern;

    cout << "Enter the text: ";
    getline(cin, text);

    cout << "Enter the pattern: ";
    getline(cin, pattern);

    int pos = horsPool(pattern, text);

    if (pos != -1)
        cout << "Pattern found at index: " << pos << endl;
    else
        cout << "Pattern not found." << endl;

    return 0;
}
