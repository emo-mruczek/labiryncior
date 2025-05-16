#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <png.h>

#define HEADER_SIZE 8

// TODO: put it all into functions to make code less messy
    
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

    /* basic info */

    fprintf(stdout, " Compiled with libpng %s. \n", PNG_LIBPNG_VER_STRING /*, ZLIB_VERSION, zlib_version*/ );

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

    png_read_info(png_ptr, png_info_ptr);
    
    /* getting some info about the file */

    png_uint_32 height;
    png_uint_32 width;
    int bit_depth;
    int color_type;

    png_get_IHDR(png_ptr, png_info_ptr, &width, &height, &bit_depth, &color_type, NULL, NULL, NULL);
    fprintf(stdout, " Width: %d Height: %d Bit depth: %d Color type: %d\n", width, height, bit_depth, color_type);

    /* reading rows */

    png_bytep row_pointers[height];

    for (size_t row = 0; row < height; row++) row_pointers[row] = NULL;
    for (size_t row = 0; row < height; row++) row_pointers[row] = png_malloc(png_ptr, png_get_rowbytes(png_ptr, png_info_ptr)); 

    png_read_image(png_ptr, row_pointers);

    fprintf(stdout, " nr of rows: %d \n", nr_of_rows);

    png_destroy_read_struct(&png_ptr, &png_info_ptr, NULL);

    // TODO compress
    bool is_black = false;
    for(int y = 0; y < height; y += 2) {
        png_bytep row = row_pointers[y];
        for(int x = 0; x < width; x += 2) {
            png_bytep px = &(row[x * 4]);
            // printf(" %3d, %3d = RGBA(%3d, %3d, %3d, %3d)\n", x, y, px[0], px[1], px[2], px[3]);
            if (px[0] == 0) {
                is_black = true;
                fprintf(stdout, "." );
            } else {
                if (is_black) {
                    fprintf(stdout, " ");
                    is_black = false;
                } else {
                    printf("*");
                }
            }
        }
        fprintf(stdout, "\n");
    }

    /* okay so 
     * look at squares
     * if side is black - there is a wall and you cannot go there
     * this squares are nodes 
     * nodes are structs with pointers to the nodes (one for each way) -> null if no connection
     * booleans for start/finish value or sth 
     */

    /* clean up */

    png_ptr = NULL;
    png_info_ptr = NULL;

    fclose(fp);

    return EXIT_SUCCESS;
}
