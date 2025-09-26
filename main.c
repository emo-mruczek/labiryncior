/* i am aware that this implementation is not efficient
 * i made it in order to get familiar with libpng
 * and learn about maze-solving algorithms */

// TODO: solving it real time?

#include <png.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define HEADER_SIZE 8

static png_uint_32 height;
static png_uint_32 width;
static int bit_depth;
static int color_type;
static png_bytep *row_pointers;
static bool **maze;
static bool **visited;
static bool **solution;
static int start_x;
static int start_y;
static int end_x;
static int end_y;

void read_png(char const *const filename);
void process_png();
bool solve(int x, int y);

int main(int argc, char **argv) {

  if (argc < 2) {
    fprintf(stdout, "provide a png file you dumass!\n");
    exit(EXIT_FAILURE);
  }

  read_png(argv[1]);
  process_png();

  return EXIT_SUCCESS;
}

// https://www.wikiwand.com/en/articles/Maze-solving_algorithm

void process_png() {

  // assersts
  // TODO:
  // https://stackoverflow.com/questions/42094465/correctly-allocating-multi-dimensional-arrays

  maze = calloc(height, sizeof(bool *));
  for (int i = 0; i < height; ++i) {
    maze[i] = calloc(width, sizeof(bool));
  }

  visited = calloc(height, sizeof(bool *));
  for (int i = 0; i < height; ++i) {
    visited[i] = calloc(width, sizeof(bool));
  }

  solution = calloc(height, sizeof(bool *));
  for (int i = 0; i < height; ++i) {
    solution[i] = calloc(width, sizeof(bool));
  }

  /* printing is just for visualisation */
  for (int y = 0; y < height; y++) {
    png_bytep row = row_pointers[y];
    for (int x = 0; x < width; x++) {
      png_bytep px = &(row[x * 4]);
      if (px[0] == 0) {
        fprintf(stdout, "**");
        maze[x][y] = true;
      } else {
        fprintf(stdout, "  ");
        maze[x][y] = false;
      }
    }
    fprintf(stdout, "\n");
  }

  /* finding start and end
   * disc: must be at first and last columns */
  // TODO: make it search also in a first and last row in order to make a code
  // more universal
  
  fprintf(stdout, "\n looking for enter...\n");

  for (int y = 0; y < height; y++) {
    png_bytep row = row_pointers[y];
    png_bytep px = &(row[0]);
    if (px[0] == 0) {
      fprintf(stdout, "wall\n");
    } else {
      fprintf(stdout, "enter %d\n", y);
      start_x = 0;
      start_y = y;
      break;
    }
  }

  fprintf(stdout, "\n looking for exit...\n");

  for (int y = 0; y < height; y++) {
    png_bytep row = row_pointers[y];
    png_bytep px = &(row[(width - 1) * 4]);
    if (px[0] == 0) {
      fprintf(stdout, "wall\n");
    } else {
      fprintf(stdout, "exit %d\n", y);
      end_x = width - 1;
      end_y = y;
      break;
    }
  }

  fprintf(stdout, "\n enter: %d %d\n exit: %d %d\n", start_x, start_y, end_x, end_y);

  // TODO: fix
  bool is_solution = solve(start_x, start_y);

  fprintf(stdout, "\n%d\n", is_solution);

  // TODO: free
}

bool solve(int x, int y) {

  if (x == end_x && y == end_y)
    return true;

  if (maze[x][y] || visited[x][y])
    return false;

  visited[x][y] = true;

  if (x != 0)
    if (solve(x - 1, y)) {
      solution[x][y] = true; 
    }
  if (x != width - 1)               
    if (solve(x + 1, y)) { 
      solution[x][y] = true;
      return true;
    }
  if (y != 0)                       
    if (solve(x, y - 1)) { 
      solution[x][y] = true;
      return true;
    }
  if (y != height - 1)              
    if (solve(x, y + 1)) { 
      solution[x][y] = true;
      return true;
    }
  return false;
}

/* png stuff */

void read_png(char const *const filename) {
  FILE *fp =
      fopen(filename, "r"); // add "b" if windows but WINDOWS SUCKS SO NO!!!

  if (!fp) {
    fprintf(stdout, "couldn't open the file!\n");
    exit(EXIT_FAILURE);
  }

  uint8_t header[HEADER_SIZE] = {0, 0, 0, 0, 0,
                                 0, 0, 0}; // TODO: memset(arr, 0, sizeof arr);

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

  fprintf(stdout, " Compiled with libpng %s. \n",
          PNG_LIBPNG_VER_STRING /*, ZLIB_VERSION, zlib_version*/);

  /* initializing the necessary structures */

  png_structp png_ptr =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

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
    png_destroy_read_struct(&png_ptr, &png_info_ptr,
                            NULL); // TODO: set the last one
    fclose(fp);
    exit(EXIT_FAILURE);
  }

  /* setting up the input code */

  png_init_io(png_ptr, fp);
  png_set_sig_bytes(png_ptr, HEADER_SIZE);

  /* reading, finally! */

  png_read_info(png_ptr, png_info_ptr);

  /* getting some info about the file */

  png_get_IHDR(png_ptr, png_info_ptr, &width, &height, &bit_depth, &color_type,
               NULL, NULL, NULL);
  fprintf(stdout, " Width: %d Height: %d Bit depth: %d Color type: %d\n", width,
          height, bit_depth, color_type);

  /* reading rows */

  row_pointers = malloc(sizeof(png_bytep) * height);

  for (size_t row = 0; row < height; row++)
    row_pointers[row] = NULL;
  for (size_t row = 0; row < height; row++)
    row_pointers[row] =
        png_malloc(png_ptr, png_get_rowbytes(png_ptr, png_info_ptr));

  png_read_image(png_ptr, row_pointers);

  /* clean up */

  png_destroy_read_struct(&png_ptr, &png_info_ptr, NULL);
  png_ptr = NULL;
  png_info_ptr = NULL;

  fclose(fp);
}

