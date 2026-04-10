#include <iostream>
#include <string>
using namespace std;

int main()
{
    string text, pattern;
    int i = 0, j = 0;
    bool found = false;

    cout << "Enter text: ";
    getline(cin,text);

    cout << "Enter pattern: ";
    getline(cin,pattern);

    int n = text.length();
    int m = pattern.length();

    while (i < n)
    {
        if (text[i] == pattern[j])
        {
            i++;
            j++;
        }
        else
        {
            i = i - j + 1; // Backtrack i to the next starting character
            j = 0;
        }

        if (j == m)
        {
            found = true;
            break;
        }
    }

    if (found) {
        int start_pos = i - m; // Starting index (0-based)
        cout << "Pattern found at " << start_pos + 1 << "th position." << endl;

        // Print the matching alignment
        cout << "Text:    " << text << endl;
        cout << "Match:   ";
        for (int s = 0; s < start_pos; s++) cout << " "; // Print leading spaces
        cout << pattern << endl;
    }
    else {
        cout << "Pattern not found" << endl;
    }

    return 0;
}
