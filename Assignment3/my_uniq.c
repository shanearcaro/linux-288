#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/**
 * Uniq command parameters
 * -c, --count: prefix lines by the number of occurrences
 * -d, --repeated: only print duplicate lines, one for each group
 * -u, --unique: only print unique lines
 * -f [fields]: Ignore the first fields on each input line when doing comparisons
 * -s [char]: Ignore the first chars characters when doing comparisons
 * [input_file [output_file]]: specify input file and output file
 * 
 */

int main(int argc, char* argv[]) {
    int opt;
    // Not handeling input file, output file currently
    while ((opt=getopt(argc, argv, ":cduf:s:")) != -1) {
        switch(opt) {
            case 'c':
            case 'd':
            case 'u':
                printf("Option: %c\n", opt);
                break;
            case 'f':
                printf("Fields: %s\n", optarg);
                break;
            case 's':
                printf("Chars: %s\n", optarg);
                break;
            case ':':
                printf("Option %c needs arguments\n", optopt);
                break;
            case '?':
                printf("Unknown option flag: %c\n", optopt);
                break;
        }
        
    }

    for (; optind < argc; optind++) {
        printf("argument: %s\n", argv[optind]);
    }
}
