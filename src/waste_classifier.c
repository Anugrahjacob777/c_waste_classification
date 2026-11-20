/*
 * INTELLIGENT WASTE CLASSIFICATION SIMULATOR USING C
 * --------------------------------------------------
 * - Rule-based (keyword matching). NOT image recognition / AI.
 * - Disposal rules may vary by location; messages are educational.
 * - Everything is keyboard driven: 1, 2, 3 ... and 0 = Back.
 *
 * Build : gcc src/waste_classifier.c -o waste_classifier
 * Run   : ./waste_classifier        (run from the project folder)
 *
 * Folders used (next to the program):
 *   data/    -> all_items.txt (about 100 items)
 *   tests/   -> test files (10 items each)
 *   export/  -> waste_summary.txt and classified_items.csv are saved here
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_ITEMS   500
#define NAME_LEN    60
#define SMALL_LIMIT 10          /* up to 10 items = small quantity */
#define ALL_ITEMS_FILE "data/all_items.txt"
#define SUMMARY_FILE   "export/waste_summary.txt"
#define CSV_FILE       "export/classified_items.csv"

/* ---------- classes ---------- */
#define PAPER    0
#define PLASTIC  1
#define GLASS    2
#define METAL    3
#define ORGANIC  4
#define EWASTE   5
#define NUM_CAT  6

/* ---------- handling types ---------- */
#define RECYCLABLE      0
#define NON_RECYCLABLE  1
#define COMPOSTABLE     2
#define HANDLE_CARE     3
#define NUM_TYPE        4

const char *typeName[NUM_TYPE] = {
    "RECYCLABLE", "NON-RECYCLABLE", "COMPOSTABLE", "HANDLE WITH CARE"
};

/* default type of each class (same order as the class numbers above) */
const int defaultType[NUM_CAT] = {
    RECYCLABLE,      /* Paper   */
    RECYCLABLE,      /* Plastic */
    RECYCLABLE,      /* Glass   */
    RECYCLABLE,      /* Metal   */
    COMPOSTABLE,     /* Organic */
    HANDLE_CARE      /* E-waste */
};

typedef struct {
    const char *name;
    const char *disposal;
    const char *smallService;  const char *smallPhone;
    const char *bulkService;   const char *bulkPhone;
} Category;

/*
 * !! Replace every XXXXXXXXXX with a verified number before submitting.
 * 1533 is the BBMP Sahaya helpline (please verify it too).
 */
const Category cat[NUM_CAT] = {
    { "Paper",
      "Keep dry, tie in bundles and give to dry-waste collector / recycler.",
      "BBMP Sahaya helpline",        "1533",
      "Local paper recycler",        "XXXXXXXXXX" },
    { "Plastic",
      "Rinse, dry and put in the DRY waste bin. Do not burn plastic.",
      "BBMP Sahaya helpline",        "1533",
      "Saahas Zero Waste",           "XXXXXXXXXX" },
    { "Glass",
      "Wrap broken glass in paper; give whole bottles to scrap dealer.",
      "BBMP Sahaya helpline",        "1533",
      "Local glass scrap dealer",    "XXXXXXXXXX" },
    { "Metal",
      "Sell or give to a scrap dealer; metal is fully recyclable.",
      "Local kabadiwala",            "XXXXXXXXXX",
      "Metal recycling unit",        "XXXXXXXXXX" },
    { "Organic",
      "Put in WET waste bin or compost at home / community compost pit.",
      "BBMP Sahaya helpline",        "1533",
      "Wet waste processing unit",   "XXXXXXXXXX" },
    { "E-waste",
      "Never put in dustbin. Hand over to an authorised e-waste recycler.",
      "E-waste collection centre",   "XXXXXXXXXX",
      "E-Parisaraa (e-waste)",       "XXXXXXXXXX" }
};

/* ---------- keyword rules (first match wins) ---------- */
typedef struct { const char *word; int category; } Rule;

const Rule rules[] = {
    /* 1. material words first */
    {"plastic", PLASTIC}, {"polythene", PLASTIC}, {"thermocol", PLASTIC},
    {"glass", GLASS},     {"paper", PAPER},       {"metal", METAL},
    {"steel", METAL},     {"aluminium", METAL},   {"aluminum", METAL},
    {"iron", METAL},      {"copper", METAL},      {"brass", METAL},
    {"tin", METAL},       {"can", METAL},         {"foil", METAL},
    /* 2. special phrases */
    {"water bottle", PLASTIC}, {"shampoo bottle", PLASTIC},
    {"tea bag", ORGANIC},
    {"pen drive", EWASTE},     {"power bank", EWASTE},
    {"e-waste", EWASTE},       {"ewaste", EWASTE},
    /* 3. paper items */
    {"newspaper", PAPER}, {"cardboard", PAPER}, {"carton", PAPER},
    {"magazine", PAPER},  {"notebook", PAPER},  {"textbook", PAPER},
    {"book", PAPER},      {"box", PAPER},       {"envelope", PAPER},
    {"brochure", PAPER},  {"pamphlet", PAPER},  {"receipt", PAPER},
    {"calendar", PAPER},  {"poster", PAPER},
    /* 4. plastic items */
    {"toy", PLASTIC},     {"straw", PLASTIC},   {"bag", PLASTIC},
    {"wrapper", PLASTIC}, {"container", PLASTIC}, {"packet", PLASTIC},
    {"bucket", PLASTIC},  {"toothbrush", PLASTIC}, {"pet", PLASTIC},
    /* 5. glass items */
    {"jar", GLASS},       {"bottle", GLASS},    {"bulb", GLASS},
    {"tube", GLASS},      {"mirror", GLASS},    {"tumbler", GLASS},
    /* 6. metal items */
    {"blade", METAL},     {"nail", METAL},      {"screw", METAL},
    {"key", METAL},       {"spoon", METAL},     {"hanger", METAL},
    {"chain", METAL},     {"wire", METAL},
    /* 7. organic items */
    {"food", ORGANIC},    {"organic", ORGANIC}, {"vegetable", ORGANIC},
    {"fruit", ORGANIC},   {"peel", ORGANIC},    {"leaf", ORGANIC},
    {"leaves", ORGANIC},  {"garden", ORGANIC},  {"egg", ORGANIC},
    {"eggshell", ORGANIC}, {"rice", ORGANIC},   {"tea", ORGANIC},
    {"coffee", ORGANIC},  {"bread", ORGANIC},   {"leftover", ORGANIC},
    {"cooked", ORGANIC},  {"tomato", ORGANIC},  {"potato", ORGANIC},
    {"onion", ORGANIC},   {"carrot", ORGANIC},  {"banana", ORGANIC},
    {"apple", ORGANIC},   {"mango", ORGANIC},   {"bone", ORGANIC},
    {"flower", ORGANIC},  {"coconut", ORGANIC},
    /* 8. e-waste items */
    {"phone", EWASTE},    {"mobile", EWASTE},   {"battery", EWASTE},
    {"charger", EWASTE},  {"laptop", EWASTE},   {"computer", EWASTE},
    {"keyboard", EWASTE}, {"mouse", EWASTE},    {"cable", EWASTE},
    {"usb", EWASTE},      {"earphone", EWASTE}, {"headphone", EWASTE},
    {"circuit", EWASTE},  {"tv", EWASTE},       {"television", EWASTE},
    {"remote", EWASTE},   {"router", EWASTE},   {"smartwatch", EWASTE},
    {"speaker", EWASTE},  {"printer", EWASTE}
};
#define NUM_RULES ((int)(sizeof(rules) / sizeof(rules[0])))

/* special words that change the handling type of an item */
const Rule typeRules[] = {
    {"broken", HANDLE_CARE}, {"bulb", HANDLE_CARE}, {"blade", HANDLE_CARE},
    {"tube", HANDLE_CARE},   {"sharp", HANDLE_CARE},
    {"polythene", NON_RECYCLABLE}, {"wrapper", NON_RECYCLABLE},
    {"straw", NON_RECYCLABLE},     {"thermocol", NON_RECYCLABLE},
    {"plastic bag", NON_RECYCLABLE}
};
#define NUM_TYPE_RULES ((int)(sizeof(typeRules) / sizeof(typeRules[0])))

/* test files shown in the File based menu */
const char *testFiles[] = {
    "tests/test_paper.txt",   "tests/test_plastic.txt",
    "tests/test_glass.txt",   "tests/test_metal.txt",
    "tests/test_organic.txt", "tests/test_ewaste.txt",
    "tests/test_edge_cases.txt"
};
#define NUM_TEST_FILES ((int)(sizeof(testFiles) / sizeof(testFiles[0])))

/* ---------- session data ---------- */
int  counts[NUM_CAT];
int  typeCounts[NUM_TYPE];
int  unknownCount = 0;
char itemName[MAX_ITEMS][NAME_LEN];
int  itemCat[MAX_ITEMS];
int  itemType[MAX_ITEMS];
int  itemTotal = 0;

/* ---------- helpers ---------- */
void clearScreen(void)
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void line(void)
{
    printf("------------------------------------------------------------\n");
}

/* reads one line, removes newline; returns 0 on end of input */
int readLine(char *buf, int size)
{
    if (fgets(buf, size, stdin) == NULL) return 0;
    buf[strcspn(buf, "\r\n")] = '\0';
    return 1;
}

/* converts to lowercase and trims spaces at both ends */
void cleanText(char *s)
{
    int i, start = 0, len;
    for (i = 0; s[i]; i++) s[i] = (char)tolower((unsigned char)s[i]);
    while (s[start] == ' ' || s[start] == '\t') start++;
    if (start > 0) {                    /* shift text left over the spaces */
        for (i = 0; s[start + i] != '\0'; i++)
            s[i] = s[start + i];
        s[i] = '\0';
    }
    len = (int)strlen(s);
    while (len > 0 && (s[len - 1] == ' ' || s[len - 1] == '\t')) s[--len] = '\0';
}

/* returns number typed, or -1 if invalid */
int readChoice(void)
{
    char buf[40];
    int n;
    printf("Enter your choice: ");
    if (!readLine(buf, sizeof(buf))) return 0;
    if (sscanf(buf, "%d", &n) != 1) return -1;
    return n;
}

void waitForEnter(void)
{
    char buf[10];
    printf("\nPress Enter to continue...");
    readLine(buf, sizeof(buf));
}

/*
 * returns 1 if 'word' appears in 'text' as a WHOLE word.
 * "bottles" matches "bottle" (plural), but "carpet" does not match "pet"
 * and "candle" does not match "can".
 */
int containsWord(const char *text, const char *word)
{
    int wl = (int)strlen(word);
    const char *p = text;
    const char *e;
    int startOk, endOk;

    while ((p = strstr(p, word)) != NULL) {
        startOk = (p == text) || !isalpha((unsigned char)p[-1]);
        e = p + wl;
        endOk = 0;
        if (!isalpha((unsigned char)e[0]))
            endOk = 1;                                   /* exact word      */
        else if (e[0] == 's' && !isalpha((unsigned char)e[1]))
            endOk = 1;                                   /* plural: bottles */
        else if (e[0] == 'e' && e[1] == 's' && !isalpha((unsigned char)e[2]))
            endOk = 1;                                   /* plural: boxes   */
        if (startOk && endOk) return 1;
        p++;
    }
    return 0;
}

/* returns class index or -1 if unknown */
int classify(const char *text)
{
    int i;
    for (i = 0; i < NUM_RULES; i++)
        if (containsWord(text, rules[i].word))
            return rules[i].category;
    return -1;
}

/* decides RECYCLABLE / NON-RECYCLABLE / COMPOSTABLE / HANDLE WITH CARE */
int getType(const char *text, int c)
{
    int i;
    for (i = 0; i < NUM_TYPE_RULES; i++)
        if (containsWord(text, typeRules[i].word))
            return typeRules[i].category;
    return defaultType[c];
}

/* classify an item, update counts, show result */
void addItem(const char *text)
{
    int c = classify(text);
    int t;

    if (c < 0) {
        unknownCount++;
        printf("  -> \"%s\" : UNKNOWN. Try a clearer name (e.g. plastic bottle).\n", text);
        return;
    }
    t = getType(text, c);
    counts[c]++;
    typeCounts[t]++;
    if (itemTotal < MAX_ITEMS) {
        strncpy(itemName[itemTotal], text, NAME_LEN - 1);
        itemName[itemTotal][NAME_LEN - 1] = '\0';
        itemCat[itemTotal] = c;
        itemType[itemTotal] = t;
        itemTotal++;
    }
    printf("  -> \"%s\"\n", text);
    printf("     Class   : %s\n", cat[c].name);
    printf("     Type    : %s\n", typeName[t]);
    printf("     Disposal: %s\n", cat[c].disposal);
}

int totalClassified(void)
{
    int i, t = 0;
    for (i = 0; i < NUM_CAT; i++) t += counts[i];
    return t;
}

void resetSession(void)
{
    int i;
    for (i = 0; i < NUM_CAT; i++)
        counts[i] = 0;
    for (i = 0; i < NUM_TYPE; i++)
        typeCounts[i] = 0;
    unknownCount = 0;
    itemTotal = 0;
}

/* ---------- summary (screen or file) ---------- */
void printSummary(FILE *out)
{
    int i;
    fprintf(out, "============================================================\n");
    fprintf(out, "           WASTE CLASSIFICATION SUMMARY\n");
    fprintf(out, "============================================================\n");
    fprintf(out, "Total items classified : %d\n", totalClassified());
    fprintf(out, "Unknown items          : %d\n\n", unknownCount);

    /* only classes / types with at least one item are shown */
    fprintf(out, "Count per class:\n");
    if (totalClassified() == 0) fprintf(out, "  (none)\n");
    for (i = 0; i < NUM_CAT; i++)
        if (counts[i] > 0)
            fprintf(out, "  %-10s : %d\n", cat[i].name, counts[i]);

    fprintf(out, "\nCount per type:\n");
    if (totalClassified() == 0) fprintf(out, "  (none)\n");
    for (i = 0; i < NUM_TYPE; i++)
        if (typeCounts[i] > 0)
            fprintf(out, "  %-17s : %d\n", typeName[i], typeCounts[i]);

    fprintf(out, "\nWaste management contacts (Bengaluru):\n");
    if (totalClassified() == 0) {
        fprintf(out, "  No items were classified.\n");
    }
    for (i = 0; i < NUM_CAT; i++) {
        if (counts[i] == 0) continue;     /* only classes with at least 1 item */
        fprintf(out, "\n[%s] - %d item(s)\n", cat[i].name, counts[i]);
        if (counts[i] <= SMALL_LIMIT) {
            fprintf(out, "  Quantity : Small (household)\n");
            fprintf(out, "  Contact  : %s - %s\n", cat[i].smallService, cat[i].smallPhone);
        } else {
            fprintf(out, "  Quantity : Large (bulk)\n");
            fprintf(out, "  Contact  : %s - %s\n", cat[i].bulkService, cat[i].bulkPhone);
        }
        fprintf(out, "  Disposal : %s\n", cat[i].disposal);
    }

    if (itemTotal > 0) {
        fprintf(out, "\nItems list:\n");
        for (i = 0; i < itemTotal; i++)
            fprintf(out, "  %d. %s -> %s (%s)\n", i + 1, itemName[i],
                    cat[itemCat[i]].name, typeName[itemType[i]]);
    }
    fprintf(out, "\nNote: Rules are predefined and educational. Actual disposal\n");
    fprintf(out, "rules may vary by location. Please verify contact numbers.\n");
    fprintf(out, "============================================================\n");
}

/* saves the summary (.txt) and the item list (.csv) into the export folder */
void saveSummary(void)
{
    FILE *fp = fopen(SUMMARY_FILE, "w");
    FILE *csv;
    int i;

    if (fp == NULL) {
        printf("Error: could not create %s\n", SUMMARY_FILE);
        printf("Make sure the 'export' folder exists and run the program\n");
        printf("from the project folder.\n");
        return;
    }
    printSummary(fp);
    fclose(fp);
    printf("Summary saved to %s\n", SUMMARY_FILE);

    csv = fopen(CSV_FILE, "w");
    if (csv == NULL) {
        printf("Error: could not create %s\n", CSV_FILE);
        return;
    }
    fprintf(csv, "item,class,type\n");
    for (i = 0; i < itemTotal; i++)
        fprintf(csv, "%s,%s,%s\n", itemName[i], cat[itemCat[i]].name,
                typeName[itemType[i]]);
    fclose(csv);
    printf("Item list saved to %s\n", CSV_FILE);
}

void summaryScreen(void)
{
    int choice;
    while (1) {
        clearScreen();
        printSummary(stdout);
        printf("\n1. Save summary to export folder\n");
        printf("2. Start a new session (clear all counts)\n");
        printf("0. Back to main menu\n");
        choice = readChoice();
        switch (choice) {
            case 1: saveSummary(); waitForEnter(); break;
            case 2: resetSession(); printf("New session started.\n"); waitForEnter(); return;
            case 0: return;
            default: printf("Invalid choice. Enter 1, 2 or 0.\n"); waitForEnter();
        }
    }
}

/* ---------- mode 1: text based ---------- */
void textMode(void)
{
    char buf[NAME_LEN + 40];

    clearScreen();
    printf("=== TEXT BASED CLASSIFICATION ===\n");
    printf("Type a waste item name, e.g. plastic bottle, bread, old phone\n");
    printf("Type  finish  to see the summary\n");
    printf("Type  back    (or 0) to return to the main menu\n");
    line();

    while (1) {
        printf("\nItem: ");
        if (!readLine(buf, sizeof(buf))) return;
        cleanText(buf);

        if (buf[0] == '\0') { printf("  Please type something.\n"); continue; }
        if (strcmp(buf, "back") == 0 || strcmp(buf, "0") == 0) return;
        if (strcmp(buf, "finish") == 0) { summaryScreen(); return; }
        addItem(buf);
    }
}

/* ---------- mode 2: file based ---------- */

/* opens a file as typed, then tries the data/ and tests/ folders */
FILE *openItemFile(const char *name)
{
    char path[160];
    FILE *fp = fopen(name, "r");
    if (fp != NULL) return fp;

    strcpy(path, "data/");
    strcat(path, name);
    fp = fopen(path, "r");
    if (fp != NULL) return fp;

    strcpy(path, "tests/");
    strcat(path, name);
    return fopen(path, "r");
}

/* reads every line of the file and classifies it; returns 1 on success */
int processFile(const char *name)
{
    char buf[NAME_LEN + 40];
    FILE *fp = openItemFile(name);
    int n = 0, choice;

    if (fp == NULL) {
        printf("\nError: cannot open '%s'.\n", name);
        printf("Run the program from the project folder (the folder that\n");
        printf("contains data/, tests/ and export/).\n");
        waitForEnter();
        return 0;
    }

    printf("\nReading '%s'...\n\n", name);
    while (fgets(buf, sizeof(buf), fp) != NULL) {
        buf[strcspn(buf, "\r\n")] = '\0';
        cleanText(buf);
        if (buf[0] == '\0') continue;       /* skip blank lines */
        addItem(buf);
        n++;
    }
    fclose(fp);
    printf("\n%d line(s) processed.\n", n);

    printf("\n1. Show summary\n0. Back to main menu\n");
    while (1) {
        choice = readChoice();
        if (choice == 1) { summaryScreen(); return 1; }
        if (choice == 0) return 1;
        printf("Invalid choice. Enter 1 or 0.\n");
    }
}

/* returns 1 if a file was processed, 0 if the user went back */
int testFileMenu(void)
{
    int i, choice;
    while (1) {
        clearScreen();
        printf("=== TEST FILES (10 items each) ===\n");
        line();
        for (i = 0; i < NUM_TEST_FILES; i++)
            printf("%d. %s\n", i + 1, testFiles[i]);
        printf("0. Back\n");
        choice = readChoice();
        if (choice == 0) return 0;
        if (choice >= 1 && choice <= NUM_TEST_FILES) {
            if (processFile(testFiles[choice - 1])) return 1;
        } else {
            printf("Invalid choice. Enter 1 to %d, or 0.\n", NUM_TEST_FILES);
            waitForEnter();
        }
    }
}

void fileMode(void)
{
    char name[100];
    int choice;

    while (1) {
        clearScreen();
        printf("=== FILE BASED CLASSIFICATION ===\n");
        printf("Each file has one waste item per line.\n");
        line();
        printf("1. All items file  (%s)\n", ALL_ITEMS_FILE);
        printf("2. Choose a test file\n");
        printf("3. Type my own file name\n");
        printf("0. Back to main menu\n");
        choice = readChoice();

        switch (choice) {
            case 1:
                if (processFile(ALL_ITEMS_FILE)) return;
                break;
            case 2:
                if (testFileMenu()) return;
                break;
            case 3:
                printf("File name (looked up as typed, then in data/ and tests/): ");
                if (!readLine(name, sizeof(name))) return;
                cleanText(name);
                if (name[0] == '\0') {
                    printf("Please type a file name.\n");
                    waitForEnter();
                } else if (processFile(name)) {
                    return;
                }
                break;
            case 0:
                return;
            default:
                printf("Invalid choice. Enter 1, 2, 3 or 0.\n");
                waitForEnter();
        }
    }
}

/* ---------- mode 3: camera / image ---------- */
void cameraMode(void)
{
    int c;
    while (1) {
        clearScreen();
        printf("=== CAMERA / IMAGE BASED CLASSIFICATION ===\n\n");
        printf("   This feature is UNDER CONSTRUCTION.\n");
        printf("   Please use Text based or File based mode.\n\n");
        printf("0. Back to main menu\n");
        c = readChoice();
        if (c == 0) return;
        printf("Invalid choice. Enter 0 to go back.\n");
        waitForEnter();
    }
}

/* ---------- main ---------- */
int main(void)
{
    int choice;

    while (1) {
        clearScreen();
        printf("============================================================\n");
        printf("     INTELLIGENT WASTE CLASSIFICATION SIMULATOR (C)\n");
        printf("============================================================\n");
        printf("Classes: Paper, Plastic, Glass, Metal, Organic, E-waste\n");
        line();
        printf("1. Text based\n");
        printf("2. File based\n");
        printf("3. Camera / Image based\n");
        printf("4. View summary\n");
        printf("0. Exit\n");
        line();
        printf("Items classified so far: %d\n", totalClassified());

        choice = readChoice();
        switch (choice) {
            case 1: textMode();      break;
            case 2: fileMode();      break;
            case 3: cameraMode();    break;
            case 4: summaryScreen(); break;
            case 0:
                printf("\nThank you! Remember: disposal rules vary by location.\n");
                return 0;
            default:
                printf("Invalid choice. Enter 1, 2, 3, 4 or 0.\n");
                waitForEnter();
        }
    }
}
