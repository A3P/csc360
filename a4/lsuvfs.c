#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include "disk.h"

#define ATTRIBUTE_SIZE 4
#define BLOCKSIZE_BYTE_SIZE 2

char *month_to_string(short m) {
    switch(m) {
    case 1: return "Jan";
    case 2: return "Feb";
    case 3: return "Mar";
    case 4: return "Apr";
    case 5: return "May";
    case 6: return "Jun";
    case 7: return "Jul";
    case 8: return "Aug";
    case 9: return "Sep";
    case 10: return "Oct";
    case 11: return "Nov";
    case 12: return "Dec";
    default: return "?!?";
    }
}


void unpack_datetime(unsigned char *time, short *year, short *month, 
    short *day, short *hour, short *minute, short *second)
{
    assert(time != NULL);

    memcpy(year, time, 2);
    *year = htons(*year);

    *month = (unsigned short)(time[2]);
    *day = (unsigned short)(time[3]);
    *hour = (unsigned short)(time[4]);
    *minute = (unsigned short)(time[5]);
    *second = (unsigned short)(time[6]);
}


int main(int argc, char *argv[]) {
    int  i;
    char *imagename = NULL;
    FILE *f;

    unsigned short block_size;    
    unsigned int root_start;
    unsigned int root_block_count;

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--image") == 0 && i+1 < argc) {
            imagename = argv[i+1];
            i++;
        }
    }

    if (imagename == NULL)
    {
        fprintf(stderr, "usage: lsuvfs --image <imagename>\n");
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
            unsigned int filesize;
            fseek(f, 8,SEEK_CUR);
            fread(&filesize, 4, 1, f);
            filesize = htonl(filesize);

            fseek(f,7,SEEK_CUR);
            unsigned short year;
            fread(&year, 2, 1, f);
            year = htons(year);
            unsigned char month;
            unsigned char day;
            unsigned char hour;
            unsigned char min;
            unsigned char sec;
            fread(&month, 1, 1, f);
            fread(&day, 1, 1, f);
            fread(&hour, 1, 1, f);
            fread(&min, 1, 1, f);
            fread(&sec, 1, 1, f);


            char filename[31];
            fread(filename, 31, 1, f);
            filename[31] = '\0';
            printf("%8u %u-%s-%u %02u:%u:%u %s\n",
            filesize, year, month_to_string(month),day,
            hour, min, sec, filename);
            
        }
    }

    return 0; 
}
