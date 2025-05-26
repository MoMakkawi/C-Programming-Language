#include <stdio.h>
#include <ctype.h> 
#include "logical_Wang.h"  // Include custom header for logical operator usage

#pragma region Exercise 1
// Exercise 1: Simple greeting function
static void Exercise1() {
    printf("%s: \n", __func__);  // Print function name using __func__
    printf("Hello Mohamad Faraj MAKKAWI!\n");  // Greet message
    printf("____________ \n");
}
#pragma endregion
#pragma region Exercise 2
// Exercise 2: Print sizes of various data types
static void Exercise2() {
    printf("%s: \n", __func__);
    // Using sizeof to show memory size in bytes for each type
    printf("Size of char: %zu bytes\n", sizeof(char));
    printf("Size of int: %zu bytes\n", sizeof(int));
    printf("Size of float: %zu bytes\n", sizeof(float));
    printf("Size of double: %zu bytes\n", sizeof(double));
    printf("Size of short: %zu bytes\n", sizeof(short));
    printf("Size of long: %zu bytes\n", sizeof(long));
    printf("Size of long long: %zu bytes\n", sizeof(long long));
    printf("Size of unsigned int: %zu bytes\n", sizeof(unsigned int));
    printf("Size of unsigned char: %zu bytes\n", sizeof(unsigned char));
    printf("Size of unsigned long: %zu bytes\n", sizeof(unsigned long));
    printf("Size of unsigned long long: %zu bytes\n", sizeof(unsigned long long));
    printf("____________ \n");
}
#pragma endregion
#pragma region Exercise 3
// Exercise 3: Demonstrate use of logical operators from external header
static void Exercise3() {
    printf("%s: \n", __func__);
    // Test logical operator function with different inputs
    logical_operator_use('A');  // Likely AND
    logical_operator_use('X');  // Likely XOR
    logical_operator_use('O');  // Likely OR
    printf("____________ \n");
}
#pragma endregion
#pragma region Exercise 4

// Print printable ASCII characters and their integer values
static void PrintAllChars()
{
    printf("Note: If you tried to print ]0, 254[ you will face an exception.");

    for (int i = 0; i <= 255; i++)
        if (isprint(i))  // Only print characters that are printable
            printf("Character: %c, Integer: %d\n", i, i);

    printf("____________ \n");
}

// Convert given USD amount to another currency
static void convertCurrency(float amount, float rate, char* currency) {
    printf("%.2f USD is %.2f %s\n", amount, amount * rate, currency);
}

// Perform currency conversion with optional custom exchange rates
static void performCurrencyConversion() {
    float usd_to_euro = 0.9;     // Default exchange rate
    float usd_to_pound = 0.75;   // Default exchange rate
    float amount;
    int choice;

    printf("Enter amount in USD: ");
    scanf("%f", &amount);

    // Ask user if they want to use custom exchange rates
    printf("Use predefined exchange rates? (1 for Yes, 0 for No): ");
    scanf("%d", &choice);

    if (choice == 0) {
        // Accept new exchange rates from user
        printf("Enter new exchange rate for USD to Euro: ");
        scanf("%f", &usd_to_euro);
        printf("Enter new exchange rate for USD to Pound: ");
        scanf("%f", &usd_to_pound);
    }

    // Perform and display conversions
    convertCurrency(amount, usd_to_euro, "Euros");
    convertCurrency(amount, usd_to_pound, "Pounds");
}

// Exercise 4: Character printing and currency conversion
static void Exercise4() {
    printf("____________ Exercise 4.1 ____________ \n");
    PrintAllChars();  // Display all printable ASCII characters
    printf("____________ Exercise 4.2____________ \n");
    performCurrencyConversion();  // Handle currency conversion
}
#pragma endregion

// Main function to run selected exercise
int main() {
    int choice;

    // Display exercise menu
    printf("Choose an exercise to run:\n");
    printf("1. Exercise 1\n");
    printf("2. Exercise 2\n");
    printf("3. Exercise 3\n");
    printf("4. Exercise 4\n");
    printf("Enter your choice (1-4): ");
    scanf("%d", &choice);

    // Call the corresponding exercise function
    switch (choice) {
    case 1:
        Exercise1();
        break;
    case 2:
        Exercise2();
        break;
    case 3:
        Exercise3();
        break;
    case 4:
        Exercise4();
        break;
    default:
        printf("Invalid choice.\n");  // Handle invalid input
        break;
    }

    return 0;
}
