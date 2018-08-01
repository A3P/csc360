#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include "disk.h"

#define IDENTIFIER_SIZE 8
#define BLOCKSIZE_BYTE_SIZE 2
#define ATTRIBUTE_SIZE 4
#define COLUMN_WIDTH 6

int main(int argc, char *argv[]) {
    superblock_entry_t sb;
    int  i;
    char *imagename = NULL;
    FILE  *f;
    int   *fat_data;

    char identifier[8];
    int block_count;
    unsigned int allocated = 0;
    unsigned int reserved = 0;

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--image") == 0 && i+1 < argc) {
            imagename = argv[i+1];
            i++;
        }
    }

    if (imagename == NULL)
    {
        fprintf(stderr, "usage: statuvfs --image <imagename>\n");
        exit(1);
    }

    // Record superblock
    f = fopen(imagename, "rb");
    fread(identifier, IDENTIFIER_SIZE, 1, f);
    fread(&sb.block_size, BLOCKSIZE_BYTE_SIZE, 1, f);
    fread(&sb.num_blocks, ATTRIBUTE_SIZE, 1, f);
    fread(&sb.fat_start, ATTRIBUTE_SIZE, 1, f);
    fread(&sb.fat_blocks, ATTRIBUTE_SIZE, 1, f);
    fread(&sb.dir_start, ATTRIBUTE_SIZE, 1, f);
    fread(&sb.dir_blocks, ATTRIBUTE_SIZE, 1, f);
    sb.block_size = htons(sb.block_size);
    sb.num_blocks = htonl(sb.num_blocks);
    sb.fat_start = htonl(sb.fat_start);
    sb.fat_blocks = htonl(sb.fat_blocks);
    sb.dir_start = htonl(sb.dir_start);
    sb.dir_blocks = htonl(sb.dir_blocks);

    // Iterate FAT
    block_count = sb.fat_start;
    fseek(f, block_count* sb.block_size, SEEK_SET);

    for(;block_count < sb.dir_start;block_count++){
        // Converting SIZE_FAT_ENTRY to bytes
        int j;
        for( j =1;(j*8*SIZE_FAT_ENTRY) <= sb.block_size;j++){
            unsigned int fat_val;
            fread(&fat_val, ATTRIBUTE_SIZE, 1, f);
            fat_val = htonl(fat_val);
            if((int)fat_val == 1){
                reserved++;
            } else if((int)fat_val !=0){
                allocated++;
            }
        }
    }

    printf("%s (%s)\n\n",identifier, imagename);
    printf("-----------------------------------------------------------\n");
    printf("  %8s %8s %8s %8s %8s %8s\n",
     "Bsz", "Bcnt", "FATst", "FATcnt", "DIRst", "DIRcnt");
    printf("  %8u %8u %8u %8u %8u %8u\n\n",
    sb.block_size, 
    sb.num_blocks, 
    sb.fat_start, 
    sb.fat_blocks, 
    sb.dir_start, 
    sb.dir_blocks);
    printf("-----------------------------------------------------------\n");
    printf("  %8s %8s %8s\n","Free", "Resv", "Alloc");
    printf("  %8u %8u %8u\n",
    (sb.num_blocks-reserved-allocated),reserved,allocated);


    return 0; 
}
