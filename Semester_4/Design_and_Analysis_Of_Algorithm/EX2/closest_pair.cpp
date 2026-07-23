#include <iostream>
#include <cmath>
#include <climits>
#include <iomanip> // Added for better decimal formatting

using namespace std;

int main()
{
    int n;
    cout << "Enter number of points: ";
    cin >> n;

    int x[n], y[n];

    cout << "Enter the points (x y):\n";
    for (int i = 0; i < n; i++)
        cin >> x[i] >> y[i];

    double min_dist = INT_MAX;
    int temp1=0, temp2=0, temp3=0, temp4=0;

    cout << "\nDistances between all points:\n";
    for (int i = 0; i < n - 1; i++)
    {
        for (int j = i + 1; j < n; j++)
        {
            double d = sqrt((x[i] - x[j]) * (x[i] - x[j]) +
                            (y[i] - y[j]) * (y[i] - y[j]));

            // Print the distance for the current pair
            cout << "(" << x[i] << "," << y[i] << ") to (" << x[j] << "," <<      y[j] << ") = " << d << endl;

            if (d < min_dist)
            {
                min_dist = d;
                temp1 = x[i];
                temp2 = y[i];
                temp3 = x[j];
                temp4 = y[j];
            }
        }
    }

    cout << "\nMinimum distance points: (" << temp1 << "," << temp2 << ") and      (" << temp3 << "," << temp4 << ")" << endl;
    cout << "Minimum distance = " << min_dist << endl;

    return 0;
}
