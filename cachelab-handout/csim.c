#include "cachelab.h"
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <string.h>

typedef unsigned long ul;
static int verbose, s, E, b;
static ul hits, misses, evictions, cnt;
static char fileName[50];
struct myRAM
{
    ul tag, stamp;
} *myCache;

static void printUsage() 
{
    printf("Usage: ./csim [-hv] -s <num> -E <num> -b <num> -t <file>\n"
            "Options:\n"
            "  -h         Print this help message.\n"
            "  -v         Optional verbose flag.\n"
            "  -s <num>   Number of set index bits.\n"
            "  -E <num>   Number of lines per set.\n"
            "  -b <num>   Number of block offset bits.\n"
            "  -t <file>  Trace file.\n"
            "\n"
            "Examples:\n"
            "  linux>  ./csim -s 4 -E 1 -b 4 -t traces/yi.trace\n"
            "  linux>  ./csim -v -s 8 -E 2 -b 4 -t traces/yi.trace\n");
}

static void update(const ul address)
{
    ul tag = address >> (s + b), 
       set = (address >> b) & ((1ul << s) - 1ul);
    
    ++cnt;
    struct myRAM* Set = &myCache[set * E];
    int i;
    
    // the first loop : check if hit
    for (i = 0; i < E; ++i) {
        if (Set[i].stamp && Set[i].tag == tag) {
            ++hits;
            Set[i].stamp = cnt;
            if (verbose) printf(" hit");
            return ;
        }
    }
    
    ++misses;
    if (verbose) printf(" miss");

    // the second loop : find a place with smallest stamp
    int flag = cnt, ind = 0;
    for (i = 0; i < E; ++i) {
        if (Set[i].stamp < flag) {
            ind = i;
            flag = Set[i].stamp;
        }
    }

    if (flag != 0) {
        ++evictions;
        if (verbose) printf(" eviction");
    }

    Set[ind].tag = tag;
    Set[ind].stamp = cnt;
}

int main(int argc, char *argv[])
{
    int c;
    while ((c = getopt (argc, argv, "hvs:E:b:t:")) != -1) 
    {
        switch (c) 
        {
            case 'h' :
                printUsage("");
                return 0;
            case 'v' :
                verbose = 1;
                break;
            case 's' :
                s = atoi(optarg);
                break;
            case 'E' :
                E = atoi(optarg);
                break;
            case 'b' :
                b = atoi(optarg);
                break;
            case 't' :
                strcpy(fileName, optarg);
                break;
            default :
                printUsage();
                return -1;
        }
    }

    if (s <= 0 || E <= 0 || b <= 0 || fileName == NULL)
    {
        printf("The argument is invalid.\n");
        printUsage();
        return -1;
    }

    FILE* fp = fopen(fileName, "r");
    if (fp == NULL) 
    {
        printf("The file does not exist!");
        printUsage();
        return -1;
    }

    unsigned num = (1 << s) * E * sizeof(struct myRAM);
    myCache = malloc(num);
    memset(myCache, 0, num);

    char identifier;
    ul address;
    int size;

    while (fscanf(fp, " %c %lx,%d", &identifier, &address, &size) > 0) {
        
        if (verbose)
            printf("%c %lx,%d", identifier, address, size);
        
        address = address - 1 + size;
        switch (identifier)
        {
        case 'M':
            update(address);
        case 'L':
        case 'S':
            update(address);
            if (verbose) printf("\n");
            break;
        default:
            break;
        }
    }

    printSummary(hits, misses, evictions);
    fclose(fp);
    free(myCache);
    return 0;
}
