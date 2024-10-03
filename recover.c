#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#define BLOCK_SIZE 512

int main(int argc, char *argv[])
{
    // Check for correct number of arguments
    if (argc != 2)
    {
        printf("Usage: ./recover IMAGE\n");
        return 1;
    }

    // Open memory card file
    char *file = argv[1];
    FILE *raw_file = fopen(file, "r");
    if (raw_file == NULL)
    {
        printf("Could not open %s.\n", file);
        return 1;
    }

    // Initialize variables
    bool found_jpg = false;
    int jpg_count = 0;
    uint8_t buffer[BLOCK_SIZE];
    char jpg_name[8];
    FILE *outptr = NULL;

    // Read through the raw file
    while (fread(buffer, BLOCK_SIZE, 1, raw_file) == 1)
    {
        // Check if the block is the start of a JPEG file
        if (buffer[0] == 0xff && buffer[1] == 0xd8 && buffer[2] == 0xff && (buffer[3] & 0xf0) == 0xe0)
        {
            // If already found a jpg, close previous one
            if (found_jpg)
            {
                fclose(outptr);
            }
            else
            {
                found_jpg = true;
            }

            // Create a new file for the new jpg
            sprintf(jpg_name, "%03d.jpg", jpg_count);
            outptr = fopen(jpg_name, "w");

            if (outptr == NULL)
            {
                fclose(raw_file);
                printf("Could not create %s.\n", jpg_name);
                return 1;
            }

            jpg_count++;
        }

        // If a jpg is found, write to the output file
        if (found_jpg)
        {
            fwrite(buffer, BLOCK_SIZE, 1, outptr);
        }
    }

    // Close any remaining open files
    fclose(raw_file);
    if (found_jpg)
    {
        fclose(outptr);
    }

    return 0;
}
