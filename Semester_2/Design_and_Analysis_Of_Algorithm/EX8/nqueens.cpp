#include <iostream>
#include <cmath>
using namespace std;

int x[100];
int n;

bool place(int k, int i)
{
    for (int j = 1; j < k; j++)
    {
        if (x[j] == i)
        {
            cout << "Conflict at Row " << k << ", Col " << i 
                 << " (Same Column as Row " << j << ")\n";
            return false;
        }
        if (abs(x[j] - i) == abs(j - k))
        {
            cout << "Conflict at Row " << k << ", Col " << i 
                 << " (Diagonal clash with Row " << j << ")\n";
            return false;
        }
    }
    return true;
}

void printArray()
{
    cout << "[ ";
    for (int i = 1; i <= n; i++)
        cout << x[i] << " ";
    cout << "]\n";
}

void printBoard()
{
    for (int i = 1; i <= n; i++)
    {
        for (int j = 1; j <= n; j++)
        {
            if (x[i] == j)
                cout << " Q ";
            else
                cout << " . ";
        }
        cout << endl;
    }
}

void nQueens(int k)
{
    for (int i = 1; i <= n; i++)
    {
        cout << "\nTrying Row " << k << " Col " << i << "...\n";

        if (place(k, i))
        {
            x[k] = i;
            cout << "Placed Queen at Row " << k << " Col " << i << endl;

            if (k == n)
            {
                cout << "\n✅ Solution Found:\n";
                printArray();
                printBoard();
                cout << "------------------\n";
            }
            else
            {
                nQueens(k + 1);
            }

            
            cout << "Backtracking from Row " << k << " Col " << i << endl;
        }
    }
}

int main()
{
    cout << "Enter number of queens: ";
    cin >> n;

    nQueens(1);

    return 0;
}