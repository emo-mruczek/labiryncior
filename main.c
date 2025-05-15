#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <png.h>

#define HEADER_SIZE 8
#define PNG_LIBPNG_VER_STRING "1.6.46"

int main(int argc, char** argv) {

    if (argc < 2) {
        fprintf(stdout,  "provide a png file you dumass! \n");
        exit(EXIT_FAILURE);
    }

    FILE *fp = fopen(argv[1], "r"); // add "b" if windows but WINDOWS SUCKS SO NO!!!
    
    if (!fp) {
        fprintf(stdout, "couldn't open the file!\n");
        exit(EXIT_FAILURE);
    }

    uint8_t header[HEADER_SIZE] = {0, 0, 0, 0, 0, 0, 0, 0};

    if (fread(header, sizeof(header[0]), HEADER_SIZE, fp) != HEADER_SIZE) {
        fprintf(stdout, "error reading header bytes\n");
        exit(EXIT_FAILURE);
    } 

    /* check whether file is a png */
    
    bool is_png = (png_sig_cmp(header, 0, HEADER_SIZE) == 0); 
    if (!is_png) {
        fprintf(stdout, "not a png!\n");
        exit(EXIT_FAILURE);
    }

    /* initializing the necessary structures */

    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if (!png_ptr) {
        fprintf(stdout, "failed to create a png struct!\n");
        exit(EXIT_FAILURE);
    }

    png_infop png_info_ptr = png_create_info_struct(png_ptr);

    if (!png_info_ptr) {
        fprintf(stdout, "couldn't create a png info struct!\n");
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        exit(EXIT_FAILURE);
    }

    printf("dupa!");
    return EXIT_SUCCESS;
}
