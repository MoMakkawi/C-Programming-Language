#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <string.h>

#pragma region Exercise1

// Function to swap two float values
static void exchange(float* x1, float* x2) {
    float temp = *x1;
    *x1 = *x2;
    *x2 = temp;
}

// Function to get values either from command line arguments or user input
static void getValues(int argc, char* argv[], float* x1, float* x2) {
    if (argc == 3) {
        // If command line arguments are provided, use them
        *x1 = atof(argv[1]);
        *x2 = atof(argv[2]);
    }
    else {
        // Otherwise, ask the user for input
        printf("Enter the first number: ");
        scanf("%f", x1);
        printf("Enter the second number: ");
        scanf("%f", x2);
    }
}

// Function to swap two float values
static void E1_Swap2FloatValues(int argc, char* argv[]) {
    float x1, x2;

    // Get values
    getValues(argc, argv, &x1, &x2);

    // Swap the values
    exchange(&x1, &x2);

    // Display the result
    printf("After swapping:\n");
    printf("First number: %.2f\n", x1);
    printf("Second number: %.2f\n", x2);

    printf("_____________________________________________ \n");
}
#pragma endregion
#pragma region Exercise2

// Function to fill the matrix with random numbers
static void fillMatrix(float** matrix, int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            matrix[i][j] = (float)(rand() % 1000) / 100;
        }
    }
}

// Function to print the matrix
static void printMatrix(float** matrix, int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            printf("%.2f ", matrix[i][j]);
        }
        printf("\n");
    }
}

// Function to transpose the matrix
static void transposeMatrix(float** matrix, int size) {
    for (int i = 0; i < size; i++) {
        for (int j = i + 1; j < size; j++) {
            exchange(&matrix[i][j], &matrix[j][i]);
        }
    }
}

// Function to handle matrix transposition
static void E2_MatrixTransposition() {
    int size;
    printf("Enter the dimension of the matrix: ");
    scanf("%d", &size);

    // Allocate memory for the matrix
    float** matrix = (float**)malloc(size * sizeof(float*));
    for (int i = 0; i < size; i++) {
        matrix[i] = (float*)malloc(size * sizeof(float));
    }

    // Seed the random number generator
    srand(time(0));

    // Fill the matrix with random numbers
    fillMatrix(matrix, size);

    // Print the initial matrix
    printf("Initial matrix:\n");
    printMatrix(matrix, size);

    // Transpose the matrix
    transposeMatrix(matrix, size);

    // Print the transposed matrix
    printf("Transposed matrix:\n");
    printMatrix(matrix, size);

    // Deallocate memory
    for (int i = 0; i < size; i++)
        free(matrix[i]);

    free(matrix);

    printf("_____________________________________________ \n");
}

#pragma endregion
#pragma region Exercise3

// Enum for case conversion
typedef enum {
    LOWER_CASE,
    UPPER_CASE
} CaseType;

// Function to change the case of the string
static void changeCase(char* str, CaseType caseType) {
    for (int i = 0; str[i] != '\0'; i++)
        str[i] = caseType == LOWER_CASE ? tolower(str[i]) : toupper(str[i]);
}


// Function to split the string into words and calculate statistics
static void processWords(char* str) {
    char** words = NULL;
    int wordCount = 0;
    int totalLetters = 0;

    char* context;  // context pointer for strtok_r
    char* token = strtok_r(str, " ", &context);
    while (token != NULL) {
        words = realloc(words, (wordCount + 1) * sizeof(char*));
        if (words == NULL) {
            perror("realloc failed");
            exit(EXIT_FAILURE);
        }
        words[wordCount] = token;
        totalLetters += strlen(token);
        wordCount++;
        token = strtok_r(NULL, " ", &context);
    }

    if (wordCount > 0) {
        printf("The total number of words is %d and the average number of letters per word is %.2f\n",
            wordCount, (float)totalLetters / wordCount);
    }
    else {
        printf("No words found.\n");
    }

    // Free the allocated memory for words array
    free(words);
}


// Function to read text from the user
static char* readText() {
    char* text = NULL;
    int size = 0;
    int capacity = 10;

    text = malloc(capacity * sizeof(char));
    if (!text) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }

    printf("Please give your text and press enter when you finish:\n");
    char ch;
    while ((ch = getchar()) != '\n') {
        if (size >= capacity) {
            capacity *= 2;
            text = realloc(text, capacity * sizeof(char));
            if (!text) {
                fprintf(stderr, "Memory allocation failed\n");
                exit(1);
            }
        }
        text[size++] = ch;
    }
    text[size] = '\0';

    return text;
}

// Function to handle text processing
static void E3_TextProcessing() {
    char* text = readText();

    printf("You gave the following string: %s\n", text);

    int caseChoice;
    printf("Please choose if you want to change your string to lower-case (type 0) or upper-case (type 1):\n");
    scanf("%d", &caseChoice);

    changeCase(text, caseChoice == 0 ? LOWER_CASE : UPPER_CASE);

    printf("Here are the words that you gave after changing the case:\n%s\n", text);

    processWords(text);

    // Free the allocated memory for text
    free(text);

    printf("________________________________________\n");
}

#pragma endregion



int main(int argc, char* argv[]) {

    int choice;

    printf("Choose an exercise to run:\n");
    printf("1. E1 - Swap 2 Float Values\n");
    printf("2. E2 - Matrix Transposition\n");
    printf("3. E3 - Text Processing\n");
    printf("Enter your choice (1-3): ");
    scanf("%d", &choice);

    switch (choice) {
    case 1:
        E1_Swap2FloatValues(argc, argv);
        break;
    case 2:
        E2_MatrixTransposition();
        break;
    case 3:
        E3_TextProcessing();
        break;
    default:
        printf("Invalid choice.\n");
        break;
    }

    return 0;
}
