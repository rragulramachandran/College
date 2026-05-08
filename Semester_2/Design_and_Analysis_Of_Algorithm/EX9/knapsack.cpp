#include <iostream>
#include <queue>
#include <algorithm>
using namespace std;

class Item
{
public:
    int weight, value;
    float ratio;

    Item() {}

    Item(int w, int v)
    {
        weight = w;
        value = v;
        ratio = (float)v / w;
    }
};

bool cmp(Item a, Item b)
{
    return a.ratio > b.ratio;
}

class Node
{
public:
    int level;      
    int profit;     
    int weight;     
    float bound;    

    Node(int l, int p, int w)
    {
        level = l;
        profit = p;
        weight = w;
        bound = 0;
    }
};

float calculateBound(Node u, int n, int W, Item items[])
{
    if (u.weight >= W)
        return 0;

    float profit_bound = u.profit;
    int total_weight = u.weight;
    int j = u.level + 1;

    
    while (j < n && total_weight + items[j].weight <= W)
    {
        total_weight += items[j].weight;
        profit_bound += items[j].value;
        j++;
    }

    
    if (j < n)
    {
        profit_bound += (W - total_weight) * items[j].ratio;
    }

    return profit_bound;
}

int knapsack(int W, Item items[], int n)
{
    sort(items, items + n, cmp);

    queue<Node> q;

    Node u(0, 0, 0);
    Node v(0, 0, 0);

    int maxProfit = 0;

    u.bound = calculateBound(u, n, W, items);
    q.push(u);

    while (!q.empty())
    {
        u = q.front();
        q.pop();

        if (u.level >= n)
            continue;

        
        v.level = u.level + 1;
        v.weight = u.weight + items[u.level].weight;
        v.profit = u.profit + items[u.level].value;

        if (v.weight <= W && v.profit > maxProfit)
        {
            maxProfit = v.profit;
        }

        v.bound = calculateBound(v, n, W, items);

        if (v.bound > maxProfit)
        {
            q.push(v);
        }

        
        v.weight = u.weight;
        v.profit = u.profit;
        v.bound = calculateBound(v, n, W, items);

        if (v.bound > maxProfit)
        {
            q.push(v);
        }
    }

    return maxProfit;
}

int main()
{
    int n, W;

    cout << "Enter number of items: ";
    cin >> n;

    Item items[n];

    for (int i = 0; i < n; i++)
    {
        int w, v;
        cout << "Enter weight and value of item " << i+1 << ": ";
        cin >> w >> v;
        items[i] = Item(w, v);
    }

    cout << "Enter capacity: ";
    cin >> W;

    cout << "Maximum Profit = " << knapsack(W, items, n) << endl;

    return 0;
  }