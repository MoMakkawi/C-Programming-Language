#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "packing_Wang.h"  // Custom header for file packing/unpacking functions

#pragma region Exercise 2.1 - Gaussian Plotting with gnuplot

// Define M_PI if not defined (for portability)
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Exercise 2.1: Generate Gaussian distribution values and plot with gnuplot
static void E1_plot_gaussian(int argc, char* argv[]) {
    // Expecting 3 arguments: mean, stddev, samples
    if (argc < 4) {
        fprintf(stderr, "Usage: %s <mean> <stddev> <samples>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Convert command-line arguments to appropriate types
    double mean = atof(argv[1]);
    double stddev = atof(argv[2]);
    int samples = atoi(argv[3]);

    // Open a file to write Gaussian data
    FILE* fp = fopen("function_data.txt", "w");
    if (fp == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    // Step size to cover ±3 stddev around mean
    double step = (6 * stddev) / (samples - 1);

    // Write x, y values based on Gaussian formula
    for (int i = 0; i < samples; i++) {
        double x = mean - 3 * stddev + i * step;
        double y = (1 / (stddev * sqrt(2 * M_PI))) * exp(-0.5 * pow((x - mean) / stddev, 2));
        fprintf(fp, "%lf %lf\n", x, y);
    }

    fclose(fp);

    // Use gnuplot to plot the generated data file
    system("gnuplot --persist -e \"plot 'function_data.txt'\"");
}

#pragma endregion
#pragma region Exercise 2.2 - Binary to Text Conversion for gnuplot

// Exercise 2.2: Convert binary file of floats to a text file for plotting
static void E2_convert_binary_to_text(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Usage: %s <binary_filename> <text_filename>\n", argv[0]);
        return;
    }

    const char* binary_filename = argv[1];
    const char* text_filename = argv[2];

    // Open binary file for reading
    FILE* bin_fp = fopen(binary_filename, "rb");
    if (bin_fp == NULL) {
        perror("Error opening binary file");
        exit(EXIT_FAILURE);
    }

    // Determine size of file and number of float values
    fseek(bin_fp, 0, SEEK_END);
    long file_size = ftell(bin_fp);
    rewind(bin_fp);

    int num_floats = file_size / sizeof(float);

    // Ensure the float count is even (x/y pairs)
    if (num_floats % 2 != 0) {
        fprintf(stderr, "Invalid binary file format: odd number of floats.\n");
        fclose(bin_fp);
        exit(EXIT_FAILURE);
    }

    int num_points = num_floats / 2;

    // Allocate memory to hold x and y values
    float* x_vals = (float*)malloc(num_points * sizeof(float));
    float* y_vals = (float*)malloc(num_points * sizeof(float));

    if (!x_vals || !y_vals) {
        fprintf(stderr, "Memory allocation failed.\n");
        fclose(bin_fp);
        exit(EXIT_FAILURE);
    }

    // Read binary values into memory
    fread(x_vals, sizeof(float), num_points, bin_fp);
    fread(y_vals, sizeof(float), num_points, bin_fp);
    fclose(bin_fp);

    // Write values to text file in readable format
    FILE* txt_fp = fopen(text_filename, "w");
    if (txt_fp == NULL) {
        perror("Error opening text file");
        free(x_vals);
        free(y_vals);
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < num_points; ++i) {
        fprintf(txt_fp, "%f %f\n", x_vals[i], y_vals[i]);
    }

    fclose(txt_fp);
    free(x_vals);
    free(y_vals);

    // Optional: Auto-plot the text data using gnuplot
    char command[256];
    snprintf(command, sizeof(command), "gnuplot --persist -e \"plot '%s' with linespoints\"", text_filename);
    system(command);
}

#pragma endregion
#pragma region Exercise 2.3 - File Packing and Unpacking

// Exercise 2.3: Handle file packing and unpacking operations
static void E3_file_packing(int argc, char* argv[]) {

    // Require at least 3 arguments to proceed
    if (argc < 3) {
        printf("Usage:\n");
        printf("  To pack: %s pack output.pak file1 file2 ...\n", argv[0]);
        printf("  To unpack: %s unpack output.pak\n", argv[0]);
        return;
    }

    // Pack multiple files into one .pak archive
    if (strcmp(argv[1], "pack") == 0) {
        filepacking(argv[2], argc - 3, &argv[3]);
    }
    // Unpack .pak archive into individual files
    else if (strcmp(argv[1], "unpack") == 0) {
        fileunpacking(argv[2]);
    }
    else {
        printf("Unknown command.\n");
    }

}
#pragma endregion

// Main entry point to choose which exercise to run
int main(int argc, char* argv[]) {

    int choice;

    // Show exercise menu
    printf("Choose an exercise to run:\n");
    printf("1. E1 - Plot Gaussian\n");
    printf("2. E2 - Convert Binary to Text\n");
    printf("3. E3 - File Packing\n");
    printf("Enter your choice (1-3): ");
    scanf("%d", &choice);

    // Dispatch to selected exercise
    switch (choice) {
    case 1:
        E1_plot_gaussian(argc, argv);
        break;
    case 2:
        E2_convert_binary_to_text(argc, argv);
        break;
    case 3:
        E3_file_packing(argc, argv);
        break;
    default:
        printf("Invalid choice.\n");
        break;
    }

    return 0;
}