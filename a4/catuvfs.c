#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include "disk.h"

#define ATTRIBUTE_SIZE 4
#define BLOCKSIZE_BYTE_SIZE 2

int main(int argc, char *argv[]) {
    superblock_entry_t sb;
    int  i, ii;
    char *imagename = NULL;
    char *filename  = NULL;
    FILE *f;

    unsigned short block_size;    
    unsigned int root_start;
    unsigned int root_block_count;

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--image") == 0 && i+1 < argc) {
            imagename = argv[i+1];
            i++;
        } else if (strcmp(argv[i], "--file") == 0 && i+1 < argc) {
            filename = argv[i+1];
            i++;
        }
    }

    if (imagename == NULL || filename == NULL) {
        fprintf(stderr, "usage: catuvfs --image <imagename> " \
            "--file <filename in image>");
        exit(1);
    }

    f = fopen(imagename, "rb");
    // Parse to byte 8 where root directory start value is
    fseek(f, 8, SEEK_SET);
    fread(&block_size, BLOCKSIZE_BYTE_SIZE, 1, f);
    block_size = htons(block_size);
    // Parse to byte 22 where root directory start value is
    fseek(f, 22, SEEK_SET);
    fread(&root_start, ATTRIBUTE_SIZE, 1, f);
    root_start = htonl(root_start);
    fread(&root_block_count, ATTRIBUTE_SIZE, 1, f);
    root_block_count = htonl(root_block_count);
    
    unsigned int count;
    for( count=0;count<root_block_count;count++){
        unsigned int address = (root_start*block_size)+(SIZE_DIR_ENTRY*count);
        fseek(f,address, SEEK_SET);
        unsigned char status;
        fread(&status, 1, 1, f);
        // status = htonl(status);
        if(status){
            unsigned int starting_block;
            fread(&starting_block, 4, 1, f);
            starting_block = htonl(starting_block);
            unsigned int num_blocks;
            fread(&num_blocks, 4, 1, f);
            num_blocks = htonl(num_blocks);
            unsigned int filesize;
            fread(&filesize, 4, 1, f);
            filesize = htonl(filesize);

            fseek(f,14,SEEK_CUR);
            char file_name[31];
            fread(file_name, 31, 1, f);
            file_name[31] = '\0';
            if (strcmp(file_name, filename) == 0){
                printf("File %s found %u, %u, %u\n",
                file_name, starting_block, num_blocks, filesize);
                exit(0);
            }
        }
    }
    printf("File not found\n");
    return 0; 
}
