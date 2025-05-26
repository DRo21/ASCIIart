#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize.h"

const char *ASCII_CHARS = "@%#*+=-:. ";

char get_ascii_char(int r, int g, int b) {
    float brightness = 0.2126f * r + 0.7152f * g + 0.0722f * b;
    int index = (int)((brightness / 255.0f) * (strlen(ASCII_CHARS) - 1));
    return ASCII_CHARS[index];
}

void get_terminal_size(int *width, int *height) {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    *width = w.ws_col;
    *height = w.ws_row;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: %s <image_path> [--color]\n", argv[0]);
        return 1;
    }

    int use_color = 0;
    if (argc >= 3 && strcmp(argv[2], "--color") == 0) {
        use_color = 1;
    }

    int width, height, channels;
    unsigned char *img = stbi_load(argv[1], &width, &height, &channels, 3);
    if (!img) {
        fprintf(stderr, "Failed to load image.\n");
        return 1;
    }

    int term_width, term_height;
    get_terminal_size(&term_width, &term_height);

    int target_width = term_width;
    int target_height = (height * target_width / width) / 2;

    if (target_height > term_height - 2) {
        target_height = term_height - 2;
    }

    unsigned char *resized = malloc(target_width * target_height * 3);
    if (!resized) {
        fprintf(stderr, "Failed to allocate memory for resized image.\n");
        stbi_image_free(img);
        return 1;
    }

    stbir_resize_uint8(img, width, height, 0,
                       resized, target_width, target_height, 0, 3);
    stbi_image_free(img);

    for (int y = 0; y < target_height; ++y) {
        for (int x = 0; x < target_width; ++x) {
            int idx = (y * target_width + x) * 3;
            unsigned char r = resized[idx];
            unsigned char g = resized[idx + 1];
            unsigned char b = resized[idx + 2];

            char ascii = get_ascii_char(r, g, b);

            if (use_color) {
                printf("\x1b[38;2;%d;%d;%dm%c", r, g, b, ascii);
            } else {
                printf("%c", ascii);
            }
        }
        printf("\x1b[0m\n");
    }

    free(resized);
    return 0;
}
