#include <iostream>
#include <vector>
using namespace std;

void merge(vector<int>& left, vector<int>& right, vector<int>& original) {
    int i = 0, j = 0, k = 0;
    while (i < left.size() && j < right.size()) {
        if (left[i] <= right[j])
            original[k++] = left[i++];
        else
            original[k++] = right[j++];
    }
    while (i < left.size()) original[k++] = left[i++];
    while (j < right.size()) original[k++] = right[j++];
}

void sort(vector<int>& arr) {
    if (arr.size() < 2) return;
    int mid = arr.size() / 2;
    vector<int> left(arr.begin(), arr.begin() + mid);
    vector<int> right(arr.begin() + mid, arr.end());
    sort(left);
    sort(right);
    merge(left, right, arr);
}

int main() {
    int n;
    cout << "Enter number of elements: ";
    cin >> n;
    vector<int> arr(n);
    cout << "Enter elements:\n";
    for (int i = 0; i < n; i++)
        cin >> arr[i];

    cout << "\nArray Before sorting: ";
    for (int i = 0; i < n; i++) cout << arr[i] << " ";
    cout << endl;

    sort(arr);

    cout << "Array After sorting: ";
    for (int i = 0; i < n; i++) cout << arr[i] << " ";
    cout << endl;
    return 0;
}
