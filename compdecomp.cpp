#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <queue>
#include <fstream>

struct Node {
    char data;
    unsigned freq;
    std::string code;
    Node* left, *right;

    Node() {
        left = right = nullptr;
    }
};

class huffman {
private:
    std::vector<Node*> arr;
    std::fstream inFile;
    std::fstream outFile;
    std::string inFileName;
    std::string outFileName;
    Node* root;

    class Compare {
    public:
        bool operator() (Node* l, Node* r) {
            return l->freq > r->freq;
        }
    };

    std::priority_queue<Node*, std::vector<Node*>, Compare> minHeap;

    void createArr() {
        for (int i = 0; i < 128; i++) {
            arr.push_back(new Node());
            arr[i]->data = i;
            arr[i]->freq = 0;
        }
    }

    void traverse(Node* r, std::string str) {
        if (r->left == nullptr && r->right == nullptr) {
            r->code = str;
            return;
        }

        traverse(r->left, str + '0');
        traverse(r->right, str + '1');
    }

    int binToDec(std::string inStr) {
        int res = 0;
        for (auto c : inStr) {
            res = res * 2 + c - '0';
        }
        return res;
    }

    std::string decToBin(int inNum) {
        std::string temp = "", res = "";
        while (inNum > 0) {
            temp += (inNum % 2 + '0');
            inNum /= 2;
        }
        res.append(8 - temp.length(), '0');
        for (int i = temp.length() - 1; i >= 0; i--) {
            res += temp[i];
        }
        return res;
    }

    void buildTree(char a_code, std::string& path) {
        Node* curr = root;
        for (int i = 0; i < path.length(); i++) {
            if (path[i] == '0') {
                if (curr->left == nullptr) {
                    curr->left = new Node();
                }
                curr = curr->left;
            }
            else if (path[i] == '1') {
                if (curr->right == nullptr) {
                    curr->right = new Node();
                }
                curr = curr->right;
            }
        }
        curr->data = a_code;
    }

    void createMinHeap() {
        char id;
        inFile.open(inFileName, std::ios::in);
        inFile.get(id);
        while (!inFile.eof()) {
            arr[id]->freq++;
            inFile.get(id);
        }
        inFile.close();

        for (int i = 0; i < 128; i++) {
            if (arr[i]->freq > 0) {
                minHeap.push(arr[i]);
            }
        }
    }

    void createTree() {
        Node* left, *right;
        std::priority_queue<Node*, std::vector<Node*>, Compare> tempPQ(minHeap);
        while (tempPQ.size() != 1) {
            left = tempPQ.top();
            tempPQ.pop();

            right = tempPQ.top();
            tempPQ.pop();

            root = new Node();
            root->freq = left->freq + right->freq;

            root->left = left;
            root->right = right;
            tempPQ.push(root);
        }
    }

    void createCodes() {
        traverse(root, "");
    }

    void saveEncodedFile() {
        inFile.open(inFileName, std::ios::in);
        outFile.open(outFileName, std::ios::out | std::ios::binary);
        std::string in = "";
        std::string s = "";
        char id;

        in += (char)minHeap.size();
        std::priority_queue<Node*, std::vector<Node*>, Compare> tempPQ(minHeap);
        while (!tempPQ.empty()) {
            Node* curr = tempPQ.top();
            in += curr->data;
            s.assign(127 - curr->code.length(), '0');
            s += '1';
            s += curr->code;
            in += (char)binToDec(s.substr(0, 8));
            for (int i = 0; i < 15; i++) {
                s = s.substr(8);
                in += (char)binToDec(s.substr(0, 8));
            }
            tempPQ.pop();
        }
        s.clear();

        inFile.get(id);
        while (!inFile.eof()) {
            s += arr[id]->code;
            while (s.length() > 8) {
                in += (char)binToDec(s.substr(0, 8));
                s = s.substr(8);
            }
            inFile.get(id);
        }

        int count = 8 - s.length();
        if (s.length() < 8) {
            s.append(count, '0');
        }
        in += (char)binToDec(s);
        in += (char)count;

        outFile.write(in.c_str(), in.size());
        inFile.close();
        outFile.close();
    }

    void saveDecodedFile() {
        inFile.open(inFileName, std::ios::in | std::ios::binary);
        outFile.open(outFileName, std::ios::out);
        unsigned char size;
        inFile.read(reinterpret_cast<char*>(&size), 1);
        inFile.seekg(-1, std::ios::end);
        char count0;
        inFile.read(&count0, 1);
        inFile.seekg(1 + 17 * size, std::ios::beg);

        std::vector<unsigned char> text;
        unsigned char textseg;
        inFile.read(reinterpret_cast<char*>(&textseg), 1);
        while (!inFile.eof()) {
            text.push_back(textseg);
            inFile.read(reinterpret_cast<char*>(&textseg), 1);
        }

        Node* curr = root;
        std::string path;
        for (int i = 0; i < text.size() - 1; i++) {
            path = decToBin(text[i]);
            if (i == text.size() - 2) {
                path = path.substr(0, 8 - count0);
            }
            for (int j = 0; j < path.size(); j++) {
                if (path[j] == '0') {
                    curr = curr->left;
                }
                else {
                    curr = curr->right;
                }

                if (curr->left == nullptr && curr->right == nullptr) {
                    outFile.put(curr->data);
                    curr = root;
                }
            }
        }
        inFile.close();
        outFile.close();
    }

    void getTree() {
        inFile.open(inFileName, std::ios::in | std::ios::binary);
        unsigned char size;
        inFile.read(reinterpret_cast<char*>(&size), 1);
        root = new Node();
        for(int i = 0; i < size; i++) {
            char aCode;
            unsigned char hCodeC[16];
            inFile.read(&aCode, 1);
            inFile.read(reinterpret_cast<char*>(hCodeC), 16);
            std::string hCodeStr = "";
            for (int i = 0; i < 16; i++) {
                hCodeStr += decToBin(hCodeC[i]);
            }
            int j = 0;
            while (hCodeStr[j] == '0') {
                j++;
            }
            hCodeStr = hCodeStr.substr(j+1);
            buildTree(aCode, hCodeStr);
        }
        inFile.close();
    }

public:
    huffman(std::string inFileName, std::string outFileName) {
        this->inFileName = inFileName;
        this->outFileName = outFileName;
        createArr();
    }

    ~huffman() {
        for (Node* node : arr) {
            delete node;
        }
        delete root;
    }

    void compress() {
        createMinHeap();
        createTree();
        createCodes();
        saveEncodedFile();
    }

    void decompress() {
        getTree();
        saveDecodedFile();
    }
};

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " compress/decompress input_file output_file" << std::endl;
        return 1;
    }

    std::string operation = argv[1];
    std::string inputFile = argv[2];
    std::string outputFile = argv[3];

    huffman f(inputFile, outputFile);

    if (operation == "compress") {
        f.compress();
        std::cout << "Compressed successfully" << std::endl;
    } else if (operation == "decompress") {
        f.decompress();
        std::cout << "Decompressed successfully" << std::endl;
    } else {
        std::cerr << "Invalid operation. Use 'compress' or 'decompress'" << std::endl;
        return 1;
    }

    return 0;
}
