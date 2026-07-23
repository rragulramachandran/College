#include <iostream>
using namespace std;

int main()
{
    int n, a[50], temp, minIndex;

    cout << "Enter number of elements: ";
    cin >> n;

    cout << "Enter elements:\n";
    for(int i = 0; i < n; i++)
        cin >> a[i];

    // Selection Sort
    for(int i = 0; i < n-1; i++)
    {
        minIndex = i;

        for(int j = i+1; j < n; j++)
        {
            if(a[j] < a[minIndex])
                minIndex = j;
        }

        // Swap minimum element with first element of unsorted part
        if(minIndex != i)
        {
            temp = a[i];
            a[i] = a[minIndex];
            a[minIndex] = temp;
        }
    }

    cout << "Sorted array:\n";
    for(int i = 0; i < n; i++)
        cout << a[i] << " ";

    return 0;
}
