#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <png.h>

#define HEADER_SIZE 8
#define PNG_LIBPNG_VER_STRING "1.6.46"

 // TODO: do stuff with it or sth, this is just for the testing purposes
    
    uint32_t nr_of_rows = 0;

    void read_row(png_structp png_ptr, png_uint_32 row, int pass) {
        nr_of_rows++;
    }

int main(int argc, char** argv) {

    if (argc < 2) {
        fprintf(stdout, "provide a png file you dumass!\n");
        exit(EXIT_FAILURE);
    }

    FILE *fp = fopen(argv[1], "r"); // add "b" if windows but WINDOWS SUCKS SO NO!!!
    
    if (!fp) {
        fprintf(stdout, "couldn't open the file!\n");
        exit(EXIT_FAILURE);
    }

    uint8_t header[HEADER_SIZE] = {0, 0, 0, 0, 0, 0, 0, 0}; // i mean 

    if (fread(header, sizeof(header[0]), HEADER_SIZE, fp) != HEADER_SIZE) {
        fprintf(stdout, "error reading header bytes\n");
        exit(EXIT_FAILURE);
    } 

    /* check whether file is a png */
    
    const bool is_png = (png_sig_cmp(header, 0, HEADER_SIZE) == 0); 
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

    /* jumping when libpng encounters an error */

    if (setjmp(png_jmpbuf(png_ptr))) {
        fprintf(stdout, "jumping from libpng, caused by an error!\n"); 
        png_destroy_read_struct(&png_ptr, &png_info_ptr, NULL); // TODO: set the last one
        fclose(fp);
        exit(EXIT_FAILURE);
    }

    /* setting up the input code */

    png_init_io(png_ptr, fp);
    png_set_sig_bytes(png_ptr, HEADER_SIZE);

    /* reading, finally! */

    png_set_read_status_fn(png_ptr, read_row); 

    /* png_transforms - integer containing the bitwise OR of some set of transformation flags */

    int png_transforms = 0;

    png_read_png(png_ptr, png_info_ptr, png_transforms, NULL);

    fprintf(stdout, "nr of rows: %d \n", nr_of_rows);


    printf("dupa!");
    fclose(fp);
    return EXIT_SUCCESS;
}
