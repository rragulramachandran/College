/*

Algorithm: Finding Kth Largest Element (Lomuto Partition)

    1. Define the Partition Function:
       * Pick the last element as the pivot.
       * Initialize a pointer i at the start.
       * Loop through the array; if an element is greater than the pivot (for largest)
             or smaller (for smallest), swap it with the element at i and increment i.
       * Finally, swap the pivot into its correct position at index i.
       * Return the index i.
    2. Define the QuickSelect Function:
       * If the partition index is exactly k-1, you've found the element.
       * If the partition index is greater than k-1, repeat the process on the left side.
       * If the partition index is less than k-1, repeat on the right side.
    3. Main Execution:
       * Pass the array and the target rank k into the QuickSelect function.
       * Print the result once the recursion hits the base case.

*/

#include <iostream>
#include <vector>
using namespace std;

// Function to partition the array according
// to pivot index element
void partition(vector<int> &arr)
{
        int n = arr.size();
        int pivot = arr[n - 1];

        // i acts as boundary between smaller and
        // larger element compared to pivot
        int i = -1;
        for (int j = 0; j < n; j++)
        {
        // If smaller element is found expand the
        // boundary and swapping it with boundary element.
        if (arr[j] < pivot)
        {
                i++;
                swap(arr[i], arr[j]);
        }
    }

        // place the pivot at its correct position
        swap(arr[i + 1], arr[n - 1]);
}

int main()
{
        int element, n, k;
        cout << "Enter Size of the Array : ";
        cin >> n;

        vector<int> arr;
        for(int i = 0; i < n; i++)
        {
                cin >> element;
                arr.push_back(element);
        }

        cout << "Enter Index of K : ";
        cin >> k;

        partition(arr); // Call Lomuto Partition

        if(k == 1)
        {
           cout << k << "st Smallest Element of an Array : " << arr[n - k] << endl;
        }
        else if(k == 2)
        {
           cout << k << "nd Smallest Element of an Array : " << arr[n - k] << endl;
        }
        else if(k == 3)
        {
           cout << k << "rd Smallest Element of an Array : " << arr[n - k] << endl;
        }
        else
        {
           cout << k << "th Smallest Element of an Array : " << arr[n - k] << endl;
        }
        return 0;
}
