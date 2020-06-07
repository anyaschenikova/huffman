#define SIZE 256
#define CODE_SIZE 40
#define BIT8 8

typedef struct node {
    unsigned char symb;
    unsigned char isSymb;
    unsigned int freq;
    unsigned char code[CODE_SIZE];
    int level;
    struct node * left, *right, *next;
}NODE;

typedef union bit2char {
    char symb;
    struct bit {
        unsigned b1 : 1;
        unsigned b2 : 1;
        unsigned b3 : 1;
        unsigned b4 : 1;
        unsigned b5 : 1;
        unsigned b6 : 1;
        unsigned b7 : 1;
        unsigned b8 : 1;
    }mbit;
}BIT2CHAR;

long CalcFreqSymb(const char * from, int* freq);
void PrintFreq(int * freq);
NODE * MakeNodeFromFreq(unsigned char symb, unsigned int freq);
NODE * MakeNodeFromNode(const NODE * left, const NODE * right);
void Add2List(NODE ** head, NODE * pnew);
NODE * MakeListFromArray(int * freq);
void PrintList(NODE * head);
NODE * MakeTreeFromList(NODE * head);
void MakeCodeInTree(NODE* root, int level, unsigned char* prev_code, unsigned char added);
void PrintTree(NODE* root);
void MakeTableFromTree(NODE * root, char (*code)[CODE_SIZE]);
void PrintTableCodes(unsigned char(*code) [CODE_SIZE]);
char* MakeBitString(const char * from, int len, unsigned char(*code)[CODE_SIZE]);
char* FromBitToChar(const char * str, int * tail, int * len);
void WriteFile(const char * to, char * res, int len);
void PackFile(const char * from, const char * to, long *bitstringLength, long *fileLength);
void UnpackFile(const char * from, const char * to, long bitstringLength, long fileLength);
char* stringToBinary(char* s, int len, long bitstringLength);
void inplace_reverse(char * str, long lenght);