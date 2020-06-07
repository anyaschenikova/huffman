#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include "huffman.h"
#include "locale.h"
#include <string.h>

// argv[1] - исходный, argv[2]-название файла нового, который быдет выполнять роль архива, argv[3]-куда расспаковывать
// в самом начале нужно указать, например: test.txt - исходник, new.zip (создастся потом) и out.txt (создастся потом)

int main(int argc, char const *argv[])
{
    if(argc < 4)
        return -1;
    unsigned char codes[SIZE][CODE_SIZE] = { 0 };
    long bitstringLength, fileLength;
    PackFile(argv[1], argv[2], &bitstringLength, &fileLength); 
    printf("File packed to: %s\n", argv[2]);
    UnpackFile(argv[2], argv[3], bitstringLength, fileLength);
    printf("File unpacked to: %s\n", argv[3]);
    return 0;
}
