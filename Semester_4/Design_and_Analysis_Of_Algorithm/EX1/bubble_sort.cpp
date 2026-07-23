#include <iostream>
using namespace std;

int main()
{
    int n, temp;
    int *a;
    cout << "Enter number of elements: ";
    cin >> n;
    a= new int[n];
    cout << "Enter elements:\n";
    for(int i = 0; i < n; i++)
        cin >> a[i];
    for(int i = 0; i < n-1; i++)
    {
        int flag = 1;
        for(int j = 0; j < n-i-1; j++)
        {
            if(a[j] > a[j+1])
            {
                temp = a[j];
                a[j] = a[j+1];
                a[j+1] = temp;
                flag = 0;
            }
        }
        if(flag != 0)
            break;
    }
    cout << "Sorted array:\n";
    for(int i = 0; i < n; i++)
        cout << a[i] <<  " ";
        cout << endl;
    return 0;
}
