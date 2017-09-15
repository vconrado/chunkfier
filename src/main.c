#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


void usage(int    argc,
           char **argv);
           
int  check_params(uint32_t x_size,
                  uint32_t x_chunk_size,
                  uint32_t y_size,
                  uint32_t y_chunk_size,
                  uint32_t z_size,
                  uint32_t z_chunk_size,
                  uint32_t data_size);

int chunkfy(uint32_t x_size,
                      uint32_t x_chunk_size,
                      uint32_t y_size,
                      uint32_t y_chunk_size,
                      uint32_t z_size,
                      uint32_t z_chunk_size,
                      uint32_t data_size,
                      FILE *input_file,
                      FILE *output_file);


int chunkfy_with_cache(uint32_t x_size,
                      uint32_t x_chunk_size,
                      uint32_t y_size,
                      uint32_t y_chunk_size,
                      uint32_t z_size,
                      uint32_t z_chunk_size,
                      uint32_t data_size,
                      FILE *input_file,
                      FILE *output_file);

int main(int argc, char *argv[]) {
  uint32_t x_size, x_chunk_size;
  uint32_t y_size, y_chunk_size;
  uint32_t z_size, z_chunk_size;


  uint32_t data_size;
  uint32_t file_size;
  FILE    *input_file;
  FILE    *output_file;
  char    *input_file_name;
  char    *output_file_name;
  int ret;

  if (argc != 10) {
    usage(argc, argv);
    return 1;
  }
  x_size           = atoi(argv[1]);
  x_chunk_size     = atoi(argv[2]);
  y_size           = atoi(argv[3]);
  y_chunk_size     = atoi(argv[4]);
  z_size           = atoi(argv[5]);
  z_chunk_size     = atoi(argv[6]);
  data_size        = atoi(argv[7]);
  input_file_name  = argv[8];
  output_file_name = argv[9];

  if (check_params(x_size,               x_chunk_size,               y_size,
                   y_chunk_size,               z_size,               z_chunk_size,
                   data_size)) {
    return 2;
  }

  // Abrindo arquivos
  input_file = fopen(input_file_name, "rb");

  if (input_file == NULL) {
    fprintf(stderr, "Nao foi possivel abrir o arquivo %s\n", input_file_name);
    return 3;
  }

  // Verificando tamanho do arquivos
  fseek(input_file, 0L, SEEK_END);
  file_size = ftell(input_file);
  fseek(input_file, 0L, SEEK_SET);
  uint32_t total_size = (x_size * y_size * z_size * data_size);

  if (file_size != total_size) {
    fprintf(stderr,
            "O tamanho do arquivo '%s' e' incompatível como os dados informados.\n",
            input_file_name);
    return 4;
  }

  output_file = fopen(output_file_name, "wb");

  if (output_file == NULL) {
    fprintf(stderr, "Nao foi possivel abrir o arquivo %s\n", output_file_name);
    return 5;
  }
  
  ret = chunkfy_with_cache(x_size,
                      x_chunk_size,
                      y_size,
                      y_chunk_size,
                      z_size,
                      z_chunk_size,
                      data_size,
                      input_file,
                      output_file);


  fclose(input_file);
  fclose(output_file);
  return ret;
}

void usage(int argc, char **argv) {
  fprintf(
    stderr,
    "Usage: %s X-size X-chunk-size Y-size Y-chunk-size Z-size Z-chunk-size data-size input_file output_file\n",
    argv[0]);
}

int check_params(uint32_t x_size,
                 uint32_t x_chunk_size,
                 uint32_t y_size,
                 uint32_t y_chunk_size,
                 uint32_t z_size,
                 uint32_t z_chunk_size,
                 uint32_t data_size) {
  if ((x_chunk_size < 1) || (y_chunk_size < 1) || (z_chunk_size < 1)) {
    fprintf(stderr, "Chunk size must be > 1.\n");
    return 1;
  }

  if ((x_size < x_chunk_size) || (y_size < y_chunk_size) ||
      (z_size < z_chunk_size)) {
    fprintf(stderr, "Domain size must equal or greather than chunk size.\n");
    return 2;
  }

  if (data_size < 1) {
    fprintf(stderr, "Data size must be > 1.\n");
    return 3;
  }
  return 0;
}


int chunkfy(uint32_t x_size,
                      uint32_t x_chunk_size,
                      uint32_t y_size,
                      uint32_t y_chunk_size,
                      uint32_t z_size,
                      uint32_t z_chunk_size,
                      uint32_t data_size,
                      FILE *input_file,
                      FILE *output_file) {
  uint32_t x, y, z;
  uint32_t xc, yc, zc;
  uint32_t seek;
  char *buf;
  
  buf = (char *)malloc(sizeof(char) * (data_size + 1));

  buf[data_size] = 0;

  for (x = 0; x < x_size; x += x_chunk_size) {
    for (y = 0; y < y_size; y += y_chunk_size) {
      for (z = 0; z < z_size; z += z_chunk_size) {
        for (xc = 0; xc < x_chunk_size; ++xc) {
          for (yc = 0; yc < y_chunk_size; ++yc) {
            for (zc = 0; zc < z_chunk_size; ++zc) {
              seek = (x + xc) + (y + yc) * x_size + (z + zc) *
                     x_size * y_size;
              fseek(input_file, seek * data_size, SEEK_SET);

              if (fread(buf, sizeof(char), data_size, input_file) != data_size) {
                fprintf(stderr, "Error while reading input file");
                free(buf);
                return 10;
              }
              fwrite(buf, sizeof(char), data_size, output_file);
            }
          }
        }
        printf(". ");
        fflush(stdout);
      }
    }
  }
  free(buf);
  return 0;
}


int chunkfy_with_cache(uint32_t x_size,
                      uint32_t x_chunk_size,
                      uint32_t y_size,
                      uint32_t y_chunk_size,
                      uint32_t z_size,
                      uint32_t z_chunk_size,
                      uint32_t data_size,
                      FILE *input_file,
                      FILE *output_file) {
  uint32_t x, y, z;
  uint32_t xc, yc, zc;
  uint32_t seek;
  uint32_t content_size;
  char *content;
  
  content_size = x_size*y_size*z_size*data_size;
  printf("Alocando %d bytes para cache\n", content_size);
  content = (char*) malloc(sizeof(char)*content_size);
  if (fread(content, sizeof(char), content_size, input_file) != content_size) {
    fprintf(stderr, "Error while reading input file");
    free(content);
    return 10;
  }

  for (x = 0; x < x_size; x += x_chunk_size) {
    for (y = 0; y < y_size; y += y_chunk_size) {
      for (z = 0; z < z_size; z += z_chunk_size) {
        for (xc = 0; xc < x_chunk_size; ++xc) {
          for (yc = 0; yc < y_chunk_size; ++yc) {
            for (zc = 0; zc < z_chunk_size; ++zc) {
              seek = (x + xc) + (y + yc) * x_size + (z + zc) *
                     x_size * y_size;              
              fwrite(&content[seek * data_size], sizeof(char), data_size, output_file);
            }
          }
        }
        printf(". ");
        fflush(stdout);
      }
    }
  }
  free(content);
  return 0;
}
