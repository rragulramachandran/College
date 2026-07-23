#include <iostream>
using namespace std;

void heapSort(int H[], int n)
{
    // Build Max Heap
    for(int i = n/2; i >= 1; i--)
    {
        int k = i;
        int v = H[k];
        bool heap = false;

        while(!heap && 2*k <= n)
        {
            int j = 2*k;

            if(j < n && H[j] < H[j+1])
            {
                j = j + 1;
            }

            if(v >= H[j])
            {
                heap = true;
            }
            else
            {
                H[k] = H[j];
                k = j;
            }
        }
        H[k] = v;
    }

    // Heap Sort Phase
    for(int i = n; i >= 2; i--)
    {
        // Swap root with last element
        int temp = H[1];
        H[1] = H[i];
        H[i] = temp;

        // Restore heap property for reduced heap
        int k = 1;
        int v = H[k];
        bool heap = false;

        while(!heap && 2*k <= i-1)
        {
            int j = 2*k;

            if(j < i-1 && H[j] < H[j+1])
            {
                j = j + 1;
            }

            if(v >= H[j])
            {
                heap = true;
            }
            else
            {
                H[k] = H[j];
                k = j;
            }
        }
        H[k] = v;
    }
}

int main()
{
    int n;
    cout << "No.of Elements: ";
    cin >> n;

    int H[n+1];

    for(int i = 1; i <= n; i++)   // FIXED
    {
        cin >> H[i];
    }

    heapSort(H, n);

    for(int i = 1; i <= n; i++)
    {
        cout << H[i] << " ";
    }

    return 0;
}
