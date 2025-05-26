#include <stdio.h>
#include "logical_MAKKAWI.h"  // Header file declaration for this module

// Function for logical AND operation using short-circuit logic (non-zero is true)
static int and_operation(int a, int b) { return a && b; }

// Function for logical OR operation using short-circuit logic (non-zero is true)
static int or_operation(int a, int b) { return a || b; }

// Function for bitwise XOR operation (works like logical XOR for 0/1)
static int xor_operation(int a, int b) { return a ^ b; }

// Function to demonstrate the selected logical operation (AND, OR, XOR)
void logical_operator_use(char x) {
    const char* operator_name;         // Name of the logical operator to display
    int (*operation)(int, int);        // Pointer to the selected operation function

    // Determine which logical operator to use based on input character
    switch (x) {
    case 'A':
        operator_name = "AND";
        operation = and_operation;
        break;
    case 'O':
        operator_name = "OR";
        operation = or_operation;
        break;
    case 'X':
        operator_name = "XOR";
        operation = xor_operation;
        break;
    default:
        // Handle invalid input character
        printf("Invalid operator\n");
        return;
    }

    // Print the truth table header
    printf("%s Table:\n", operator_name);
    printf("a b | result\n");

    // Loop through all possible input combinations (0 and 1) for a and b
    for (int a = 0; a <= 1; a++)
        for (int b = 0; b <= 1; b++)
            // Call the selected operation and print the result
            printf("%d %d | %d\n", a, b, operation(a, b));
}
