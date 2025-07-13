#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>

#define NUM_THREADS 4

struct stat st;

typedef struct {
    int in_fd;
    int out_fd;
    off_t offset;
    size_t size;
    unsigned char *key;
    size_t key_len;
    int thread_id;
} thread_data_t;

void *encrypt_chunk(void *arg) {
    thread_data_t *data = (thread_data_t *)arg;

    unsigned char *in_buf = (unsigned char *)malloc(data->size);
    unsigned char *out_buf = (unsigned char *)malloc(data->size);

    if (!in_buf || !out_buf) {
        perror("malloc failed");
        pthread_exit(NULL);
    }

    ssize_t bytes_read = pread(data->in_fd, in_buf, data->size, data->offset);
    if (bytes_read < 0) {
        perror("pread failed");
        free(in_buf);
        free(out_buf);
        pthread_exit(NULL);
    }

    // XOR encryption/decryption
    for (size_t i = 0; i < data->size; i++) {
        out_buf[i] = in_buf[i] ^ data->key[(data->offset + i) % data->key_len];
    }

    ssize_t bytes_written = pwrite(data->out_fd, out_buf, data->size, data->offset);
    if (bytes_written < 0) {
        perror("pwrite failed");
        free(in_buf);
        free(out_buf);
        pthread_exit(NULL);
    }

    free(in_buf);
    free(out_buf);
    return NULL;
}

int main(int argc, char **argv) {
    if (argc != 5) {
        fprintf(stderr, "Usage: %s <-e|-d> <input_file> <output_file> <key>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Operation type
    char *operation = argv[1];
    if (strcmp(operation, "-e") != 0 && strcmp(operation, "-d") != 0) {
        fprintf(stderr, "Invalid operation. Use -e for encrypt or -d for decrypt.\n");
        exit(EXIT_FAILURE);
    }

    // Open input file
    int in_fd = open(argv[2], O_RDONLY);
    if (in_fd < 0) {
        perror("Failed to open input file");
        exit(EXIT_FAILURE);
    }

    // Open/create output file
    int out_fd = open(argv[3], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (out_fd < 0) {
        perror("Failed to open output file");
        close(in_fd);
        exit(EXIT_FAILURE);
    }

    // Get file size
    if (fstat(in_fd, &st) < 0) {
        perror("fstat failed");
        close(in_fd);
        close(out_fd);
        exit(EXIT_FAILURE);
    }
    size_t file_size = st.st_size;

    // Set output file size
    if (ftruncate(out_fd, file_size) < 0) {
        perror("ftruncate failed");
        close(in_fd);
        close(out_fd);
        exit(EXIT_FAILURE);
    }

    // Encryption key
    unsigned char *key = (unsigned char *)argv[4];
    size_t key_len = strlen((char *)key);

    pthread_t threads[NUM_THREADS];
    thread_data_t thread_data[NUM_THREADS];
    off_t chunk_size = file_size / NUM_THREADS;
    off_t offset = 0;

    // Create threads
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_data[i].in_fd = in_fd;
        thread_data[i].out_fd = out_fd;
        thread_data[i].offset = offset;
        thread_data[i].size = (i == NUM_THREADS - 1) ? (file_size - offset) : chunk_size;
        thread_data[i].key = key;
        thread_data[i].key_len = key_len;
        thread_data[i].thread_id = i;

        if (pthread_create(&threads[i], NULL, encrypt_chunk, &thread_data[i]) != 0) {
            perror("pthread_create failed");
            close(in_fd);
            close(out_fd);
            exit(EXIT_FAILURE);
        }
        offset += chunk_size;
    }

    // Join threads
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    close(in_fd);
    close(out_fd);

    if (strcmp(operation, "-e") == 0) {
        printf("Encryption complete.\n");
    } else {
        printf("Decryption complete.\n");
    }

    return 0;
}
