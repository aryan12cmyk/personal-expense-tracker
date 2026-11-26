#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define MAX_USERS 3
#define MAX_T 600
#define MAX_I 120
#define MAX_C 30
#define MAX_S 30
#define MAX_NAME 50
#define CAT_COUNT 8
#define BANK_COUNT 4
#define BAR_SIZE 20

#define GREEN   "\033[0;32m"
#define YELLOW  "\033[0;33m"
#define RED     "\033[0;31m"
#define CYAN    "\033[0;36m"
#define MAGENTA "\033[0;35m"
#define BOLD    "\033[1m"
#define RESET   "\033[0m"

void categorizeMerchant(char merchant[], char category[]);
void saveUsers(void);

/* =========================================================
   STRUCTURES
   ========================================================= */

typedef struct {
    int id;
    char merchant[MAX_NAME];
    float amount;
    char category[20], mode[15], bank[25];
    int day, month, year;
} Transaction;

typedef struct {
    int id;
    char source[MAX_NAME];
    float amount;
    int day, month, year;
} Income;

typedef struct {
    int id;
    char name[MAX_NAME];
    float amount;
    int daysLeft, priority, paid;
} Commitment;

typedef struct {
    int id;
    char name[MAX_NAME];
    float amount;
    int frequency;              /* 1 Monthly, 2 Yearly */
    int daysUntilDue;
    char category[20];
    int active;
} Subscription;

typedef struct {
    char name[MAX_NAME], password[30];

    float monthlyAllowance;

    float monthlyGoal, yearlyGoal;
    float monthlySaved, yearlySaved;

    float emergencyFund, emergencyTarget;

    float budgets[CAT_COUNT];

    int accountNumbers[BANK_COUNT];
    int primaryBank;

    Transaction transactions[MAX_T];
    int transactionCount;

    Income incomes[MAX_I];
    int incomeCount;

    Commitment commitments[MAX_C];
    int commitmentCount;

    Subscription subscriptions[MAX_S];
    int subscriptionCount;
} User;

/* =========================================================
   GLOBAL DATA
   ========================================================= */

User users[MAX_USERS];
int currentUser = -1;

char categories[CAT_COUNT][20] = {
    "Food", "Groceries", "Transport", "Shopping",
    "Bills", "Entertainment", "Education", "Health"
};

char banks[BANK_COUNT][25] = {
    "HDFC Bank", "SBI", "Bank of India", "Axis Bank"
};

/* =========================================================
   BASIC INTERFACE
   ========================================================= */

void printLine() {
    int i;
    for (i = 0; i < 64; i++) printf("=");
    printf("\n");
}

void printSmallLine() {
    int i;
    for (i = 0; i < 64; i++) printf("-");
    printf("\n");
}

void clearScreen() {
    printf("\033[2J\033[H");
    fflush(stdout);
}

void delayMs(int milliseconds) {
    clock_t start = clock();

    while ((clock() - start) * 1000 / CLOCKS_PER_SEC < milliseconds) {
        /* Wait */
    }
}

void showLogo() {
    printf(CYAN BOLD);
    printLine();
    printf("\n");
    printf("                           ₹\n");
    printf("                     H I S A A B\n");
    printf("              PERSONAL FINANCE GUARDIAN\n\n");
    printf(RESET);
    printf("              Protect today. Plan tomorrow.\n");
    printLine();
}

/* =========================================================
   ORIGINAL STARTUP ANIMATION
   ========================================================= */

void startupAnimation() {
    int i, j;

    /* Cinematic 5-second HISAAB opening */
    for (i = 0; i < 16; i++) {
        clearScreen();
        printf("\n\n");

        if (i < 4) {
            printf("\n\n");
            printf("                 %*s" GREEN BOLD "₹" RESET "\n", i * 4, "");
            printf("                         " CYAN "·" RESET "\n");
            printf("                 %*s" CYAN BOLD "₹" RESET "\n", 24 - i * 4, "");
            printf("\n                 Building your money space...");
        }
        else if (i < 8) {
            printf("\n                 " GREEN "₹" RESET "      " CYAN "₹" RESET "      " GREEN "₹" RESET "\n\n");
            if (i == 4) printf(CYAN BOLD "                           H\n" RESET);
            if (i == 5) printf(CYAN BOLD "                        H I S\n" RESET);
            if (i == 6) printf(CYAN BOLD "                     H I S A A\n" RESET);
            if (i >= 7) printf(CYAN BOLD "                   H I S A A B\n" RESET);
            printf("\n                    Initializing core...");
        }
        else {
            printLine();
            printf(CYAN BOLD "\n                           ₹\n");
            printf("                     H I S A A B\n" RESET);
            printf("              PERSONAL FINANCE GUARDIAN\n\n");
            printf(GREEN BOLD "                Protect • Plan • Progress\n" RESET);
            printLine();
            printf("\n                 INITIALIZING SYSTEM\n\n                 [");
            for (j = 0; j < 20; j++)
                printf(j < (i - 7) * 2 ? GREEN "#" RESET : "-");
            printf("]\n\n");
            if (i >= 11) printf("                 Loading profiles       " GREEN "✓\n" RESET);
            if (i >= 12) printf("                 Preparing money engine  " GREEN "✓\n" RESET);
            if (i >= 13) printf("                 Securing local records  " GREEN "✓\n" RESET);
            if (i >= 14) printf("                 Starting HISAAB         " GREEN "✓\n" RESET);
            if (i >= 15) printf(GREEN BOLD "\n                         READY ✓\n" RESET);
        }

        fflush(stdout);
        delayMs(280);
    }

    delayMs(550);
    clearScreen();
}

void loadingScreen(char message[]) {
    int i, j, phase;

    /* General transition: about 2.4 seconds */
    for (i = 0; i < 10; i++) {
        clearScreen();
        printf("\n");
        printLine();
        printf(CYAN BOLD "                         ₹ HISAAB\n" RESET);
        printLine();
        printf("\n\n");

        phase = i % 4;
        if (phase == 0) printf("                           " GREEN BOLD "₹" RESET "\n");
        if (phase == 1) printf("                       " CYAN "◇" RESET "   " GREEN BOLD "₹" RESET "   " CYAN "◇" RESET "\n");
        if (phase == 2) printf("                    " GREEN "₹" RESET "   " CYAN "◇" RESET "   " GREEN BOLD "₹" RESET "   " CYAN "◇" RESET "   " GREEN "₹" RESET "\n");
        if (phase == 3) printf("                       " GREEN "◇" RESET "   " CYAN BOLD "₹" RESET "   " GREEN "◇" RESET "\n");

        printf("\n" BOLD "                    %s" RESET, message);
        for (j = 0; j < (i % 3) + 1; j++) printf(".");

        printf("\n\n                 ");
        for (j = 0; j < 10; j++)
            printf(j <= i ? GREEN "● " RESET : "○ ");

        printf("\n\n");
        printLine();
        fflush(stdout);
        delayMs(220);
    }
    delayMs(200);
    clearScreen();
}

void successAnimation(char title[], char subtitle[]) {
    int i;
    for (i = 0; i < 5; i++) {
        clearScreen();
        printf("\n\n");
        printLine();
        printf(CYAN BOLD "                         ₹ HISAAB\n" RESET);
        printLine();
        printf("\n\n");
        if (i == 0) printf("                           " CYAN "◇" RESET "\n");
        if (i == 1) printf("                           " GREEN BOLD "₹" RESET "\n");
        if (i >= 2) printf("                           " GREEN BOLD "✓" RESET "\n");
        if (i >= 3) printf(GREEN BOLD "\n                     %s\n" RESET, title);
        if (i >= 4) printf("                    %s\n", subtitle);
        printf("\n");
        printLine();
        fflush(stdout);
        delayMs(300);
    }
    delayMs(450);
    clearScreen();
}

void authenticationAnimation() {
    int i, j;
    char *checks[3] = {"USER ID", "PASSWORD", "FINANCIAL PROFILE"};

    for (i = 0; i < 9; i++) {
        clearScreen();
        printf("\n"); printLine();
        printf(CYAN BOLD "                       SECURE ACCESS\n" RESET);
        printLine();
        printf("\n                         [ " GREEN BOLD "₹" RESET " ]\n\n");
        printf(BOLD "                  Verifying identity" RESET);
        for (j = 0; j < (i % 3) + 1; j++) printf(".");
        printf("\n\n");
        for (j = 0; j < 3; j++) {
            int done = i >= (j + 1) * 2;
            printf("                  %-20s %s\n", checks[j], done ? GREEN "✓" RESET : YELLOW "○" RESET);
        }
        printf("\n                 Security pulse: ");
        for (j = 0; j < 8; j++) printf(j <= i % 8 ? CYAN "◆ " RESET : "◇ ");
        printf("\n"); printLine();
        fflush(stdout); delayMs(300);
    }
    successAnimation("ACCESS GRANTED", "Your profile is ready.");
}

void spendingAnimation() {
    int i, j, k;
    char *labels[4] = {"Food", "Shopping", "Transport", "Bills"};
    int target[4] = {9, 7, 5, 6};
    char *stage[3] = {"Reading this month's transactions", "Checking category budgets", "Finding spending patterns"};

    for (i = 0; i < 12; i++) {
        clearScreen();
        printf("\n"); printLine();
        printf(CYAN BOLD "                    SPENDING ANALYSIS\n" RESET);
        printLine();
        printf("\n                         " GREEN BOLD "₹" RESET "\n\n");
        printf(BOLD "              %s" RESET, stage[i / 4]);
        for (j = 0; j < (i % 3) + 1; j++) printf(".");
        printf("\n\n");

        for (j = 0; j < 4; j++) {
            int fill = target[j] * (i + 1) / 12;
            printf("%-14s [", labels[j]);
            for (k = 0; k < 12; k++) printf(k < fill ? GREEN "#" RESET : "-");
            printf("]\n");
        }
        printf("\n                 Analysing only CURRENT MONTH\n");
        fflush(stdout); delayMs(250);
    }
    successAnimation("ANALYSIS READY", "Your money, understood.");
}

void historyAnimation() {
    int i, j;
    for (i = 0; i < 12; i++) {
        clearScreen();
        printf("\n"); printLine();
        printf(CYAN BOLD "                       MONEY ARCHIVE\n" RESET);
        printLine();
        printf("\n                         " GREEN BOLD "₹" RESET "\n\n");
        printf(BOLD "                 Retrieving financial records" RESET);
        for (j = 0; j < (i % 3) + 1; j++) printf(".");
        printf("\n\n");
        printf("                2024  [%s%s] %s\n", GREEN, i >= 3 ? "##########" : "###-------", i >= 3 ? "✓" RESET : "" RESET);
        printf("                      |\n                      v\n");
        printf("                2025  [%s%s] %s\n", CYAN, i >= 6 ? "##########" : (i >= 3 ? "#####-----" : "----------"), i >= 6 ? "✓" RESET : "" RESET);
        printf("                      |\n                      v\n");
        printf("                2026  [%s%s] %s\n", GREEN, i >= 10 ? "##########" : (i >= 6 ? "######----" : "----------"), i >= 10 ? "✓" RESET : YELLOW "●" RESET);
        fflush(stdout); delayMs(270);
    }
    successAnimation("RECORDS READY", "2024 • 2025 • 2026 loaded.");
}

void upiAnimation() {
    int i, j;
    char *scan[4] = {"HDFC", "SBI", "BOI", "AXIS"};
    for (i = 0; i < 12; i++) {
        int active = i / 3;
        clearScreen();
        printf("\n"); printLine();
        printf(CYAN BOLD "                       UPI NETWORK\n" RESET);
        printLine();
        printf("\n                         " GREEN BOLD "₹" RESET "\n");
        printf("                   +-------------+\n");
        printf("              HDFC o             o SBI\n");
        printf("                   |      ₹      |\n");
        printf("               BOI o             o AXIS\n");
        printf("                   +-------------+\n\n");
        printf(BOLD "                 SCANNING LINKED BANKS\n\n" RESET);
        for (j = 0; j < 4; j++)
            printf("                  %-8s %s\n", scan[j], j < active ? GREEN "CONNECTED ✓" RESET : (j == active ? YELLOW "SCANNING ●" RESET : "WAITING ○"));
        printf("\n                 Encrypted demo channel ");
        for (j = 0; j < (i % 4) + 1; j++) printf(CYAN "•" RESET);
        printf("\n");
        fflush(stdout); delayMs(300);
    }
    successAnimation("BANKS SYNCED", "Searching for new UPI payments.");
}

void guardianAnimation() {
    int i, j;
    char *checks[5] = {"Available Balance", "Commitments", "Subscriptions", "Savings", "Emergency Fund"};
    for (i = 0; i < 10; i++) {
        clearScreen();
        printf("\n"); printLine();
        printf(MAGENTA BOLD "                    PURCHASE GUARDIAN\n" RESET);
        printLine();
        printf("\n                         " GREEN BOLD "₹" RESET "\n");
        printf("                         |\n                         v\n\n");
        printf(BOLD "                 CHECKING SAFE SPEND\n\n" RESET);
        for (j = 0; j < 5; j++) {
            int done = i >= (j + 1) * 2;
            printf("                  %-20s %s\n", checks[j], done ? GREEN "✓" RESET : YELLOW "[ checking ]" RESET);
        }
        fflush(stdout); delayMs(300);
    }
    successAnimation("CHECK COMPLETE", "Purchase impact calculated.");
}

void riskAnimation() {
    int i, j, score;
    for (i = 0; i <= 10; i++) {
        score = i * 10;
        clearScreen();
        printf("\n"); printLine();
        printf(CYAN BOLD "                  FINANCIAL HEALTH SCAN\n" RESET);
        printLine();
        printf("\n                         " GREEN BOLD "₹" RESET "\n\n");
        printf(BOLD "                     SCORE %3d / 100\n\n" RESET, score);
        printf("                 [");
        for (j = 0; j < 20; j++) printf(j < i * 2 ? GREEN "#" RESET : "-");
        printf("]\n\n");
        if (i < 3) printf("                 Reading spending behaviour...\n");
        else if (i < 6) printf("                 Checking commitments...\n");
        else if (i < 9) printf("                 Checking savings & emergency...\n");
        else printf("                 Finalising financial health...\n");
        fflush(stdout); delayMs(280);
    }
    successAnimation("SCORE READY", "Financial health calculated.");
}

void paymentAnimation() {
    int i;
    for (i = 0; i < 9; i++) {
        clearScreen();
        printf("\n"); printLine();
        printf(CYAN BOLD "                     RECORDING PAYMENT\n" RESET);
        printLine();
        printf("\n\n        ACCOUNT ");
        printf("%*s" GREEN BOLD "₹" RESET, 5 + i * 3, "");
        printf("%*s MERCHANT\n", 30 - i * 3, "");
        printf("\n                 Securing transaction record");
        printf(i % 3 == 0 ? "." : (i % 3 == 1 ? ".." : "..."));
        printf("\n");
        fflush(stdout); delayMs(250);
    }
    successAnimation("PAYMENT RECORDED", "Transaction saved locally.");
}

void logoutAnimation() {
    int i, j;
    char *steps[3] = {"Saving financial data", "Closing user profile", "Securing access"};
    for (i = 0; i < 9; i++) {
        clearScreen();
        printf("\n"); printLine();
        printf(CYAN BOLD "                       SECURE LOGOUT\n" RESET);
        printLine();
        printf("\n                         [" GREEN BOLD "₹" RESET "]\n\n");
        for (j = 0; j < 3; j++)
            printf("                  %-22s %s\n", steps[j], i >= (j + 1) * 2 ? GREEN "✓" RESET : "○");
        if (i >= 7) printf(GREEN BOLD "\n                         [LOCKED]\n" RESET);
        fflush(stdout); delayMs(280);
    }
    successAnimation("PROFILE SECURED", "Logged out safely.");
}

/* =========================================================
   PROGRESS BAR
   ========================================================= */

void showBar(float percentage, int reverse) {
    int i, filled;

    if (percentage < 0) percentage = 0;
    if (percentage > 100) percentage = 100;

    filled = (int)(percentage * BAR_SIZE / 100);

    if (!reverse) {
        if (percentage >= 80) printf(RED);
        else if (percentage >= 60) printf(YELLOW);
        else printf(GREEN);
    } else {
        if (percentage >= 80) printf(GREEN);
        else if (percentage >= 40) printf(YELLOW);
        else printf(RED);
    }

    printf("[");
    for (i = 0; i < BAR_SIZE; i++)
        printf(i < filled ? "#" : "-");

    printf("]" RESET " %.1f%%", percentage);
}

/* =========================================================
   DATE / STRING / IDs
   ========================================================= */

void getCurrentDate(int *d, int *m, int *y) {
    time_t now = time(NULL);
    struct tm *date = localtime(&now);

    *d = date->tm_mday;
    *m = date->tm_mon + 1;
    *y = date->tm_year + 1900;
}

void convertToUpper(char text[]) {
    int i;
    for (i = 0; text[i]; i++)
        text[i] = toupper((unsigned char)text[i]);
}

int generateTransactionID() {
    return 100000 + rand() % 900000;
}

int generateSmallID() {
    return 1000 + rand() % 9000;
}

/* =========================================================
   INITIAL USERS
   ========================================================= */

void initializeUsers() {
    int i;

    float defaultBudgets[CAT_COUNT] = {
        3000, 1500, 1500, 1200, 1000, 800, 1000, 700
    };

    memset(users, 0, sizeof(users));

    strcpy(users[0].name, "Amber Kant");
    strcpy(users[0].password, "Amber1804");
    users[0].monthlyAllowance = 15000;
    users[0].monthlyGoal = 3000;
    users[0].yearlyGoal = 40000;
    users[0].monthlySaved = 1500;
    users[0].yearlySaved = 15000;
    users[0].emergencyTarget = 15000;
    users[0].emergencyFund = 3000;
    users[0].primaryBank = 0;
    users[0].accountNumbers[0] = 4821;
    users[0].accountNumbers[1] = 7314;
    users[0].accountNumbers[2] = 2956;
    users[0].accountNumbers[3] = 8642;

    strcpy(users[1].name, "Aryan Pandey");
    strcpy(users[1].password, "Aryan1234");
    users[1].monthlyAllowance = 12000;
    users[1].monthlyGoal = 2500;
    users[1].yearlyGoal = 30000;
    users[1].monthlySaved = 1000;
    users[1].yearlySaved = 11000;
    users[1].emergencyTarget = 12000;
    users[1].emergencyFund = 2500;
    users[1].primaryBank = 1;
    users[1].accountNumbers[0] = 5612;
    users[1].accountNumbers[1] = 9087;
    users[1].accountNumbers[2] = 3421;
    users[1].accountNumbers[3] = 6754;

    strcpy(users[2].name, "Yash Sharma");
    strcpy(users[2].password, "Yash8790");
    users[2].monthlyAllowance = 10000;
    users[2].monthlyGoal = 2000;
    users[2].yearlyGoal = 25000;
    users[2].monthlySaved = 800;
    users[2].yearlySaved = 7500;
    users[2].emergencyTarget = 10000;
    users[2].emergencyFund = 2000;
    users[2].primaryBank = 2;
    users[2].accountNumbers[0] = 1935;
    users[2].accountNumbers[1] = 4268;
    users[2].accountNumbers[2] = 7531;
    users[2].accountNumbers[3] = 8146;

    for (i = 0; i < MAX_USERS; i++)
        memcpy(users[i].budgets, defaultBudgets, sizeof(defaultBudgets));
}

/* =========================================================
   REALISTIC DEMO HISTORY
   ========================================================= */

void addDemoTransaction(int userIndex, char merchant[], float amount,
                        char mode[], char bank[], int day, int month, int year) {
    User *u = &users[userIndex];
    Transaction *t;

    if (u->transactionCount >= MAX_T) return;

    t = &u->transactions[u->transactionCount++];
    t->id = 100000 + rand() % 900000;
    strcpy(t->merchant, merchant);
    t->amount = amount;
    categorizeMerchant(merchant, t->category);
    strcpy(t->mode, mode);
    strcpy(t->bank, bank);
    t->day = day;
    t->month = month;
    t->year = year;
}

void addDemoIncome(int userIndex, char source[], float amount,
                   int day, int month, int year) {
    User *u = &users[userIndex];
    Income *in;

    if (u->incomeCount >= MAX_I) return;

    in = &u->incomes[u->incomeCount++];
    in->id = 1000 + rand() % 9000;
    strcpy(in->source, source);
    in->amount = amount;
    in->day = day;
    in->month = month;
    in->year = year;
}

void loadDemoHistory() {
    int u, year, month, k;
    int currentDay, currentMonth, currentYear;

    char foodPlaces[6][MAX_NAME] = {
        "ZOMATO", "SWIGGY", "CAMPUS MESS", "CAFE", "DOMINOS", "CANTEEN"
    };

    char groceryPlaces[4][MAX_NAME] = {
        "BLINKIT", "ZEPTO", "DMART GROCERY", "LOCAL GROCERY"
    };

    char transportPlaces[5][MAX_NAME] = {
        "RAPIDO", "UBER", "OLA", "METRO", "PETROL"
    };

    char shoppingPlaces[5][MAX_NAME] = {
        "AMAZON", "FLIPKART", "MYNTRA", "AJIO", "LOCAL STORE"
    };

    char extraPlaces[5][MAX_NAME] = {
        "UPES BOOK STORE", "PHARMACY", "PVR", "JIO RECHARGE", "STATIONERY"
    };

    float baseIncome[3] = {15000, 12000, 10000};
    float normalMin[3] = {8500, 6500, 5500};
    float normalMax[3] = {11500, 9000, 7500};
    float hardMax[3] = {12500, 10000, 8500};

    getCurrentDate(&currentDay, &currentMonth, &currentYear);

    /*
       Build a realistic permanent track record.
       Every month is generated from its income first.
       Spending is then limited to a believable target and can never
       cross the month's safe hard limit.
    */

    for (u = 0; u < MAX_USERS; u++) {
        for (year = 2024; year <= currentYear; year++) {
            int lastMonth = 12;

            if (year == currentYear)
                lastMonth = currentMonth;

            for (month = 1; month <= lastMonth; month++) {
                float allowance;
                float extraIncome = 0;
                float totalIncome;
                float monthSpent = 0;
                float targetSpent;
                float monthHardMax;
                int extraIncomeChance;
                int expenseCount;

                allowance = baseIncome[u];

                if (year == 2024)
                    allowance *= 0.88f;
                else if (year == 2025)
                    allowance *= 0.94f;

                /* Small natural variation, but never a wild change. */
                allowance += (float)(rand() % 301 - 150);

                addDemoIncome(u, "Monthly Allowance", allowance,
                              1 + rand() % 3, month, year);

                extraIncomeChance = rand() % 100;

                if (extraIncomeChance < 24) {
                    char source[MAX_NAME];

                    if (u == 0) {
                        strcpy(source, "Freelance Work");
                        extraIncome = 800 + rand() % 1701;
                    }
                    else if (u == 1) {
                        strcpy(source, "Project Work");
                        extraIncome = 600 + rand() % 1301;
                    }
                    else {
                        strcpy(source, "Event / Extra Work");
                        extraIncome = 400 + rand() % 1001;
                    }

                    addDemoIncome(u, source, extraIncome,
                                  10 + rand() % 12, month, year);
                }

                totalIncome = allowance + extraIncome;

                /*
                   Normal monthly target:
                   Amber : 8500 - 11500
                   Aryan : 6500 - 9000
                   Yash  : 5500 - 7500

                   Older years scale slightly with their lower allowance.
                */
                targetSpent = normalMin[u] +
                    rand() % ((int)(normalMax[u] - normalMin[u]) + 1);

                if (year == 2024)
                    targetSpent *= 0.88f;
                else if (year == 2025)
                    targetSpent *= 0.94f;

                /* Extra income permits a little more spending, not all of it. */
                targetSpent += extraIncome * 0.25f;

                monthHardMax = hardMax[u];

                if (year == 2024)
                    monthHardMax *= 0.88f;
                else if (year == 2025)
                    monthHardMax *= 0.94f;

                monthHardMax += extraIncome * 0.35f;

                /* Always leave at least 18% of actual money in the month. */
                if (monthHardMax > totalIncome * 0.82f)
                    monthHardMax = totalIncome * 0.82f;

                if (targetSpent > monthHardMax)
                    targetSpent = monthHardMax;

                /* 8 to 10 believable purchases in each month. */
                expenseCount = 8 + rand() % 3;

                for (k = 0; k < expenseCount && monthSpent < targetSpent; k++) {
                    char merchant[MAX_NAME];
                    char mode[15];
                    char bank[25];
                    float amount;
                    float remaining = targetSpent - monthSpent;
                    int day = 2 + rand() % 26;
                    int type = rand() % 5;

                    if (type == 0) {
                        strcpy(merchant, foodPlaces[rand() % 6]);
                        amount = 180 + rand() % 721;
                    }
                    else if (type == 1) {
                        strcpy(merchant, groceryPlaces[rand() % 4]);
                        amount = 450 + rand() % 1151;
                    }
                    else if (type == 2) {
                        strcpy(merchant, transportPlaces[rand() % 5]);
                        amount = 100 + rand() % 601;
                    }
                    else if (type == 3) {
                        strcpy(merchant, shoppingPlaces[rand() % 5]);
                        amount = 500 + rand() % 1801;
                    }
                    else {
                        strcpy(merchant, extraPlaces[rand() % 5]);
                        amount = 250 + rand() % 1051;
                    }

                    if (u == 0)
                        amount *= 1.05f;
                    else if (u == 2)
                        amount *= 0.92f;

                    /* Never let a transaction push the month above its target. */
                    if (amount > remaining)
                        amount = remaining;

                    if (amount < 80)
                        break;

                    if (rand() % 100 < 86) {
                        strcpy(mode, "UPI");
                        strcpy(bank, banks[(u + month + k) % BANK_COUNT]);
                    }
                    else {
                        strcpy(mode, "Cash");
                        strcpy(bank, "No Bank");
                    }

                    addDemoTransaction(u, merchant, amount,
                                       mode, bank, day, month, year);

                    monthSpent += amount;
                }

                /*
                   If normal purchases did not reach a realistic monthly level,
                   add one ordinary household/college expense for the remainder.
                */
                if (targetSpent - monthSpent >= 100) {
                    float remainder = targetSpent - monthSpent;

                    addDemoTransaction(u, "DMART GROCERY", remainder,
                                       "UPI", banks[(u + month) % BANK_COUNT],
                                       24 + rand() % 4, month, year);

                    monthSpent += remainder;
                }
            }
        }
    }
}

/* =========================================================
   HISTORY DATABASE CHECK
   Adds the realistic demo history only when it is missing.
   This also upgrades an older users.dat file automatically.
   ========================================================= */

int demoHistoryExists() {
    int u, i;

    for (u = 0; u < MAX_USERS; u++) {
        int has2024Income = 0, has2024Expense = 0;
        int has2025Income = 0, has2025Expense = 0;
        int has2026Income = 0, has2026Expense = 0;

        for (i = 0; i < users[u].incomeCount; i++) {
            int y = users[u].incomes[i].year;
            if (y == 2024) has2024Income = 1;
            if (y == 2025) has2025Income = 1;
            if (y == 2026) has2026Income = 1;
        }

        for (i = 0; i < users[u].transactionCount; i++) {
            int y = users[u].transactions[i].year;
            if (y == 2024) has2024Expense = 1;
            if (y == 2025) has2025Expense = 1;
            if (y == 2026) has2026Expense = 1;
        }

        if (!has2024Income || !has2024Expense ||
            !has2025Income || !has2025Expense ||
            !has2026Income || !has2026Expense)
            return 0;
    }

    return 1;
}

int historyWithinLimits() {
    int u, year, month, i;
    float hardMax[3] = {12500, 10000, 8500};

    for (u = 0; u < MAX_USERS; u++) {
        for (year = 2024; year <= 2026; year++) {
            for (month = 1; month <= 12; month++) {
                float income = 0;
                float spent = 0;
                float extra = 0;
                float limit;

                for (i = 0; i < users[u].incomeCount; i++) {
                    Income *in = &users[u].incomes[i];

                    if (in->month == month && in->year == year) {
                        income += in->amount;

                        if (strcmp(in->source, "Monthly Allowance") != 0)
                            extra += in->amount;
                    }
                }

                for (i = 0; i < users[u].transactionCount; i++) {
                    Transaction *t = &users[u].transactions[i];

                    if (t->month == month && t->year == year)
                        spent += t->amount;
                }

                if (income <= 0 && spent <= 0)
                    continue;

                limit = hardMax[u];

                if (year == 2024)
                    limit *= 0.88f;
                else if (year == 2025)
                    limit *= 0.94f;

                limit += extra * 0.35f;

                if (limit > income * 0.82f)
                    limit = income * 0.82f;

                /* Small tolerance only for floating point rounding. */
                if (spent > limit + 2.0f)
                    return 0;
            }
        }
    }

    return 1;
}

void upgradeHistoryDatabase() {
    if (!demoHistoryExists() || !historyWithinLimits()) {
        int u;

        /* Rebuild only the generated money history using the corrected rules. */
        for (u = 0; u < MAX_USERS; u++) {
            users[u].transactionCount = 0;
            users[u].incomeCount = 0;
        }

        loadDemoHistory();
        saveUsers();
    }
}

/* =========================================================
   PERMANENT STORAGE
   ========================================================= */

void saveUsers() {
    FILE *fp = fopen("users.dat", "wb");

    if (!fp) {
        printf(RED "Warning: Could not save financial data.\n" RESET);
        return;
    }

    fwrite(users, sizeof(User), MAX_USERS, fp);
    fclose(fp);
}

int loadUsers() {
    FILE *fp = fopen("users.dat", "rb");
    size_t loaded;

    if (!fp) return 0;

    loaded = fread(users, sizeof(User), MAX_USERS, fp);
    fclose(fp);

    return loaded == MAX_USERS;
}

/* =========================================================
   DETAILED ORIGINAL LOGIN
   ========================================================= */

int login() {
    char name[MAX_NAME], password[30];
    int i, found = -1, attempts = 3;

    clearScreen();

    printf(CYAN BOLD);
    printLine();
    printf("\n");
    printf("                           ₹\n");
    printf("                     H I S A A B\n\n");
    printf("                 PERSONAL FINANCE GUARDIAN\n\n");
    printLine();
    printf(RESET);

    printf("\n" BOLD "                      SECURE LOGIN\n" RESET);
    printf("\n              Your money. Your privacy.\n\n");
    printSmallLine();

    printf("\n" CYAN "  USER LOGIN\n" RESET);
    printf("\n  Full Name     : ");
    scanf(" %49[^\n]", name);

    for (i = 0; i < MAX_USERS; i++) {
        if (strcmp(name, users[i].name) == 0) {
            found = i;
            break;
        }
    }

    if (found == -1) {
        loadingScreen("Checking user");

        printLine();
        printf(RED BOLD "\n                     ACCESS DENIED\n\n" RESET);
        printf("                User not recognised.\n\n");
        printLine();

        delayMs(900);
        return 0;
    }

    printf("\n" GREEN "  ✓ Account Found\n" RESET);
    printf("  User          : " BOLD "%s\n" RESET, users[found].name);
    printf("\n");

    while (attempts > 0) {
        printf("  Password      : ");
        scanf("%29s", password);

        authenticationAnimation();

        if (strcmp(password, users[found].password) == 0) {
            currentUser = found;

            clearScreen();
            printf("\n");
            printLine();

            printf("\n" GREEN BOLD);
            printf("                           ✓\n\n");
            printf("                     ACCESS GRANTED\n");
            printf(RESET);

            printf("\n" CYAN BOLD);
            printf("                    Welcome, %s\n", users[currentUser].name);
            printf(RESET "\n");

            printLine();

            delayMs(650);
            loadingScreen("Loading your financial profile");

            return 1;
        }

        attempts--;

        clearScreen();
        printLine();

        printf("\n" RED BOLD "                   INCORRECT PASSWORD\n" RESET);
        printf("\n                 Attempts Remaining: %d\n\n", attempts);

        printLine();

        if (attempts > 0)
            printf("\n  User          : %s\n\n", users[found].name);
    }

    clearScreen();
    printLine();

    printf("\n" RED BOLD);
    printf("                     ACCESS DENIED\n\n");
    printf("                     HISAAB LOCKED\n");
    printf(RESET);

    printf("\n             Too many incorrect attempts.\n\n");
    printLine();

    delayMs(1000);

    return 0;
}

/* =========================================================
   AUTOMATIC CATEGORISATION
   ========================================================= */

void categorizeMerchant(char merchant[], char category[]) {
    char temp[MAX_NAME];

    strcpy(temp, merchant);
    convertToUpper(temp);

    if (strstr(temp, "ZOMATO") || strstr(temp, "SWIGGY") ||
        strstr(temp, "CAFE") || strstr(temp, "MESS") ||
        strstr(temp, "KFC") || strstr(temp, "DOMINOS") ||
        strstr(temp, "CANTEEN"))
        strcpy(category, "Food");

    else if (strstr(temp, "BLINKIT") || strstr(temp, "ZEPTO") ||
             strstr(temp, "DMART") || strstr(temp, "GROCERY"))
        strcpy(category, "Groceries");

    else if (strstr(temp, "UBER") || strstr(temp, "OLA") ||
             strstr(temp, "RAPIDO") || strstr(temp, "METRO") ||
             strstr(temp, "PETROL"))
        strcpy(category, "Transport");

    else if (strstr(temp, "JIO") || strstr(temp, "AIRTEL") ||
             strstr(temp, "WIFI") || strstr(temp, "RECHARGE") ||
             strstr(temp, "ELECTRICITY"))
        strcpy(category, "Bills");

    else if (strstr(temp, "NETFLIX") || strstr(temp, "SPOTIFY") ||
             strstr(temp, "PVR") || strstr(temp, "MOVIE"))
        strcpy(category, "Entertainment");

    else if (strstr(temp, "UPES") || strstr(temp, "BOOK") ||
             strstr(temp, "COLLEGE") || strstr(temp, "STATIONERY"))
        strcpy(category, "Education");

    else if (strstr(temp, "PHARMACY") || strstr(temp, "DOCTOR") ||
             strstr(temp, "HOSPITAL") || strstr(temp, "MEDICAL"))
        strcpy(category, "Health");

    else
        strcpy(category, "Shopping");
}

/* Current-month helpers are declared here because they are used
   by the dashboard calculations below. */
float calculateMonthSpent(int month, int year);
float calculateMonthIncome(int month, int year);
float calculateCurrentMonthIncome(void);
float calculateCurrentMonthSpent(void);
float calculateCurrentMonthCategory(char category[]);
int countCurrentMonthCategoryTransactions(char category[]);

/* =========================================================
   CORE CALCULATIONS
   ========================================================= */

float calculateTotalSpent() {
    int i;
    float total = 0;

    for (i = 0; i < users[currentUser].transactionCount; i++)
        total += users[currentUser].transactions[i].amount;

    return total;
}

float calculateTotalIncome() {
    int i;
    float total = 0;

    for (i = 0; i < users[currentUser].incomeCount; i++)
        total += users[currentUser].incomes[i].amount;

    return total;
}

float calculateCategorySpent(char category[]) {
    int i;
    float total = 0;

    for (i = 0; i < users[currentUser].transactionCount; i++)
        if (strcmp(users[currentUser].transactions[i].category, category) == 0)
            total += users[currentUser].transactions[i].amount;

    return total;
}

int countCategoryTransactions(char category[]) {
    int i, count = 0;

    for (i = 0; i < users[currentUser].transactionCount; i++)
        if (strcmp(users[currentUser].transactions[i].category, category) == 0)
            count++;

    return count;
}

float calculateCommitments() {
    int i;
    float total = 0;

    for (i = 0; i < users[currentUser].commitmentCount; i++)
        if (!users[currentUser].commitments[i].paid)
            total += users[currentUser].commitments[i].amount;

    return total;
}

float calculateSubscriptions() {
    int i;
    float total = 0;

    for (i = 0; i < users[currentUser].subscriptionCount; i++) {
        Subscription *s = &users[currentUser].subscriptions[i];

        if (s->active)
            total += s->frequency == 1 ? s->amount : s->amount / 12.0f;
    }

    return total;
}

float calculateYearlySubscriptions() {
    int i;
    float total = 0;

    for (i = 0; i < users[currentUser].subscriptionCount; i++) {
        Subscription *s = &users[currentUser].subscriptions[i];

        if (s->active)
            total += s->frequency == 1 ? s->amount * 12 : s->amount;
    }

    return total;
}

float savingsStillNeeded() {
    float needed = users[currentUser].monthlyGoal -
                   users[currentUser].monthlySaved;

    return needed > 0 ? needed : 0;
}

float calculateAvailableMoney() {
    float income = calculateCurrentMonthIncome();

    if (income <= 0)
        income = users[currentUser].monthlyAllowance;

    return income - calculateCurrentMonthSpent();
}

float calculateSafeMoney() {
    float safe = calculateAvailableMoney()
                 - calculateCommitments()
                 - calculateSubscriptions()
                 - savingsStillNeeded()
                 - users[currentUser].emergencyFund;

    return safe > 0 ? safe : 0;
}

/* =========================================================
   MONTHLY CALCULATIONS
   ========================================================= */

float calculateMonthSpent(int month, int year) {
    int i;
    float total = 0;

    for (i = 0; i < users[currentUser].transactionCount; i++) {
        Transaction *t = &users[currentUser].transactions[i];

        if (t->month == month && t->year == year)
            total += t->amount;
    }

    return total;
}

float calculateMonthIncome(int month, int year) {
    int i;
    float total = 0;

    for (i = 0; i < users[currentUser].incomeCount; i++) {
        Income *in = &users[currentUser].incomes[i];

        if (in->month == month && in->year == year)
            total += in->amount;
    }

    return total;
}

float calculateMonthCategory(char category[], int month, int year) {
    int i;
    float total = 0;

    for (i = 0; i < users[currentUser].transactionCount; i++) {
        Transaction *t = &users[currentUser].transactions[i];

        if (t->month == month && t->year == year &&
            strcmp(t->category, category) == 0)
            total += t->amount;
    }

    return total;
}

float calculateCurrentMonthIncome() {
    int d, m, y;

    getCurrentDate(&d, &m, &y);
    return calculateMonthIncome(m, y);
}

float calculateCurrentMonthSpent() {
    int d, m, y;

    getCurrentDate(&d, &m, &y);
    return calculateMonthSpent(m, y);
}

float calculateCurrentMonthCategory(char category[]) {
    int d, m, y;

    getCurrentDate(&d, &m, &y);
    return calculateMonthCategory(category, m, y);
}

int countCurrentMonthCategoryTransactions(char category[]) {
    int i, d, m, y, count = 0;

    getCurrentDate(&d, &m, &y);

    for (i = 0; i < users[currentUser].transactionCount; i++) {
        Transaction *t = &users[currentUser].transactions[i];

        if (t->month == m && t->year == y &&
            strcmp(t->category, category) == 0)
            count++;
    }

    return count;
}

/* =========================================================
   RISK SCORE
   ========================================================= */

int calculateRiskScore() {
    float income = calculateCurrentMonthIncome();
    float percentage;
    int score = 100;

    if (income <= 0)
        income = users[currentUser].monthlyAllowance;

    if (income <= 0)
        return 0;

    percentage = calculateCurrentMonthSpent() / income * 100;

    if (percentage > 100) score -= 50;
    else if (percentage > 90) score -= 40;
    else if (percentage > 75) score -= 25;
    else if (percentage > 60) score -= 15;

    if (calculateCommitments() > income * 0.30f) score -= 15;
    if (calculateSubscriptions() > income * 0.15f) score -= 10;

    if (users[currentUser].monthlyGoal > 0 &&
        users[currentUser].monthlySaved <
        users[currentUser].monthlyGoal * 0.50f)
        score -= 10;

    if (users[currentUser].emergencyTarget > 0 &&
        users[currentUser].emergencyFund <
        users[currentUser].emergencyTarget * 0.25f)
        score -= 10;

    if (calculateSafeMoney() <= 0)
        score -= 20;

    if (score < 0) score = 0;
    if (score > 100) score = 100;

    return score;
}

/* =========================================================
   BUDGET ALERT
   ========================================================= */

void checkBudget(char category[]) {
    int i;
    float percentage;

    for (i = 0; i < CAT_COUNT; i++) {
        if (strcmp(categories[i], category) == 0) {
            if (users[currentUser].budgets[i] <= 0)
                return;

            percentage = calculateCurrentMonthCategory(category) /
                         users[currentUser].budgets[i] * 100;

            printf("\n%s Budget\n", category);
            showBar(percentage, 0);
            printf("\n");

            if (percentage >= 100)
                printf(RED BOLD "!! BUDGET EXCEEDED !!\n" RESET);
            else if (percentage >= 80)
                printf(RED "!! EARLY BUDGET ALERT !!\n" RESET);

            return;
        }
    }
}

/* =========================================================
   TRANSACTIONS
   ========================================================= */

void addTransaction(char merchant[], float amount,
                    char mode[], char bank[]) {
    int n = users[currentUser].transactionCount;
    Transaction *t;

    if (n >= MAX_T) {
        printf(RED "Transaction storage full.\n" RESET);
        return;
    }

    t = &users[currentUser].transactions[n];

    t->id = generateTransactionID();
    strcpy(t->merchant, merchant);
    t->amount = amount;

    categorizeMerchant(merchant, t->category);

    strcpy(t->mode, mode);
    strcpy(t->bank, bank);

    getCurrentDate(&t->day, &t->month, &t->year);

    users[currentUser].transactionCount++;
    saveUsers();

    printLine();
    printf(GREEN BOLD "                     PAYMENT RECORDED\n" RESET);
    printLine();

    printf("Transaction ID : %d\n", t->id);
    printf("Date           : %02d/%02d/%04d\n", t->day, t->month, t->year);
    printf("Merchant       : %s\n", t->merchant);
    printf("Amount         : Rs %.2f\n", t->amount);
    printf("Category       : %s\n", t->category);
    printf("Payment Mode   : %s\n", t->mode);
    printf("Bank           : %s\n", t->bank);

    printLine();
    checkBudget(t->category);
}

void addExtraPayment() {
    char merchant[MAX_NAME], mode[15], selectedBank[25];
    int choice, bankChoice;
    float amount;

    printLine();
    printf(CYAN BOLD "                    ADD EXTRA PAYMENT\n" RESET);
    printLine();

    printf("1. Cash Payment\n");
    printf("2. Missed UPI Payment\n");
    printf("3. Other Payment\n\n");

    printf("Payment Type: ");
    scanf("%d", &choice);

    if (choice == 1) {
        strcpy(mode, "Cash");
        strcpy(selectedBank, "No Bank");
    }
    else if (choice == 2) {
        strcpy(mode, "UPI");

        printf("\n1. HDFC Bank\n2. SBI\n3. Bank of India\n4. Axis Bank\n");
        printf("\nBank: ");
        scanf("%d", &bankChoice);

        if (bankChoice >= 1 && bankChoice <= BANK_COUNT)
            strcpy(selectedBank, banks[bankChoice - 1]);
        else
            strcpy(selectedBank, banks[users[currentUser].primaryBank]);
    }
    else {
        strcpy(mode, "Other");
        strcpy(selectedBank, "Manual");
    }

    printf("Merchant / Purpose: ");
    scanf(" %49[^\n]", merchant);

    do {
        printf("Amount: Rs ");
        scanf("%f", &amount);
    } while (amount <= 0);

    paymentAnimation();
    addTransaction(merchant, amount, mode, selectedBank);
}

void viewTransactions() {
    int i;

    printLine();
    printf(CYAN BOLD "                    TRANSACTION HISTORY\n" RESET);
    printLine();

    if (!users[currentUser].transactionCount) {
        printf("No transactions available.\n");
        return;
    }

    for (i = users[currentUser].transactionCount - 1; i >= 0; i--) {
        Transaction *t = &users[currentUser].transactions[i];

        printf("\n%02d/%02d/%04d | ID %d\n",
               t->day, t->month, t->year, t->id);

        printf(BOLD "%s\n" RESET, t->merchant);

        printf("Rs %.2f | %s | %s\n",
               t->amount, t->category, t->mode);

        printf("%s\n", t->bank);

        printSmallLine();
    }
}

/* =========================================================
   INCOME
   ========================================================= */

void addIncome() {
    int n = users[currentUser].incomeCount;
    Income *in;

    if (n >= MAX_I) {
        printf(RED "Income storage full.\n" RESET);
        return;
    }

    in = &users[currentUser].incomes[n];
    in->id = generateSmallID();

    printf("Income Source: ");
    scanf(" %49[^\n]", in->source);

    do {
        printf("Amount: Rs ");
        scanf("%f", &in->amount);
    } while (in->amount <= 0);

    getCurrentDate(&in->day, &in->month, &in->year);

    loadingScreen("Recording income");

    users[currentUser].incomeCount++;
    saveUsers();

    printf(GREEN "Income added successfully.\n" RESET);
}

void viewIncome() {
    int i;

    printLine();
    printf(CYAN BOLD "                      INCOME HISTORY\n" RESET);
    printLine();

    if (!users[currentUser].incomeCount) {
        printf("No income records available.\n");
        return;
    }

    for (i = users[currentUser].incomeCount - 1; i >= 0; i--) {
        Income *in = &users[currentUser].incomes[i];

        printf("%02d/%02d/%04d | %-20s | "
               GREEN "+Rs %.2f" RESET " | ID %d\n",
               in->day, in->month, in->year,
               in->source, in->amount, in->id);
    }
}

void cashFlowSummary() {
    float income = calculateCurrentMonthIncome();
    float expenses = calculateCurrentMonthSpent();

    if (income <= 0)
        income = users[currentUser].monthlyAllowance;

    printLine();
    printf(CYAN BOLD "                       CASH FLOW\n" RESET);
    printLine();

    printf("Money In       : Rs %.2f\n", income);
    printf("Money Out      : Rs %.2f\n", expenses);
    printf("Net Cash Flow  : Rs %.2f\n", income - expenses);
    printf("Commitments    : Rs %.2f\n", calculateCommitments());
    printf("Subscriptions  : Rs %.2f/month\n", calculateSubscriptions());
    printf("Emergency Fund : Rs %.2f\n", users[currentUser].emergencyFund);
    printf(BOLD "Safe Money     : Rs %.2f\n" RESET, calculateSafeMoney());

    printLine();
}

void incomeMenu() {
    int choice;

    do {
        printLine();
        printf(CYAN BOLD "                     INCOME MANAGER\n" RESET);
        printLine();

        printf("1. Add Income\n");
        printf("2. View Income\n");
        printf("3. Cash Flow Summary\n");
        printf("4. Back\n\n");

        printf("Choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                loadingScreen("Opening income entry");
                addIncome();
                break;

            case 2:
                loadingScreen("Loading income history");
                viewIncome();
                break;

            case 3:
                loadingScreen("Calculating cash flow");
                cashFlowSummary();
                break;

            case 4:
                loadingScreen("Returning to My Money");
                break;

            default:
                loadingScreen("Checking selection");
                printf(RED "Invalid option.\n" RESET);
        }

    } while (choice != 4);
}

/* =========================================================
   SPENDING ANALYSIS
   ========================================================= */

void spendingAnalysis() {
    int i, d, m, y;
    int topIndex = 0;
    float spent, percentage;
    float highestSpent = 0;
    float monthSpent, monthIncome;

    getCurrentDate(&d, &m, &y);
    monthSpent = calculateCurrentMonthSpent();
    monthIncome = calculateCurrentMonthIncome();

    if (monthIncome <= 0)
        monthIncome = users[currentUser].monthlyAllowance;

    printLine();
    printf(CYAN BOLD "                    SPENDING ANALYSIS\n" RESET);
    printf("                     %02d/%04d\n", m, y);
    printLine();

    printf("Monthly Money In : Rs %.2f\n", monthIncome);
    printf("Spent This Month : Rs %.2f\n", monthSpent);
    printf("Money Left       : Rs %.2f\n", monthIncome - monthSpent);

    printf("\nCATEGORY BREAKDOWN\n");
    printSmallLine();

    for (i = 0; i < CAT_COUNT; i++) {
        spent = calculateCurrentMonthCategory(categories[i]);

        if (spent > highestSpent) {
            highestSpent = spent;
            topIndex = i;
        }

        percentage = users[currentUser].budgets[i] > 0
                   ? spent / users[currentUser].budgets[i] * 100
                   : 0;

        printf("\n%-15s ", categories[i]);
        showBar(percentage, 0);
        printf("  Rs %.0f / %.0f", spent, users[currentUser].budgets[i]);

        if (percentage >= 100)
            printf(RED "  EXCEEDED" RESET);
        else if (percentage >= 80)
            printf(RED "  WARNING" RESET);
        else if (percentage >= 60)
            printf(YELLOW "  WATCH" RESET);
        else
            printf(GREEN "  SAFE" RESET);
    }

    printf("\n\n");
    printSmallLine();

    if (highestSpent > 0) {
        printf(BOLD "Top Spending Category : %s\n" RESET, categories[topIndex]);
        printf("Spent in Category     : Rs %.2f\n", highestSpent);
    }

    printf("This analysis uses CURRENT MONTH transactions only.\n");
    printLine();
}

/* =========================================================
   SAVINGS
   ========================================================= */

void showSavings() {
    float mp = users[currentUser].monthlyGoal > 0
             ? users[currentUser].monthlySaved /
               users[currentUser].monthlyGoal * 100 : 0;

    float yp = users[currentUser].yearlyGoal > 0
             ? users[currentUser].yearlySaved /
               users[currentUser].yearlyGoal * 100 : 0;

    printf("\nMonthly Savings Goal\n");
    printf("Rs %.2f / Rs %.2f\n",
           users[currentUser].monthlySaved,
           users[currentUser].monthlyGoal);

    showBar(mp, 1);
    printf("\n");

    if (mp >= 100)
        printf(GREEN BOLD "GOAL ACHIEVED!\n" RESET);

    printf("\nYearly Savings Goal\n");
    printf("Rs %.2f / Rs %.2f\n",
           users[currentUser].yearlySaved,
           users[currentUser].yearlyGoal);

    showBar(yp, 1);
    printf("\n");

    if (yp >= 100)
        printf(GREEN BOLD "GOAL ACHIEVED!\n" RESET);
}

void savingsMenu() {
    int choice;
    float amount;

    do {
        printf("\n");
        printLine();
        printf(CYAN BOLD "                     SAVINGS GOALS\n" RESET);
        printLine();

        showSavings();

        printf("\n1. Set Monthly Goal\n");
        printf("2. Set Yearly Goal\n");
        printf("3. Add Money to Savings\n");
        printf("4. Back\n\n");

        printf("Choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                loadingScreen("Opening monthly goal");

                printf("New Monthly Goal: Rs ");
                scanf("%f", &amount);

                loadingScreen("Updating monthly goal");

                if (amount > 0) {
                    users[currentUser].monthlyGoal = amount;
                    saveUsers();
                    printf(GREEN "Monthly goal updated.\n" RESET);
                }
                break;

            case 2:
                loadingScreen("Opening yearly goal");

                printf("New Yearly Goal: Rs ");
                scanf("%f", &amount);

                loadingScreen("Updating yearly goal");

                if (amount > 0) {
                    users[currentUser].yearlyGoal = amount;
                    saveUsers();
                    printf(GREEN "Yearly goal updated.\n" RESET);
                }
                break;

            case 3:
                loadingScreen("Opening savings deposit");

                printf("Amount to Save: Rs ");
                scanf("%f", &amount);

                loadingScreen("Moving money into savings");

                if (amount > 0) {
                    users[currentUser].monthlySaved += amount;
                    users[currentUser].yearlySaved += amount;

                    saveUsers();

                    printf(GREEN "Rs %.2f added to savings.\n" RESET,
                           amount);
                }
                break;

            case 4:
                loadingScreen("Returning to My Money");
                break;

            default:
                loadingScreen("Checking selection");
                printf(RED "Invalid option.\n" RESET);
        }

    } while (choice != 4);
}

/* =========================================================
   BUDGET / ALLOWANCE
   ========================================================= */

void changeCategoryBudget() {
    int i, choice;
    float amount;

    printLine();
    printf(CYAN BOLD "                    CATEGORY BUDGETS\n" RESET);
    printLine();

    for (i = 0; i < CAT_COUNT; i++)
        printf("%d. %-15s Rs %.2f\n",
               i + 1, categories[i],
               users[currentUser].budgets[i]);

    printf("\nSelect Category: ");
    scanf("%d", &choice);

    if (choice < 1 || choice > CAT_COUNT) {
        printf(RED "Invalid category.\n" RESET);
        return;
    }

    printf("New Budget: Rs ");
    scanf("%f", &amount);

    loadingScreen("Updating category budget");

    if (amount > 0) {
        users[currentUser].budgets[choice - 1] = amount;
        saveUsers();

        printf(GREEN "Budget updated successfully.\n" RESET);
    }
}

void changeAllowance() {
    float amount;

    printf("Current Monthly Allowance: Rs %.2f\n",
           users[currentUser].monthlyAllowance);

    printf("New Monthly Allowance: Rs ");
    scanf("%f", &amount);

    loadingScreen("Updating monthly allowance");

    if (amount > 0) {
        users[currentUser].monthlyAllowance = amount;
        saveUsers();

        printf(GREEN "Monthly allowance updated.\n" RESET);
    }
}

/* =========================================================
   COMMITMENTS
   ========================================================= */

void addCommitment() {
    int n = users[currentUser].commitmentCount;
    Commitment *c;

    if (n >= MAX_C) {
        printf(RED "Commitment storage full.\n" RESET);
        return;
    }

    c = &users[currentUser].commitments[n];
    c->id = generateSmallID();

    printf("Commitment Name: ");
    scanf(" %49[^\n]", c->name);

    do {
        printf("Amount: Rs ");
        scanf("%f", &c->amount);
    } while (c->amount <= 0);

    do {
        printf("Due in how many days? ");
        scanf("%d", &c->daysLeft);
    } while (c->daysLeft < 0);

    printf("\nPriority\n");
    printf("1. Essential\n2. Important\n3. Optional\n");

    do {
        printf("\nChoice: ");
        scanf("%d", &c->priority);
    } while (c->priority < 1 || c->priority > 3);

    loadingScreen("Protecting commitment");

    c->paid = 0;
    users[currentUser].commitmentCount++;

    saveUsers();

    printf(GREEN "Commitment protected successfully.\n" RESET);
    printf("Commitment ID: %d\n", c->id);
}

void viewCommitments() {
    int i;
    char *priority;

    printLine();
    printf(CYAN BOLD "                 UPCOMING COMMITMENTS\n" RESET);
    printLine();

    if (!users[currentUser].commitmentCount) {
        printf("No commitments added.\n");
        return;
    }

    for (i = 0; i < users[currentUser].commitmentCount; i++) {
        Commitment *c = &users[currentUser].commitments[i];

        priority = c->priority == 1 ? "Essential" :
                   c->priority == 2 ? "Important" : "Optional";

        printf("\nID       : %d\n", c->id);
        printf("Name     : %s\n", c->name);
        printf("Amount   : Rs %.2f\n", c->amount);
        printf("Due      : %d days\n", c->daysLeft);
        printf("Priority : %s\n", priority);
        printf("Status   : %s\n", c->paid ? "PAID" : "PENDING");

        printSmallLine();
    }

    printf("Total Pending: Rs %.2f\n", calculateCommitments());
}

void markCommitmentPaid() {
    int id, i;

    viewCommitments();

    if (!users[currentUser].commitmentCount)
        return;

    printf("\nEnter Commitment ID: ");
    scanf("%d", &id);

    loadingScreen("Updating commitment");

    for (i = 0; i < users[currentUser].commitmentCount; i++) {
        if (users[currentUser].commitments[i].id == id) {
            users[currentUser].commitments[i].paid = 1;
            saveUsers();

            printf(GREEN "Commitment marked as paid.\n" RESET);
            return;
        }
    }

    printf(RED "Commitment not found.\n" RESET);
}

void removeCommitment() {
    int id, i, j;

    viewCommitments();

    if (!users[currentUser].commitmentCount)
        return;

    printf("\nEnter Commitment ID to remove: ");
    scanf("%d", &id);

    loadingScreen("Removing commitment");

    for (i = 0; i < users[currentUser].commitmentCount; i++) {
        if (users[currentUser].commitments[i].id == id) {
            for (j = i;
                 j < users[currentUser].commitmentCount - 1;
                 j++)
                users[currentUser].commitments[j] =
                    users[currentUser].commitments[j + 1];

            users[currentUser].commitmentCount--;

            saveUsers();

            printf(GREEN "Commitment removed.\n" RESET);
            return;
        }
    }

    printf(RED "Commitment not found.\n" RESET);
}

void commitmentsMenu() {
    int choice;

    do {
        printf("\n");
        printLine();
        printf(CYAN BOLD "                   MONEY COMMITMENTS\n" RESET);
        printLine();

        printf("1. Add Commitment\n");
        printf("2. View Commitments\n");
        printf("3. Mark Commitment Paid\n");
        printf("4. Remove Commitment\n");
        printf("5. Back\n\n");

        printf("Choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                loadingScreen("Opening commitment entry");
                addCommitment();
                break;

            case 2:
                loadingScreen("Loading commitments");
                viewCommitments();
                break;

            case 3:
                loadingScreen("Opening commitment payment");
                markCommitmentPaid();
                break;

            case 4:
                loadingScreen("Opening commitment removal");
                removeCommitment();
                break;

            case 5:
                loadingScreen("Returning to Smart Guardian");
                break;

            default:
                loadingScreen("Checking selection");
                printf(RED "Invalid option.\n" RESET);
        }

    } while (choice != 5);
}

/* =========================================================
   EMERGENCY FUND
   ========================================================= */

void showEmergencyFund() {
    float percentage = users[currentUser].emergencyTarget > 0
                     ? users[currentUser].emergencyFund /
                       users[currentUser].emergencyTarget * 100
                     : 0;

    printLine();
    printf(CYAN BOLD "                    EMERGENCY FUND\n" RESET);
    printLine();

    printf("Target             : Rs %.2f\n",
           users[currentUser].emergencyTarget);

    printf("Currently Protected: Rs %.2f\n",
           users[currentUser].emergencyFund);

    printf("\n");
    showBar(percentage, 1);
    printf("\n");

    if (users[currentUser].emergencyFund >=
        users[currentUser].emergencyTarget)
        printf(GREEN BOLD "EMERGENCY TARGET ACHIEVED!\n" RESET);
    else
        printf("Still Needed        : Rs %.2f\n",
               users[currentUser].emergencyTarget -
               users[currentUser].emergencyFund);
}

void emergencyFundMenu() {
    int choice;
    float amount;

    do {
        showEmergencyFund();

        printf("\n1. Set Emergency Target\n");
        printf("2. Add Money\n");
        printf("3. Emergency Withdrawal\n");
        printf("4. Back\n\n");

        printf("Choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                loadingScreen("Opening emergency target");

                printf("New Emergency Target: Rs ");
                scanf("%f", &amount);

                loadingScreen("Updating emergency target");

                if (amount > 0) {
                    users[currentUser].emergencyTarget = amount;
                    saveUsers();

                    printf(GREEN "Emergency target updated.\n" RESET);
                }
                break;

            case 2:
                loadingScreen("Opening emergency deposit");

                printf("Amount to Protect: Rs ");
                scanf("%f", &amount);

                loadingScreen("Protecting emergency money");

                if (amount > 0) {
                    users[currentUser].emergencyFund += amount;
                    saveUsers();

                    printf(GREEN "Money added to emergency fund.\n" RESET);
                }
                break;

            case 3:
                loadingScreen("Opening emergency withdrawal");

                printf("Emergency Withdrawal: Rs ");
                scanf("%f", &amount);

                if (amount > 0 &&
                    amount <= users[currentUser].emergencyFund) {
                    loadingScreen("Processing emergency withdrawal");

                    users[currentUser].emergencyFund -= amount;
                    saveUsers();

                    printf(YELLOW "Emergency withdrawal recorded.\n" RESET);
                }
                else
                    printf(RED "Invalid withdrawal amount.\n" RESET);

                break;

            case 4:
                loadingScreen("Returning to Smart Guardian");
                break;

            default:
                loadingScreen("Checking selection");
                printf(RED "Invalid option.\n" RESET);
        }

    } while (choice != 4);
}

/* =========================================================
   SUBSCRIPTIONS
   ========================================================= */

void addSubscription() {
    int n = users[currentUser].subscriptionCount;
    Subscription *s;

    if (n >= MAX_S) {
        printf(RED "Subscription storage full.\n" RESET);
        return;
    }

    s = &users[currentUser].subscriptions[n];
    s->id = generateSmallID();

    printf("Subscription Name: ");
    scanf(" %49[^\n]", s->name);

    do {
        printf("Amount: Rs ");
        scanf("%f", &s->amount);
    } while (s->amount <= 0);

    printf("\n1. Monthly\n2. Yearly\n");

    do {
        printf("Frequency: ");
        scanf("%d", &s->frequency);
    } while (s->frequency < 1 || s->frequency > 2);

    do {
        printf("Next payment in days: ");
        scanf("%d", &s->daysUntilDue);
    } while (s->daysUntilDue < 0);

    printf("Category: ");
    scanf(" %19[^\n]", s->category);

    loadingScreen("Adding recurring subscription");

    s->active = 1;
    users[currentUser].subscriptionCount++;

    saveUsers();

    printf(GREEN "Subscription added successfully.\n" RESET);
}

void viewSubscriptions() {
    int i, found = 0;

    printLine();
    printf(CYAN BOLD "                  ACTIVE SUBSCRIPTIONS\n" RESET);
    printLine();

    for (i = 0; i < users[currentUser].subscriptionCount; i++) {
        Subscription *s = &users[currentUser].subscriptions[i];

        if (!s->active)
            continue;

        found = 1;

        printf("\nID        : %d\n", s->id);
        printf("Name      : %s\n", s->name);
        printf("Amount    : Rs %.2f\n", s->amount);
        printf("Frequency : %s\n",
               s->frequency == 1 ? "Monthly" : "Yearly");
        printf("Due       : %d days\n", s->daysUntilDue);
        printf("Category  : %s\n", s->category);

        printSmallLine();
    }

    if (!found)
        printf("No active subscriptions.\n");

    printf("\nMonthly Cost : Rs %.2f\n", calculateSubscriptions());
    printf("Yearly Cost  : Rs %.2f\n", calculateYearlySubscriptions());
}

void markSubscriptionPaid() {
    int id, i;

    viewSubscriptions();

    printf("\nEnter Subscription ID: ");
    scanf("%d", &id);

    loadingScreen("Updating subscription payment");

    for (i = 0; i < users[currentUser].subscriptionCount; i++) {
        Subscription *s = &users[currentUser].subscriptions[i];

        if (s->id == id && s->active) {
            s->daysUntilDue = s->frequency == 1 ? 30 : 365;

            saveUsers();

            printf(GREEN
                   "Subscription payment marked successfully.\n"
                   RESET);
            return;
        }
    }

    printf(RED "Subscription not found.\n" RESET);
}

void cancelSubscription() {
    int id, i;

    viewSubscriptions();

    printf("\nEnter Subscription ID: ");
    scanf("%d", &id);

    loadingScreen("Cancelling subscription");

    for (i = 0; i < users[currentUser].subscriptionCount; i++) {
        if (users[currentUser].subscriptions[i].id == id) {
            users[currentUser].subscriptions[i].active = 0;

            saveUsers();

            printf(GREEN "Subscription cancelled.\n" RESET);
            return;
        }
    }

    printf(RED "Subscription not found.\n" RESET);
}

void subscriptionAnalysis() {
    float monthly = calculateSubscriptions();
    float yearly = calculateYearlySubscriptions();

    float percentage = users[currentUser].monthlyAllowance > 0
                     ? monthly /
                       users[currentUser].monthlyAllowance * 100
                     : 0;

    printLine();
    printf(MAGENTA BOLD "                 SUBSCRIPTION ANALYSIS\n" RESET);
    printLine();

    printf("Monthly Cost       : Rs %.2f\n", monthly);
    printf("Estimated Yearly   : Rs %.2f\n", yearly);
    printf("Allowance Consumed : %.1f%%\n", percentage);

    if (percentage >= 20)
        printf(RED BOLD "\nHIGH RECURRING COST\n" RESET);
    else if (percentage >= 10)
        printf(YELLOW "\nReview your recurring subscriptions.\n" RESET);
    else
        printf(GREEN "\nRecurring cost is manageable.\n" RESET);
}

void subscriptionMenu() {
    int choice;

    do {
        printLine();
        printf(CYAN BOLD "                RECURRING SUBSCRIPTIONS\n" RESET);
        printLine();

        printf("1. Add Subscription\n");
        printf("2. View Subscriptions\n");
        printf("3. Mark Subscription Paid\n");
        printf("4. Cancel Subscription\n");
        printf("5. Subscription Analysis\n");
        printf("6. Back\n\n");

        printf("Choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                loadingScreen("Opening subscription entry");
                addSubscription();
                break;

            case 2:
                loadingScreen("Loading subscriptions");
                viewSubscriptions();
                break;

            case 3:
                loadingScreen("Opening subscription payment");
                markSubscriptionPaid();
                break;

            case 4:
                loadingScreen("Opening subscription cancellation");
                cancelSubscription();
                break;

            case 5:
                loadingScreen("Analysing recurring payments");
                subscriptionAnalysis();
                break;

            case 6:
                loadingScreen("Returning to Smart Guardian");
                break;

            default:
                loadingScreen("Checking selection");
                printf(RED "Invalid option.\n" RESET);
        }

    } while (choice != 6);
}

/* =========================================================
   SPENDING LEAKAGE
   ========================================================= */

void spendingLeakageDetector() {
    int i, count;
    int leakIndex = -1, highestCount = 0;

    float spent, highestSpent = 0;

    printLine();
    printf(MAGENTA BOLD "                SPENDING LEAKAGE DETECTOR\n" RESET);
    printLine();

    if (!users[currentUser].transactionCount) {
        printf("No transactions available.\n");
        return;
    }

    printf("\nAnalysing repeated spending...\n\n");

    for (i = 0; i < CAT_COUNT; i++) {
        count = countCurrentMonthCategoryTransactions(categories[i]);
        spent = calculateCurrentMonthCategory(categories[i]);

        if (count > 0)
            printf("%-15s : %d payments | Rs %.2f\n",
                   categories[i], count, spent);

        if (count >= 2 &&
            (count > highestCount ||
             (count == highestCount && spent > highestSpent))) {
            highestCount = count;
            highestSpent = spent;
            leakIndex = i;
        }
    }

    if (leakIndex == -1) {
        printf(GREEN "\nNo repeated spending leak detected.\n" RESET);
        return;
    }

    printf("\n");
    printSmallLine();

    printf(RED BOLD "POTENTIAL LEAK DETECTED\n" RESET);

    printf("\nCategory         : %s\n", categories[leakIndex]);
    printf("Transactions     : %d\n", highestCount);
    printf("Total Spent      : Rs %.2f\n", highestSpent);
    printf("Possible Saving  : Rs %.2f/month\n", highestSpent * 0.30f);
    printf("Yearly Saving    : Rs %.2f\n", highestSpent * 0.30f * 12);

    printLine();
}

/* =========================================================
   FINANCIAL HEALTH
   ========================================================= */

void showFinancialRisk() {
    int score = calculateRiskScore();

    printLine();
    printf(CYAN BOLD "                  FINANCIAL RISK SCORE\n" RESET);
    printLine();

    printf("\n");
    showBar((float)score, 1);

    printf("\n\nScore: %d / 100\n", score);

    if (score >= 80)
        printf(GREEN BOLD "Status: LOW RISK\n" RESET);
    else if (score >= 60)
        printf(YELLOW BOLD "Status: MODERATE RISK\n" RESET);
    else
        printf(RED BOLD "Status: HIGH RISK\n" RESET);

    printf("\nFactors considered:\n");
    printf("- Current spending\n");
    printf("- Upcoming commitments\n");
    printf("- Recurring subscriptions\n");
    printf("- Savings progress\n");
    printf("- Emergency fund\n");
    printf("- Actually safe money\n");

    printLine();
}

/* =========================================================
   MONEY FORECAST
   ========================================================= */

void moneyForecast() {
    int daysPassed, daysRemaining;

    float spent, dailyAverage;
    float safe, recommended, daysMoneyWillLast;

    printLine();
    printf(CYAN BOLD "                    MONEY FORECAST\n" RESET);
    printLine();

    do {
        printf("How many days have passed this month? ");
        scanf("%d", &daysPassed);
    } while (daysPassed < 1 || daysPassed > 30);

    loadingScreen("Calculating money forecast");

    daysRemaining = 30 - daysPassed;
    spent = calculateCurrentMonthSpent();
    dailyAverage = spent / daysPassed;
    safe = calculateSafeMoney();

    recommended = daysRemaining > 0
                ? safe / daysRemaining : safe;

    daysMoneyWillLast = dailyAverage > 0
                      ? safe / dailyAverage : 999;

    printf("Days Passed          : %d\n", daysPassed);
    printf("Days Remaining       : %d\n", daysRemaining);
    printf("Spent So Far         : Rs %.2f\n", spent);
    printf("Average Daily Spend  : Rs %.2f\n", dailyAverage);
    printf("Actually Safe Money  : Rs %.2f\n", safe);
    printf("Recommended / Day    : Rs %.2f\n", recommended);

    if (daysRemaining > 0 &&
        daysMoneyWillLast < daysRemaining) {
        printf(RED BOLD "\nFINANCIAL RISK DETECTED\n" RESET);

        printf("At your current spending rate,\n");
        printf("safe money may last only %.1f days.\n",
               daysMoneyWillLast);

        printf("But %d days remain.\n", daysRemaining);
    }
    else
        printf(GREEN BOLD
               "\nSPENDING RATE IS CURRENTLY SAFE\n"
               RESET);

    printLine();
}

/* =========================================================
   PURCHASE GUARDIAN
   ========================================================= */

void canIAffordIt() {
    char item[MAX_NAME];

    float price;
    float available, safe;

    printLine();
    printf(CYAN BOLD "                  PURCHASE GUARDIAN\n" RESET);
    printLine();

    printf("\nWhat do you want to buy? ");
    scanf(" %49[^\n]", item);

    do {
        printf("Price: Rs ");
        scanf("%f", &price);
    } while (price <= 0);

    loadingScreen("Analysing purchase");

    available = calculateAvailableMoney();
    safe = calculateSafeMoney();

    printf("Item                  : %s\n", item);
    printf("Price                 : Rs %.2f\n", price);
    printf("Available Money       : Rs %.2f\n", available);
    printf("Future Commitments    : Rs %.2f\n", calculateCommitments());
    printf("Subscriptions         : Rs %.2f\n", calculateSubscriptions());
    printf("Savings Still Needed  : Rs %.2f\n", savingsStillNeeded());
    printf("Emergency Protected   : Rs %.2f\n",
           users[currentUser].emergencyFund);

    printf(BOLD "Actually Safe Money   : Rs %.2f\n" RESET, safe);

    printf("\n");

    if (price > available) {
        printf(RED BOLD "RESULT: CANNOT AFFORD\n" RESET);
        printf("You are short by Rs %.2f.\n", price - available);
    }
    else if (price > safe) {
        printf(RED BOLD "RESULT: NOT RECOMMENDED\n" RESET);

        printf("Purchase exceeds your safe limit by Rs %.2f.\n",
               price - safe);
    }
    else if (safe > 0 && price >= safe * 0.70f) {
        printf(YELLOW BOLD "RESULT: BUY WITH CAUTION\n" RESET);
        printf("Safe money after purchase: Rs %.2f\n", safe - price);
    }
    else {
        printf(GREEN BOLD "RESULT: SAFE TO BUY\n" RESET);
        printf("Safe money after purchase: Rs %.2f\n", safe - price);
    }

    printLine();
}

/* =========================================================
   WHAT-IF SIMULATOR
   ========================================================= */

void whatIfSimulator() {
    char item[MAX_NAME];

    float price;
    float before, after;

    int scoreBefore, scoreAfter;

    printLine();
    printf(MAGENTA BOLD "                   WHAT-IF SIMULATOR\n" RESET);
    printLine();

    printf("\nHypothetical Purchase: ");
    scanf(" %49[^\n]", item);

    do {
        printf("Price: Rs ");
        scanf("%f", &price);
    } while (price <= 0);

    loadingScreen("Simulating financial impact");

    before = calculateSafeMoney();
    scoreBefore = calculateRiskScore();

    after = before - price;
    if (after < 0) after = 0;

    scoreAfter = scoreBefore -
        (int)(price /
        (users[currentUser].monthlyAllowance > 0
         ? users[currentUser].monthlyAllowance : 1) * 40);

    if (price > before)
        scoreAfter -= 15;

    if (scoreAfter < 0)
        scoreAfter = 0;

    printf("BEFORE PURCHASE\n");
    printf("Safe Money       : Rs %.2f\n", before);
    printf("Financial Score  : %d / 100\n", scoreBefore);

    printf("\nAFTER %s\n", item);
    printf("Safe Money       : Rs %.2f\n", after);
    printf("Estimated Score  : %d / 100\n", scoreAfter);

    if (price > before)
        printf(RED BOLD "\nHIGH FINANCIAL IMPACT\n" RESET);
    else if (scoreAfter < 60)
        printf(YELLOW BOLD "\nSIGNIFICANT FINANCIAL IMPACT\n" RESET);
    else
        printf(GREEN BOLD "\nMANAGEABLE FINANCIAL IMPACT\n" RESET);

    printf("\nSimulation only - no transaction was added.\n");

    printLine();
}

/* =========================================================
   RECOVERY PLAN
   ========================================================= */

void recoveryPlan() {
    int i;

    float spent, reduction;

    printLine();
    printf(CYAN BOLD "                    RECOVERY PLAN\n" RESET);
    printLine();

    if (calculateRiskScore() >= 60 &&
        calculateSafeMoney() >
        users[currentUser].monthlyAllowance * 0.15f) {
        printf(GREEN BOLD "\nNo major recovery required.\n" RESET);
        printf("Your current financial position is manageable.\n");
        return;
    }

    printf(RED BOLD "\nRECOVERY REQUIRED\n" RESET);

    reduction = calculateCurrentMonthSpent() * 0.20f;

    printf("\nRecommended reduction: Rs %.2f\n", reduction);
    printf("\nSuggested areas to reduce:\n\n");

    for (i = 0; i < CAT_COUNT; i++) {
        if (strcmp(categories[i], "Shopping") == 0 ||
            strcmp(categories[i], "Entertainment") == 0 ||
            strcmp(categories[i], "Food") == 0) {
            spent = calculateCurrentMonthCategory(categories[i]);

            if (spent > 0)
                printf("%-15s Current Rs %.2f | Suggested Cut Rs %.2f\n",
                       categories[i], spent, spent * 0.25f);
        }
    }

    printf("\nPriority order:\n");
    printf("1. Protect essential commitments\n");
    printf("2. Protect recurring subscriptions\n");
    printf("3. Protect savings target\n");
    printf("4. Reduce Shopping/Entertainment\n");
    printf("5. Reduce repeated Food spending\n");
    printf("6. Never use emergency fund for normal overspending\n");

    printLine();
}

/* =========================================================
   MONEY HISTORY
   ========================================================= */

char *monthName(int month) {
    char *names[12] = {
        "January", "February", "March", "April", "May", "June",
        "July", "August", "September", "October", "November", "December"
    };

    if (month < 1 || month > 12) return "Unknown";
    return names[month - 1];
}

float calculateYearSpent(int year) {
    int i;
    float total = 0;

    for (i = 0; i < users[currentUser].transactionCount; i++)
        if (users[currentUser].transactions[i].year == year)
            total += users[currentUser].transactions[i].amount;

    return total;
}

float calculateYearIncome(int year) {
    int i;
    float total = 0;

    for (i = 0; i < users[currentUser].incomeCount; i++)
        if (users[currentUser].incomes[i].year == year)
            total += users[currentUser].incomes[i].amount;

    return total;
}

float calculateYearCategory(char category[], int year) {
    int i;
    float total = 0;

    for (i = 0; i < users[currentUser].transactionCount; i++) {
        Transaction *t = &users[currentUser].transactions[i];
        if (t->year == year && strcmp(t->category, category) == 0)
            total += t->amount;
    }

    return total;
}

void showMonthHistory() {
    int month, year, i, found = 0;
    float income, expenses, net;

    do {
        printf("Month (1-12): ");
        scanf("%d", &month);
    } while (month < 1 || month > 12);

    printf("Year: ");
    scanf("%d", &year);

    loadingScreen("Loading monthly statement");

    income = calculateMonthIncome(month, year);
    expenses = calculateMonthSpent(month, year);
    net = income - expenses;

    printLine();
    printf(CYAN BOLD "                 %s %d STATEMENT\n" RESET,
           monthName(month), year);
    printLine();

    printf("\nMONEY IN\n");
    printSmallLine();

    for (i = 0; i < users[currentUser].incomeCount; i++) {
        Income *in = &users[currentUser].incomes[i];
        if (in->month == month && in->year == year) {
            printf("%02d/%02d  %-30s " GREEN "+Rs %.2f\n" RESET,
                   in->day, in->month, in->source, in->amount);
            found = 1;
        }
    }

    if (!found) printf("No income recorded for this month.\n");

    printf("\nMONEY OUT\n");
    printSmallLine();
    found = 0;

    for (i = 0; i < users[currentUser].transactionCount; i++) {
        Transaction *t = &users[currentUser].transactions[i];
        if (t->month == month && t->year == year) {
            printf("%02d/%02d  %-22s %-14s " RED "-Rs %.2f\n" RESET,
                   t->day, t->month, t->merchant, t->category, t->amount);
            found = 1;
        }
    }

    if (!found) printf("No expenses recorded for this month.\n");

    printf("\n");
    printSmallLine();
    printf("TOTAL INCOME      : Rs %.2f\n", income);
    printf("TOTAL SPENDING    : Rs %.2f\n", expenses);
    printf("NET CASH FLOW     : ");
    if (net >= 0) printf(GREEN "+Rs %.2f\n" RESET, net);
    else printf(RED "-Rs %.2f\n" RESET, -net);

    printf("\nCATEGORY BREAKDOWN\n");
    printSmallLine();

    for (i = 0; i < CAT_COUNT; i++) {
        float amount = calculateMonthCategory(categories[i], month, year);
        if (amount > 0)
            printf("%-15s Rs %.2f\n", categories[i], amount);
    }

    printLine();
}

void showSixMonthHistory() {
    int d, currentMonth, currentYear;
    int offset, month, year;
    float totalSpent = 0, totalIncome = 0;
    float highest = -1, lowest = -1;
    int highMonth = 0, highYear = 0, lowMonth = 0, lowYear = 0;

    getCurrentDate(&d, &currentMonth, &currentYear);

    loadingScreen("Building six month history");

    printLine();
    printf(CYAN BOLD "                   6-MONTH MONEY HISTORY\n" RESET);
    printLine();
    printf("%-12s %-13s %-13s %-13s\n", "MONTH", "INCOME", "SPENT", "NET");
    printSmallLine();

    for (offset = 5; offset >= 0; offset--) {
        month = currentMonth - offset;
        year = currentYear;

        while (month <= 0) {
            month += 12;
            year--;
        }

        {
            float income = calculateMonthIncome(month, year);
            float spent = calculateMonthSpent(month, year);
            float net = income - spent;

            printf("%-12s Rs %-10.2f Rs %-10.2f ", monthName(month), income, spent);
            if (net >= 0) printf(GREEN "+Rs %.2f\n" RESET, net);
            else printf(RED "-Rs %.2f\n" RESET, -net);

            totalIncome += income;
            totalSpent += spent;

            if (spent > 0 && (highest < 0 || spent > highest)) {
                highest = spent;
                highMonth = month;
                highYear = year;
            }

            if (spent > 0 && (lowest < 0 || spent < lowest)) {
                lowest = spent;
                lowMonth = month;
                lowYear = year;
            }
        }
    }

    printSmallLine();
    printf("6-Month Income      : Rs %.2f\n", totalIncome);
    printf("6-Month Spending    : Rs %.2f\n", totalSpent);
    printf("Net Cash Flow       : Rs %.2f\n", totalIncome - totalSpent);
    printf("Average / Month     : Rs %.2f\n", totalSpent / 6.0f);

    if (highest >= 0)
        printf("Highest Spend       : %s %d (Rs %.2f)\n",
               monthName(highMonth), highYear, highest);

    if (lowest >= 0)
        printf("Lowest Spend        : %s %d (Rs %.2f)\n",
               monthName(lowMonth), lowYear, lowest);

    printLine();
}

void compareMonths() {
    int m1, y1, m2, y2;
    float i1, i2, e1, e2, n1, n2, change;

    do {
        printf("First Month (1-12): ");
        scanf("%d", &m1);
    } while (m1 < 1 || m1 > 12);

    printf("First Year: ");
    scanf("%d", &y1);

    do {
        printf("Second Month (1-12): ");
        scanf("%d", &m2);
    } while (m2 < 1 || m2 > 12);

    printf("Second Year: ");
    scanf("%d", &y2);

    loadingScreen("Comparing financial history");

    i1 = calculateMonthIncome(m1, y1);
    i2 = calculateMonthIncome(m2, y2);
    e1 = calculateMonthSpent(m1, y1);
    e2 = calculateMonthSpent(m2, y2);
    n1 = i1 - e1;
    n2 = i2 - e2;

    printLine();
    printf(CYAN BOLD "                    MONTH COMPARISON\n" RESET);
    printLine();

    printf("%-18s %-18s %-18s\n", "", monthName(m1), monthName(m2));
    printf("Income             Rs %-12.2f Rs %.2f\n", i1, i2);
    printf("Expenses           Rs %-12.2f Rs %.2f\n", e1, e2);
    printf("Net Cash Flow      Rs %-12.2f Rs %.2f\n", n1, n2);

    if (e1 > 0) {
        change = (e2 - e1) / e1 * 100;
        printf("\nSpending Change    : %.1f%%\n", change);
    }

    if (e2 < e1)
        printf(GREEN "Spending improved by Rs %.2f\n" RESET, e1 - e2);
    else if (e2 > e1)
        printf(RED "Spending increased by Rs %.2f\n" RESET, e2 - e1);
    else
        printf("Spending remained the same.\n");

    printLine();
}

void yearlyHistory() {
    int year, month, i;
    float totalIncome, totalSpent;
    float highest = -1, lowest = -1;
    int highMonth = 0, lowMonth = 0, monthsWithData = 0;

    printf("Year: ");
    scanf("%d", &year);

    loadingScreen("Building yearly money history");

    totalIncome = calculateYearIncome(year);
    totalSpent = calculateYearSpent(year);

    printLine();
    printf(CYAN BOLD "                     %d YEAR IN REVIEW\n" RESET, year);
    printLine();

    for (month = 1; month <= 12; month++) {
        float spent = calculateMonthSpent(month, year);
        float income = calculateMonthIncome(month, year);
        int bars = 0, j;

        if (spent > 0 || income > 0) monthsWithData++;
        if (spent > 0 && (highest < 0 || spent > highest)) {
            highest = spent;
            highMonth = month;
        }
        if (spent > 0 && (lowest < 0 || spent < lowest)) {
            lowest = spent;
            lowMonth = month;
        }

        if (users[currentUser].monthlyAllowance > 0)
            bars = (int)(spent / users[currentUser].monthlyAllowance * 20);
        if (bars > 20) bars = 20;

        printf("%-9s [", monthName(month));
        for (j = 0; j < 20; j++) printf(j < bars ? "#" : "-");
        printf("] Rs %.2f\n", spent);
    }

    printSmallLine();
    printf("Total Income       : Rs %.2f\n", totalIncome);
    printf("Total Expenses     : Rs %.2f\n", totalSpent);
    printf("Net Cash Flow      : Rs %.2f\n", totalIncome - totalSpent);

    if (monthsWithData > 0)
        printf("Average / Active Mo: Rs %.2f\n", totalSpent / monthsWithData);

    if (highest >= 0)
        printf("Highest Spend      : %s (Rs %.2f)\n", monthName(highMonth), highest);
    if (lowest >= 0)
        printf("Lowest Spend       : %s (Rs %.2f)\n", monthName(lowMonth), lowest);

    printf("\nYEARLY CATEGORY BREAKDOWN\n");
    printSmallLine();

    for (i = 0; i < CAT_COUNT; i++) {
        float amount = calculateYearCategory(categories[i], year);
        printf("%-15s Rs %.2f\n", categories[i], amount);
    }

    printLine();
}

void yearlyCategoryAnalysis() {
    int year, i, top = -1;
    float highest = 0, total;

    printf("Year: ");
    scanf("%d", &year);

    loadingScreen("Analysing yearly categories");

    total = calculateYearSpent(year);

    printLine();
    printf(MAGENTA BOLD "                 YEARLY CATEGORY ANALYSIS\n" RESET);
    printLine();

    for (i = 0; i < CAT_COUNT; i++) {
        float amount = calculateYearCategory(categories[i], year);
        float percentage = total > 0 ? amount / total * 100 : 0;

        printf("%-15s Rs %-10.2f ", categories[i], amount);
        showBar(percentage, 0);
        printf("\n");

        if (amount > highest) {
            highest = amount;
            top = i;
        }
    }

    if (top >= 0)
        printf("\nHighest Spending Category: %s (Rs %.2f)\n",
               categories[top], highest);

    printLine();
}

void completeMoneyRecord() {
    int year, month, i;
    float totalIn = 0, totalOut = 0;

    printf("Year to view (2024-2026): ");
    scanf("%d", &year);

    if (year < 2024 || year > 2026) {
        printf(RED "History is available from 2024 to 2026.\n" RESET);
        return;
    }

    loadingScreen("Loading complete money record");

    printLine();
    printf(CYAN BOLD "                    COMPLETE MONEY RECORD\n" RESET);
    printf("                         YEAR %d\n", year);
    printLine();

    for (month = 1; month <= 12; month++) {
        float monthIn = calculateMonthIncome(month, year);
        float monthOut = calculateMonthSpent(month, year);

        if (monthIn <= 0 && monthOut <= 0)
            continue;

        printf("\n" BOLD "%s %d\n" RESET, monthName(month), year);
        printSmallLine();
        printf("DATE        DESCRIPTION                 TYPE           AMOUNT\n");
        printSmallLine();

        for (i = 0; i < users[currentUser].incomeCount; i++) {
            Income *in = &users[currentUser].incomes[i];

            if (in->month == month && in->year == year) {
                printf("%02d/%02d/%04d  %-27s %-12s " GREEN "+Rs %8.2f\n" RESET,
                       in->day, in->month, in->year,
                       in->source, "INCOME", in->amount);
            }
        }

        for (i = 0; i < users[currentUser].transactionCount; i++) {
            Transaction *t = &users[currentUser].transactions[i];

            if (t->month == month && t->year == year) {
                printf("%02d/%02d/%04d  %-27s %-12s " RED "-Rs %8.2f\n" RESET,
                       t->day, t->month, t->year,
                       t->merchant, t->category, t->amount);
            }
        }

        printf("%-42s Month In  : " GREEN "+Rs %.2f\n" RESET, "", monthIn);
        printf("%-42s Month Out : " RED "-Rs %.2f\n" RESET, "", monthOut);
        printf("%-42s Net       : Rs %.2f\n", "", monthIn - monthOut);

        totalIn += monthIn;
        totalOut += monthOut;
    }

    printf("\n");
    printLine();
    printf("TOTAL MONEY IN   : " GREEN "+Rs %.2f\n" RESET, totalIn);
    printf("TOTAL MONEY OUT  : " RED "-Rs %.2f\n" RESET, totalOut);
    printf(BOLD "NET CASH FLOW    : Rs %.2f\n" RESET, totalIn - totalOut);
    printLine();
}

void monthlyHistoryMenu() {
    int choice;

    do {
        printLine();
        printf(CYAN BOLD "                      MONEY HISTORY\n" RESET);
        printLine();

        printf("1. Complete Money Record\n");
        printf("2. View Monthly Statement\n");
        printf("3. Last 6 Months Overview\n");
        printf("4. Compare Two Months\n");
        printf("5. Yearly History\n");
        printf("6. Yearly Category Analysis\n");
        printf("7. Back\n\n");

        printf("Choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                loadingScreen("Opening complete money record");
                completeMoneyRecord();
                break;
            case 2:
                loadingScreen("Opening monthly statement");
                showMonthHistory();
                break;
            case 3:
                loadingScreen("Opening six month overview");
                showSixMonthHistory();
                break;
            case 4:
                loadingScreen("Opening month comparison");
                compareMonths();
                break;
            case 5:
                loadingScreen("Opening yearly history");
                yearlyHistory();
                break;
            case 6:
                loadingScreen("Opening yearly category analysis");
                yearlyCategoryAnalysis();
                break;
            case 7:
                loadingScreen("Returning to My Money");
                break;
            default:
                loadingScreen("Checking selection");
                printf(RED "Invalid option.\n" RESET);
        }

    } while (choice != 7);
}

/* =========================================================
   FINANCIAL SUMMARY
   ========================================================= */

void financialSummary() {
    printLine();
    printf(CYAN BOLD "                    FINANCIAL SUMMARY\n" RESET);
    printLine();

    printf("User                : %s\n", users[currentUser].name);
    printf("Current Month Income: Rs %.2f\n", calculateCurrentMonthIncome());
    printf("Current Month Spent : Rs %.2f\n", calculateCurrentMonthSpent());
    printf("Available Money     : Rs %.2f\n", calculateAvailableMoney());
    printf("Commitments         : Rs %.2f\n", calculateCommitments());
    printf("Subscriptions       : Rs %.2f/month\n", calculateSubscriptions());
    printf("Savings Required    : Rs %.2f\n", savingsStillNeeded());
    printf("Emergency Protected : Rs %.2f\n",
           users[currentUser].emergencyFund);

    printf(BOLD "ACTUALLY SAFE MONEY : Rs %.2f\n" RESET,
           calculateSafeMoney());

    printf("\nFinancial Score     : %d / 100\n", calculateRiskScore());
    printf("Transactions        : %d\n",
           users[currentUser].transactionCount);

    printf("Primary Bank        : %s\n",
           banks[users[currentUser].primaryBank]);

    printLine();
}

/* =========================================================
   MY MONEY
   ========================================================= */

void myMoney() {
    int choice;

    do {
        printf("\n");
        printLine();
        printf(CYAN BOLD "                         MY MONEY\n" RESET);
        printLine();

        printf("1. Spending Analysis\n");
        printf("2. Income Manager\n");
        printf("3. Savings Goals\n");
        printf("4. Category Budgets\n");
        printf("5. Change Monthly Allowance\n");
        printf("6. Money Records & History\n");
        printf("7. Financial Summary\n");
        printf("8. Back\n\n");

        printf("Choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                spendingAnimation();
                spendingAnalysis();
                break;

            case 2:
                loadingScreen("Opening income manager");
                incomeMenu();
                break;

            case 3:
                loadingScreen("Opening savings goals");
                savingsMenu();
                break;

            case 4:
                loadingScreen("Opening category budgets");
                changeCategoryBudget();
                break;

            case 5:
                loadingScreen("Opening allowance settings");
                changeAllowance();
                break;

            case 6:
                historyAnimation();
                monthlyHistoryMenu();
                break;

            case 7:
                loadingScreen("Building financial summary");
                financialSummary();
                break;

            case 8:
                loadingScreen("Returning to dashboard");
                break;

            default:
                loadingScreen("Checking selection");
                printf(RED "Invalid option.\n" RESET);
        }

    } while (choice != 8);
}

/* =========================================================
   SMART MONEY GUARDIAN
   ========================================================= */

void smartGuardian() {
    int choice;

    do {
        printf("\n");
        printLine();

        printf(MAGENTA BOLD "                  SMART MONEY GUARDIAN\n" RESET);

        printLine();

        printf("1. Upcoming Commitments\n");
        printf("2. Emergency Fund\n");
        printf("3. Recurring Subscriptions\n");
        printf("4. Money Forecast\n");
        printf("5. Can I Afford It?\n");
        printf("6. What-If Simulator\n");
        printf("7. Spending Leakage Detector\n");
        printf("8. Financial Risk Score\n");
        printf("9. Recovery Plan\n");
        printf("10. Back\n\n");

        printf("Choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                loadingScreen("Opening commitments");
                commitmentsMenu();
                break;

            case 2:
                loadingScreen("Opening emergency fund");
                emergencyFundMenu();
                break;

            case 3:
                loadingScreen("Opening subscriptions");
                subscriptionMenu();
                break;

            case 4:
                loadingScreen("Opening money forecast");
                moneyForecast();
                break;

            case 5:
                guardianAnimation();
                canIAffordIt();
                break;

            case 6:
                loadingScreen("Opening What-If Simulator");
                whatIfSimulator();
                break;

            case 7:
                loadingScreen("Detecting spending leakage");
                spendingLeakageDetector();
                break;

            case 8:
                riskAnimation();
                showFinancialRisk();
                break;

            case 9:
                loadingScreen("Building recovery plan");
                recoveryPlan();
                break;

            case 10:
                loadingScreen("Returning to dashboard");
                break;

            default:
                loadingScreen("Checking selection");
                printf(RED "Invalid option.\n" RESET);
        }

    } while (choice != 10);
}

/* =========================================================
   BANKS
   ========================================================= */

void viewBanks() {
    int i;

    printLine();
    printf(CYAN BOLD "                     LINKED ACCOUNTS\n" RESET);
    printLine();

    for (i = 0; i < BANK_COUNT; i++) {
        printf("\n" GREEN "✓ CONNECTED" RESET);

        printf("   %s ****%d",
               banks[i],
               users[currentUser].accountNumbers[i]);

        if (i == users[currentUser].primaryBank)
            printf(CYAN BOLD "   [PRIMARY]" RESET);

        printf("\n");
    }

    printf("\n");
    printLine();
}

void changePrimaryBank() {
    int i, choice;

    printf("\nSELECT PRIMARY BANK\n\n");

    for (i = 0; i < BANK_COUNT; i++)
        printf("%d. %s ****%d\n",
               i + 1, banks[i],
               users[currentUser].accountNumbers[i]);

    printf("\nChoice: ");
    scanf("%d", &choice);

    loadingScreen("Updating primary bank");

    if (choice >= 1 && choice <= BANK_COUNT) {
        users[currentUser].primaryBank = choice - 1;

        saveUsers();

        printf(GREEN "%s is now your primary bank.\n" RESET,
               banks[users[currentUser].primaryBank]);
    }
    else
        printf(RED "Invalid bank.\n" RESET);
}

/* =========================================================
   UPI SYNC
   ========================================================= */

void syncUPI() {
    char merchants[8][MAX_NAME] = {
        "ZOMATO", "RAPIDO", "BLINKIT", "AMAZON",
        "SWIGGY", "UPES BOOK STORE", "PHARMACY", "CAFE"
    };

    int merchant = rand() % 8;
    int bank = rand() % BANK_COUNT;
    int amount = 50 + rand() % 951;

    printLine();
    printf(CYAN BOLD "                      SYNCING UPI\n" RESET);
    printLine();

    printf("Checking HDFC Bank...\n");
    printf("Checking SBI...\n");
    printf("Checking Bank of India...\n");
    printf("Checking Axis Bank...\n");

    printf(GREEN "\nAll accounts synced.\n" RESET);

    printf("\nOne new payment detected:\n");

    printf("Rs %d paid to %s via %s\n",
           amount, merchants[merchant], banks[bank]);

    loadingScreen("Adding detected transaction");

    addTransaction(
        merchants[merchant],
        (float)amount,
        "UPI",
        banks[bank]
    );
}

void linkedBanksMenu() {
    int choice;

    do {
        printf("\n");
        printLine();

        printf(CYAN BOLD "                     LINKED BANKS\n" RESET);

        printLine();

        printf("1. View Linked Banks\n");
        printf("2. Change Primary Bank\n");
        printf("3. Sync UPI\n");
        printf("4. Back\n\n");

        printf("Choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                loadingScreen("Loading linked accounts");
                viewBanks();
                break;

            case 2:
                loadingScreen("Opening primary bank settings");
                changePrimaryBank();
                break;

            case 3:
                upiAnimation();
                syncUPI();
                break;

            case 4:
                loadingScreen("Returning to dashboard");
                break;

            default:
                loadingScreen("Checking selection");
                printf(RED "Invalid option.\n" RESET);
        }

    } while (choice != 4);
}

/* =========================================================
   DASHBOARD
   ========================================================= */

void showDashboard() {
    float income = calculateCurrentMonthIncome();
    int score = calculateRiskScore();

    if (income <= 0)
        income = users[currentUser].monthlyAllowance;

    printf("\n");
    printLine();

    printf(CYAN BOLD "                    YOUR MONEY TODAY\n" RESET);

    printLine();

    printf(GREEN "  ✓ ONLINE" RESET "   Logged in as: ");
    printf(BOLD "%s\n" RESET, users[currentUser].name);

    printSmallLine();

    printf("Primary Account      : %s ****%d\n",
           banks[users[currentUser].primaryBank],
           users[currentUser]
           .accountNumbers[users[currentUser].primaryBank]);

    printf("Account Status       : " GREEN "CONNECTED" RESET " (DEMO)\n");

    printf("\nMoney In             : Rs %.2f\n", income);
    printf("Spent This Month     : Rs %.2f\n", calculateCurrentMonthSpent());
    printf("Available            : Rs %.2f\n", calculateAvailableMoney());
    printf("Future Commitments   : Rs %.2f\n", calculateCommitments());
    printf("Subscriptions        : Rs %.2f/month\n", calculateSubscriptions());
    printf("Savings Needed       : Rs %.2f\n", savingsStillNeeded());
    printf("Emergency Protected  : Rs %.2f\n",
           users[currentUser].emergencyFund);

    printf("\n");
    printSmallLine();

    printf(BOLD "                 ACTUALLY SAFE MONEY\n" RESET);
    printf(GREEN BOLD "                     Rs %.2f\n" RESET,
           calculateSafeMoney());

    printSmallLine();

    printf("\nFinancial Health\n");
    showBar((float)score, 1);
    printf("  %d/100\n", score);

    showSavings();

    printf("\n");
    printLine();
}

/* =========================================================
   FINAL CINEMATIC EXIT - ABOUT 7 SECONDS
   ========================================================= */

void finalExitAnimation() {
    int i, j;

    /* Phase 1: secure the session step by step */
    for (i = 0; i < 4; i++) {
        clearScreen();
        printf("\n");
        printLine();
        printf(CYAN BOLD "                         HISAAB CORE\n" RESET);
        printLine();
        printf("\n                    CLOSING YOUR SESSION\n\n");

        printf("              Financial Records      %s\n", i >= 0 ? GREEN "[SECURED]" RESET : "[       ]");
        printf("              Transaction History    %s\n", i >= 1 ? GREEN "[SAVED]" RESET : "[       ]");
        printf("              User Profile           %s\n", i >= 2 ? GREEN "[SECURED]" RESET : "[       ]");
        printf("              Local Database         %s\n", i >= 3 ? GREEN "[LOCKED]" RESET : "[       ]");

        printf("\n");
        printLine();
        fflush(stdout);
        delayMs(350);
    }

    /* Phase 2: the rupee drops into the lock */
    for (i = 0; i < 5; i++) {
        clearScreen();
        printf("\n\n");
        printf(CYAN BOLD "                     SECURING HISAAB\n\n" RESET);

        for (j = 0; j < i; j++)
            printf("\n");

        printf(GREEN BOLD "                           ₹\n" RESET);

        for (j = i; j < 4; j++)
            printf("\n");

        printf("                        +-------+\n");
        printf("                        |  LOCK |\n");
        printf("                        +-------+\n");
        fflush(stdout);
        delayMs(250);
    }

    clearScreen();
    printf("\n\n\n");
    printf("                        +-------+\n");
    printf(GREEN BOLD "                        |  ₹ ✓  |\n" RESET);
    printf("                        +-------+\n\n");
    printf(GREEN BOLD "                     SESSION SECURED\n" RESET);
    fflush(stdout);
    delayMs(650);

    /* Phase 3: HISAAB collapses back to the rupee */
    {
        const char *frames[] = {
            "                     H I S A A B",
            "                       H I S A",
            "                         H I S",
            "                           H",
            "                           ₹"
        };

        for (i = 0; i < 5; i++) {
            clearScreen();
            printf("\n\n\n\n\n");
            printf(CYAN BOLD "%s\n" RESET, frames[i]);
            fflush(stdout);
            delayMs(280);
        }
    }

    /* A tiny blackout before the final reveal */
    clearScreen();
    fflush(stdout);
    delayMs(350);

    /* Phase 4: final reveal */
    clearScreen();
    printf(CYAN BOLD);
    printf("+==============================================================+\n");
    printf("|                                                              |\n");
    printf("|                           ₹                                  |\n");
    printf("|                                                              |\n");
    printf("|                     H I S A A B                              |\n");
    printf("|                                                              |\n");
    printf("|                   SESSION COMPLETE                           |\n");
    printf("|                                                              |\n");
    printf(RESET);
    printf(BOLD "|                MONEY DISAPPEARS QUIETLY.                     |\n");
    printf("|                                                              |\n");
    printf(GREEN BOLD "|             HISAAB MAKES EVERY RUPEE SPEAK.                  |\n" RESET);
    printf(CYAN BOLD);
    printf("|                                                              |\n");
    printf("|                           ✓                                  |\n");
    printf("|                                                              |\n");
    printf("+==============================================================+\n");
    printf(RESET);
    fflush(stdout);

    /* Hold the final quote long enough to land. */
    delayMs(1800);

    clearScreen();
}

/* =========================================================
   EXIT SUMMARY
   ========================================================= */

void exitSummary() {
    saveUsers();

    printLine();
    printf(CYAN BOLD "                     HISAAB SUMMARY\n" RESET);
    printLine();

    printf("User                : %s\n", users[currentUser].name);
    printf("Current Month Income: Rs %.2f\n", calculateCurrentMonthIncome());
    printf("Current Month Spent : Rs %.2f\n", calculateCurrentMonthSpent());
    printf("Transactions        : %d\n",
           users[currentUser].transactionCount);
    printf("Commitments         : Rs %.2f\n", calculateCommitments());
    printf("Subscriptions       : Rs %.2f/month\n", calculateSubscriptions());
    printf("Emergency Fund      : Rs %.2f\n",
           users[currentUser].emergencyFund);
    printf("Actually Safe Money : Rs %.2f\n", calculateSafeMoney());
    printf("Monthly Saved       : Rs %.2f\n",
           users[currentUser].monthlySaved);
    printf("Financial Score     : %d / 100\n", calculateRiskScore());
    printf("Primary Bank        : %s\n",
           banks[users[currentUser].primaryBank]);

    printf("\n" GREEN BOLD);
    printf("       Thank you for using HISAAB, %s!\n",
           users[currentUser].name);
    printf(RESET);

    printf("\n              Protect today. Plan tomorrow.\n");

    printLine();
}

/* =========================================================
   USER SESSION
   ========================================================= */

int runHisaab() {
    int choice;

    showDashboard();

    do {
        printf("\n" BOLD "WHAT WOULD YOU LIKE TO DO?\n\n" RESET);

        printf("1. Add Extra Payment\n");
        printf("2. View Transactions\n");
        printf("3. My Money\n");
        printf("4. Smart Money Guardian\n");
        printf("5. Linked Banks\n");
        printf("6. Sync UPI\n");
        printf("7. Refresh Dashboard\n");
        printf("8. Logout / Switch User\n");
        printf("9. Exit HISAAB\n\n");

        printf("Choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                loadingScreen("Opening payments");
                addExtraPayment();
                break;

            case 2:
                loadingScreen("Opening transactions");
                viewTransactions();
                break;

            case 3:
                loadingScreen("Opening My Money");
                myMoney();
                break;

            case 4:
                loadingScreen("Opening Smart Money Guardian");
                smartGuardian();
                break;

            case 5:
                loadingScreen("Opening linked banks");
                linkedBanksMenu();
                break;

            case 6:
                upiAnimation();
                syncUPI();
                break;

            case 7:
                loadingScreen("Refreshing dashboard");
                showDashboard();
                break;

            case 8:
                logoutAnimation();

                saveUsers();

                printf(GREEN "Successfully logged out.\n" RESET);

                delayMs(400);
                currentUser = -1;

                return 0;

            case 9:
                loadingScreen("Preparing final summary");
                exitSummary();
                return 1;

            default:
                loadingScreen("Checking selection");
                printf(RED "Invalid option. Please try again.\n" RESET);
        }

        if (choice >= 1 && choice <= 6) {
            loadingScreen("Returning to dashboard");
            showDashboard();
        }

    } while (1);
}

/* =========================================================
   MAIN
   ========================================================= */

int main() {
    int loginResult;
    int choice;
    int exitProgram = 0;

    srand((unsigned int)time(0));

    /*
       Load previous financial information.

       If users.dat does not exist,
       create the three default accounts.
    */

    if (!loadUsers()) {
        initializeUsers();
        loadDemoHistory();
        saveUsers();
    }
    else {
        upgradeHistoryDatabase();
    }

    /* EXACT ORIGINAL STARTUP ANIMATION */
    startupAnimation();

    /*
       Program remains active so a user
       can logout and another user can login.
    */

    while (!exitProgram) {
        loginResult = login();

        if (loginResult == 1)
            exitProgram = runHisaab();

        else {
            printf("\n1. Try Login Again\n");
            printf("2. Exit HISAAB\n\n");

            printf("Choice: ");
            scanf("%d", &choice);

            loadingScreen("Processing selection");

            if (choice == 2)
                exitProgram = 1;
        }
    }

    saveUsers();

    /* The full finale plays exactly once at the true end of HISAAB. */
    finalExitAnimation();

    return 0;
}