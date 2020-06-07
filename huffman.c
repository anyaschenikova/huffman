#include "huffman.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define SIZE 100000
#define CODE_SIZE 40
#define BIT8 8

long CalcFreqSymb(const char * from, int* freq)
{
    FILE* fr = fopen(from, "rb");
    if (!fr)
        return -1;
    fseek(fr, 0, SEEK_END);
    long length = ftell(fr);
    fseek(fr, 0, SEEK_SET);
    for (int i = 0; i < length; ++i)
    {
        freq[(unsigned char)fgetc(fr)] ++;
    }
    fclose(fr);
    return length;
}

void PrintFreq(int * freq)
{
    for (int i = 0; i < SIZE;++i)
    {
        printf("%c - %d\n", (unsigned char)i, freq[i]);
    }
}

NODE * MakeNodeFromFreq(unsigned char symb, unsigned int freq)
{
    NODE * res = (NODE*)malloc(sizeof(NODE));
    res->freq = freq;
    res->isSymb = 1;
    res->symb = symb;
    res->left = res->right = res->next = 0;
    return res;
}

NODE * MakeNodeFromNode(const NODE * left, const NODE * right)
{
    NODE * res = (NODE*)malloc(sizeof(NODE));
    res->freq = left->freq + right->freq;
    res->isSymb = 0;
    res->symb =
    res->left = left;
    res->right = right;
    res->next = 0;
    return res;
}

void Add2List(NODE ** head, NODE * pnew)
{
    while (*head)
    {
        if ((*head)->freq > pnew->freq)
            break;
        head = &((*head)->next) ;
    }
    pnew->next = *head;
    *head = pnew;  
}

NODE * MakeListFromArray(int * freq)
{
    NODE * head = 0;
    for (int i = 0; i < SIZE; ++i)
    {
        if(freq[i])
            Add2List(&head, MakeNodeFromFreq((unsigned char)i, (unsigned int)freq[i]));
    }
    return head;
}

void PrintList(NODE * head)
{
    while (head)
    {
        printf ("%c %d\n", head->symb, head->freq);
        head = head->next;
    }
}

NODE * MakeTreeFromList(NODE * head)
{
    while (head && head->next)
    {
        NODE * left = head;
        NODE * right = head->next;
        Add2List(&(head->next->next), MakeNodeFromNode(left, right));
        head = head->next->next;
    }
    return head;
}

void MakeCodeInTree(NODE* root, int level, unsigned char* prev_code, unsigned char added)
{
    if (root)
    {
        strcpy(root->code, prev_code);
        root->code[level] = added;
        root->code[level + 1] = 0;
        root->level = level;
        MakeCodeInTree(root->left, level + 1, root->code, '0');
        MakeCodeInTree(root->right, level + 1, root->code, '1');

    }
}

void PrintTree(NODE* root)
{
    if (root)
    {
        PrintTree(root->left);
        printf("%c - %d (%d) [%s] %d\n", root->symb, root->freq, root->isSymb, root->code, root->level);
        PrintTree(root->right);
    }
}

void MakeTableFromTree(NODE * root, char (*code)[CODE_SIZE])
{
    if (root)
    {
        MakeTableFromTree(root->left, code);
        if (root->isSymb)
        {
            strcpy(code[root->symb], root->code);
        }
        MakeTableFromTree(root->right, code);
    }
}

void PrintTableCodes(unsigned char(*code) [CODE_SIZE])
{
    FILE* fw = fopen("codes.txt", "w");
    if (!fw)
        return;
    for (int i = 0; i < SIZE; ++i)
    {
        if(strlen(code[i])) {
            fprintf(fw, "%c - %s\n", (unsigned char)i, code[i]);
        }
       
    }
    fclose(fw);
}

char* MakeBitString(const char * from, int len, unsigned char(*code)[CODE_SIZE]) {
    char * bitstr = (char*)calloc(len * BIT8, sizeof(char));
    FILE* fr = fopen(from, "rb");
    if (!fr)
        return -1;
    for (int i = 0; i < len; ++i)
    {
        unsigned char c = fgetc(fr);
        strcat(bitstr, code[c]);
    }
    fclose(fr);
    return bitstr;
}

char* FromBitToChar(const char * str, int * tail, int * len)
{
    int count = strlen(str) / BIT8;
    *tail = strlen(str) % BIT8;
    *len = count + 1;
    BIT2CHAR symb;
    char * res = (char*)malloc((*len) * sizeof(char));
    for (int i = 0; i < *len; ++i)
    {
        symb.mbit.b1 = str[i*BIT8 + 0];
        symb.mbit.b2 = str[i*BIT8 + 1];
        symb.mbit.b3 = str[i*BIT8 + 2];
        symb.mbit.b4 = str[i*BIT8 + 3];
        symb.mbit.b5 = str[i*BIT8 + 4];
        symb.mbit.b6 = str[i*BIT8 + 5];
        symb.mbit.b7 = str[i*BIT8 + 6];
        symb.mbit.b8 = str[i*BIT8 + 7];
        res[i] = symb.symb; 
    }
    return res;
}

void WriteFile(const char * to, char * res, int len)
{
    FILE* fw = fopen(to, "wb");
    if (!fw)
        return;
    for (int i = 0; i < len; ++i) {
        fputc(res[i], fw);
    }
    fclose(fw);
}


void deleteList(NODE *phead)
{
    if(phead)
    {
        deleteList(phead->next);
        if(phead)
        free(phead);
    }
}


void PackFile(const char * from, const char * to, long *bitstringLength, long *fileLength)
{
    int freq[SIZE] = { 0 };
    long len = CalcFreqSymb(from, freq);
    *fileLength = len;
    printf("%s filesize: %ld\n", from, len);
    // PrintFreq(freq) ;
    NODE * head = 0;
    head = MakeListFromArray(freq) ;
    // PrintList (head) ;
    NODE * root = 0;
    root = MakeTreeFromList(head) ;
    MakeCodeInTree(root->left, 0, "", '0');
    MakeCodeInTree(root->right, 0, "", '1');
    // PrintTree(root) ;
    unsigned char codes[SIZE][CODE_SIZE] = { 0 };
    MakeTableFromTree(root, codes);
    PrintTableCodes (codes) ;
    char * bitstring = MakeBitString(from, len, codes);
    *bitstringLength = strlen(bitstring);
    int reslen = 0;
    int tail = 0;
    char * res = FromBitToChar(bitstring, &tail, &reslen);
    WriteFile(to, res, reslen);
    printf("Compressed filesize: %d\n",  reslen);
    free(bitstring);
    free(res);
    deleteList(head);
    deleteList(root);
}

void UnpackFile(const char * from, const char * to, long bitstringLength, long fileLength)
{
    FILE* fr = fopen(from, "rb");
    if (!fr)
        return;
    fseek(fr, 0, SEEK_END);
    long length = ftell(fr);
    fseek(fr, 0, SEEK_SET);
    char *str = (char *)malloc((length + 1) * sizeof(char));

    int nread;
    while((nread = fread(str, 1, length, fr)) >0){
        printf("READING ZIP...\n");
    }
    

    fclose(fr);
    inplace_reverse(str ,length);
    if(str == NULL) {
        puts("Empty");
        return;
    } 
    char *binary = (char *)malloc(length*8 + 1);
    binary[0] = '\0';
    long binLen = 0;
    for(size_t i = 0; i < length; ++i) {
        char ch = str[i];
        for(int j = 7; j >= 0; --j){
            if(ch & (1 << j)) {
                strcat(binary,"1");
            } else {
                strcat(binary,"0");
            }
            binLen++;
        }
    }
    inplace_reverse(binary, binLen);
    int zeroCount = 0;
    for (int i = 0; i < 8; i++)
    {
        if (binary[i] == '0')
        {
            zeroCount++;
        }
        
    }

    if(zeroCount == 8) {
        char *tempBuff = (char *)malloc((strlen(binary) +1)*sizeof(char));
        strncpy(tempBuff, binary + 8, strlen(binary));
        strcat(tempBuff, "00000000");
        strcpy(binary, tempBuff);
        free(tempBuff);
    }
    char *buff = (char*)malloc((bitstringLength)*sizeof(char));
    strncpy(buff, binary, bitstringLength);
    buff[bitstringLength] = '\0';
    free(binary);
    free(str);
    

    char *codes[SIZE] = { 0 };
    for (int i = 0; i < SIZE; i++)
    {
        codes[i] = "";
    }
    
   
    FILE* fcodes = fopen("codes.txt", "rb");
    if (!fcodes)
        return;
    fseek(fcodes, 0, SEEK_END);
    long lencodes = ftell(fcodes);
    fseek(fcodes, 0, SEEK_SET);


    char * line = NULL;
    size_t len = 0;
    size_t read;
    int cnt = 0;
    int newLineFlag = 0;
    while ((read = getline(&line, &len, fcodes)) != -1) {
        if(!strcmp(line, "\n")) {
            newLineFlag = 1;
            continue;
        }
        if(newLineFlag) {
            char numint[CODE_SIZE] = {0};
            int i = 0;
            for (i = 3; i < strlen(line); i++)
            {
                numint[i - 3] = line[i];
            }
            numint[i - 4] = '\0';
            codes['\n'] = (char *)malloc(strlen(numint) * sizeof(char));
            strcpy(codes['\n'], numint);
            newLineFlag = 0;
            continue;
        }
        char numint[CODE_SIZE] = {0};
        int i = 0;
        for (i = 4; i < strlen(line); i++)
        {
            numint[i - 4] = line[i];
        }
        numint[i - 5] = '\0';
        codes[(unsigned char)line[0]] = (char *)malloc(strlen(numint) * sizeof(char));
        strcpy(codes[(unsigned char)line[0]], numint);
    }
    
    fclose(fcodes);
    if (line)
        free(line);

    char *resstr = (char *)malloc((fileLength + 3) * sizeof(char));
    int c = 0;
    int symc = 0;
    int offset = 0;
    int found = 0;
    char *digbuff  = (char *)malloc((bitstringLength + 1) * sizeof(char));
    for (int i = 0; i < bitstringLength; i++)
    {
        digbuff[i] = '\0';
    }
    
    puts("UNZIPPING FILE");
    while (offset != strlen(buff))
    {
        found = 0;
        while(found != 1) {
            c++;
            for (int i = 0; i < c; i++)
            {
                digbuff[i] = buff[i+offset];
            }
            
            digbuff[c] = '\0';
            for (int i = 0; i < SIZE; i++)
            {
                if(!strcmp(codes[i], ""))
                    continue;
                if(!strcmp(codes[i], digbuff)) {
                    resstr[symc] = i;
                    symc++;
                    found = 1;
                    break;
                }
            }
        }
        offset += c;
        c = 0;
    }    

    char *wstr = (char *)malloc((symc + 1) * sizeof(char));
    strncpy(wstr, resstr, symc);
    wstr[symc] = '\0';
    FILE* fw = fopen(to, "wb");
    if (!fw)
        return;
    fprintf(fw,"%s", wstr);
    fclose(fw);
    free(resstr);
    free(wstr);
}



void inplace_reverse(char * str, long lenght)
{
  if (str)
  {
    char * end = str + lenght - 1;
#   define XOR_SWAP(a,b) do\
    {\
      a ^= b;\
      b ^= a;\
      a ^= b;\
    } while (0)

    while (str < end)
    {
      XOR_SWAP(*str, *end);
      str++;
      end--;
    }
#   undef XOR_SWAP
  }
}