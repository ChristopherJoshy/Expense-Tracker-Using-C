#include <stdio.h>
#include <stdbool.h>
#include <string.h>

struct Expense {
    char username[50];
    int year;
    int month;
    float amount;
};

const char* getMonthName(int month);

bool userExists(const char* username, const char* password) {
    FILE* usersFile = fopen("users.txt", "r");
    if (usersFile == NULL) {
        printf("Error opening users file.\n");
        return false;
    }

    char storedUsername[50];
    char storedPassword[50];
    while (fscanf(usersFile, "%s %s", storedUsername, storedPassword) == 2) {
        if (strcmp(username, storedUsername) == 0 && strcmp(password, storedPassword) == 0) {
            fclose(usersFile);
            return true;
        }
    }

    fclose(usersFile);
    return false;
}

void signUp() {
    char username[50];
    char password[50];

    printf("Enter a username: ");
    scanf("%s", username);
    printf("Enter a password: ");
    scanf("%s", password);

    FILE* usersFile = fopen("users.txt", "a");
    if (usersFile == NULL) {
        printf("Error opening users file.\n");
        return;
    }

    fprintf(usersFile, "%s %s\n", username, password);
    fclose(usersFile);
    printf("Account created successfully!\n");
}

bool login(char* username) {
    char password[50];

    printf("Enter your username: ");
    scanf("%s", username);
    printf("Enter your password: ");
    scanf("%s", password);

    if (userExists(username, password)) {
        printf("Login successful!\n");
        return true;
    } else {
        printf("Invalid username or password. Please try again.\n");
        return false;
    }
}

void addExpense(FILE* file, const char* username) {
    struct Expense expense;
    strcpy(expense.username, username);
    printf("Enter year (YYYY): ");
    scanf("%d", &expense.year);
    printf("Enter month (1-12): ");
    scanf("%d", &expense.month);
    printf("Enter expense amount: ");
    scanf("%f", &expense.amount);
    fprintf(file, "%s %d %d %.2f\n", expense.username, expense.year, expense.month, expense.amount);
    fflush(file); // Ensure data is written immediately
}

void listExpenses(FILE* file, const char* username) {
    struct Expense expense;
    int year;
    bool found = false;

    printf("Enter the year to list expenses: ");
    scanf("%d", &year);

    rewind(file);
    printf("Expenses for %s in %d:\n", username, year);
    while (fscanf(file, "%s %d %d %f", expense.username, &expense.year, &expense.month, &expense.amount) == 4) {
        if (strcmp(expense.username, username) == 0 && expense.year == year) {
            printf("Month: %d, Amount: $%.2f\n", expense.month, expense.amount);
            found = true;
        }
    }

    if (!found) {
        printf("No expenses found for the year %d.\n", year);
    }
}

void displayYearlyTotal(FILE* file, const char* username, int year) {
    float total = 0;
    struct Expense expense;

    rewind(file);
    while (fscanf(file, "%s %d %d %f", expense.username, &expense.year, &expense.month, &expense.amount) == 4) {
        if (strcmp(expense.username, username) == 0 && expense.year == year) {
            total += expense.amount;
        }
    }

    printf("Total expenses for %d: $%.2f\n", year, total);
}

void displayMonthWithMostExpenses(FILE* file, const char* username, int year) {
    float monthlyTotal[13] = {0};
    struct Expense expense;

    rewind(file);
    while (fscanf(file, "%s %d %d %f", expense.username, &expense.year, &expense.month, &expense.amount) == 4) {
        if (strcmp(expense.username, username) == 0 && expense.year == year) {
            monthlyTotal[expense.month] += expense.amount;
        }
    }

    float maxExpense = 0;
    int maxMonth = 0;
    for (int month = 1; month <= 12; ++month) {
        if (monthlyTotal[month] > maxExpense) {
            maxExpense = monthlyTotal[month];
            maxMonth = month;
        }
    }

    printf("Month with highest expenses in %d: %s\n", year, getMonthName(maxMonth));
}

const char* getMonthName(int month) {
    static const char* monthNames[] = {
        "January", "February", "March", "April", "May", "June",
        "July", "August", "September", "October", "November", "December"
    };
    return monthNames[month - 1];
}

void editExpense(FILE* file, const char* username) {
    struct Expense expense;
    int year, month;
    float newAmount;
    bool found = false;

    printf("Enter year of expense to edit (YYYY): ");
    scanf("%d", &year);
    printf("Enter month of expense to edit (1-12): ");
    scanf("%d", &month);
    printf("Enter new amount: ");
    scanf("%f", &newAmount);

    FILE* tempFile = fopen("temp.txt", "w");
    if (tempFile == NULL) {
        printf("Error opening temporary file.\n");
        return;
    }

    rewind(file);
    while (fscanf(file, "%s %d %d %f", expense.username, &expense.year, &expense.month, &expense.amount) == 4) {
        if (strcmp(expense.username, username) == 0 && expense.year == year && expense.month == month) {
            expense.amount = newAmount;
            found = true;
        }
        fprintf(tempFile, "%s %d %d %.2f\n", expense.username, expense.year, expense.month, expense.amount);
    }

    fclose(file);
    fclose(tempFile);
    remove("expenses.txt");
    rename("temp.txt", "expenses.txt");
    file = fopen("expenses.txt", "a+");

    if (found) {
        printf("Expense updated successfully.\n");
    } else {
        printf("Expense not found for year %d, month %d.\n", year, month);
    }
}

void deleteExpense(FILE* file, const char* username) {
    struct Expense expense;
    int year, month;
    bool found = false;

    printf("Enter year of expense to delete (YYYY): ");
    scanf("%d", &year);
    printf("Enter month of expense to delete (1-12): ");
    scanf("%d", &month);

    FILE* tempFile = fopen("temp.txt", "w");
    if (tempFile == NULL) {
        printf("Error opening temporary file.\n");
        return;
    }

    rewind(file);
    while (fscanf(file, "%s %d %d %f", expense.username, &expense.year, &expense.month, &expense.amount) == 4) {
        if (!(strcmp(expense.username, username) == 0 && expense.year == year && expense.month == month)) {
            fprintf(tempFile, "%s %d %d %.2f\n", expense.username, expense.year, expense.month, expense.amount);
        } else {
            found = true;
        }
    }

    fclose(file);
    fclose(tempFile);
    remove("expenses.txt");
    rename("temp.txt", "expenses.txt");
    file = fopen("expenses.txt", "a+");

    if (found) {
        printf("Expense deleted successfully.\n");
    } else {
        printf("Expense not found for year %d, month %d.\n", year, month);
    }
}

void displayWelcomeMessage() {
    printf("\n🌟 Welcome to the Ultimate Expense Tracker! 🌟\n");
    printf("Track your expenses like a pro!\n");
}

int main() {
    FILE* expenseFile = fopen("expenses.txt", "a+");

    if (expenseFile == NULL) {
        printf("Error opening file.\n");
        return 1;
    }

    displayWelcomeMessage();

    int choice;
    bool loggedIn = false;
    char loggedInUser[50];

    do {
        printf("\nExpense Tracker Menu:\n");
        printf("1. Sign Up\n");
        printf("2. Log In\n");
        printf("3. Add Expense\n");
        printf("4. List All Expenses\n");
        printf("5. Display Yearly Total\n");
        printf("6. Display Month with Most Expenses\n");
        printf("7. Edit Expense\n");
        printf("8. Delete Expense\n");
        printf("9. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                signUp();
                break;
            case 2:
                loggedIn = login(loggedInUser);
                break;
            case 3:
                if (loggedIn) {
                    addExpense(expenseFile, loggedInUser);
                } else {
                    printf("Please log in first.\n");
                }
                break;
            case 4:
                if (loggedIn) {
                    listExpenses(expenseFile, loggedInUser);
                } else {
                    printf("Please log in first.\n");
                }
                break;
            case 5:
                if (loggedIn) {
                    int year;
                    printf("Enter the year: ");
                    scanf("%d", &year);
                    displayYearlyTotal(expenseFile, loggedInUser, year);
                } else {
                    printf("Please log in first.\n");
                }
                break;
            case 6:
                if (loggedIn) {
                    int year;
                    printf("Enter the year: ");
                    scanf("%d", &year);
                    displayMonthWithMostExpenses(expenseFile, loggedInUser, year);
                } else {
                    printf("Please log in first.\n");
                }
                break;
            case 7:
                if (loggedIn) {
                    editExpense(expenseFile, loggedInUser);
                } else {
                    printf("Please log in first.\n");
                }
                break;
            case 8:
                if (loggedIn) {
                    deleteExpense(expenseFile, loggedInUser);
                } else {
                    printf("Please log in first.\n");
                }
                break;
            case 9:
                printf("Exiting...\n");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 9);

    fclose(expenseFile);
    return 0;
}
