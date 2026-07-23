#include <iostream>
#include <queue>
#include <unordered_map>
using namespace std;

unordered_map<char, string> codeMap;

struct Node
{
    char ch;
    int freq;
    Node *left, *right;

    Node(char c, int f)
    {
        ch = c;
        freq = f;
        left = right = NULL;
    }
};

struct compare
{
    bool operator()(Node* l, Node* r)
    {
        return l->freq > r->freq;
    }
};

void printCodes(Node* root, string code)
{
    if (!root) return;

    if (root->ch != '$')
    {
        codeMap[root->ch] = code;
        cout << root->ch << ": " << code << endl;
    }

    printCodes(root->left, code + "0");
    printCodes(root->right, code + "1");
}

void stringToCode(string text)
{
    string result = "";
    for(int i = 0; i < text.length(); i++)
    {
        result += codeMap[text[i]];
    }

    cout << "Encoding of String : " << result << endl;
}

void HuffmanCodes(char data[], int freq[], int n)
{
    priority_queue<Node*, vector<Node*>, compare> pq;

    for (int i = 0; i < n; i++)
        pq.push(new Node(data[i], freq[i]));

    while (pq.size() > 1)
    {
        Node *left = pq.top(); pq.pop();
        Node *right = pq.top(); pq.pop();

        Node *merged = new Node('$', left->freq + right->freq);
        merged->left = left;
        merged->right = right;

        pq.push(merged);
    }

    cout << "\nHuffman Codes:\n";
    printCodes(pq.top(), "");
}

int main()
{
    int n;
    cout << "Enter number of characters: ";
    cin >> n;

    char data[n];
    int freq[n];

    cout << "Enter characters:\n";
    for (int i = 0; i < n; i++)
        cin >> data[i];

    cout << "Enter frequencies:\n";
    for (int i = 0; i < n; i++)
        cin >> freq[i];

    HuffmanCodes(data, freq, n);

    string text;
    cout << "Enter string to encode : ";
    cin >> text;

    stringToCode(text);

    return 0;
}