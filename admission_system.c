/*
 * Student Admission System
 * Features: Register, Search, View All, Delete, Save/Load from file
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_STUDENTS 200
#define DATA_FILE    "students.dat"

/* ─── Data Structure ─────────────────────────────────────────────────────── */

typedef struct {
    int    id;
    char   name[60];
    int    age;
    char   department[40];
    float  marks;          /* entrance score out of 100 */
    char   status[12];     /* "Admitted" or "Waitlisted" */
} Student;

/* ─── Globals ────────────────────────────────────────────────────────────── */

Student students[MAX_STUDENTS];
int     count = 0;
int     next_id = 1001;    /* auto-incremented ID */

/* ─── Utilities ──────────────────────────────────────────────────────────── */

void clear_input(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void separator(char ch, int width) {
    for (int i = 0; i < width; i++) putchar(ch);
    putchar('\n');
}

void to_upper_first(char *str) {
    if (!str || str[0] == '\0') return;
    str[0] = toupper((unsigned char)str[0]);
    for (int i = 1; str[i]; i++)
        str[i] = tolower((unsigned char)str[i]);
}

/* determine admission status based on entrance score */
void set_status(Student *s) {
    if (s->marks >= 60.0f)
        strcpy(s->status, "Admitted");
    else
        strcpy(s->status, "Waitlisted");
}

/* ─── File Handling ──────────────────────────────────────────────────────── */

void save_to_file(void) {
    FILE *fp = fopen(DATA_FILE, "wb");
    if (!fp) {
        printf("  [!] Could not open file for saving.\n");
        return;
    }
    fwrite(&count,    sizeof(int), 1,     fp);
    fwrite(&next_id,  sizeof(int), 1,     fp);
    fwrite(students,  sizeof(Student), count, fp);
    fclose(fp);
    printf("  [✓] Data saved to %s\n", DATA_FILE);
}

void load_from_file(void) {
    FILE *fp = fopen(DATA_FILE, "rb");
    if (!fp) return;   /* first run — no file yet */

    int loaded_count = 0;
    int loaded_next_id = 1001;

    if (fread(&loaded_count, sizeof(int), 1, fp) != 1 ||
        fread(&loaded_next_id, sizeof(int), 1, fp) != 1) {
        printf("  [!] Data file is incomplete; starting with an empty registry.\n");
        fclose(fp);
        return;
    }

    if (loaded_count < 0 || loaded_count > MAX_STUDENTS || loaded_next_id < 1001) {
        printf("  [!] Data file contains invalid metadata; starting with an empty registry.\n");
        fclose(fp);
        return;
    }

    if (fread(students, sizeof(Student), (size_t)loaded_count, fp) != (size_t)loaded_count) {
        printf("  [!] Data file is truncated; starting with an empty registry.\n");
        memset(students, 0, sizeof(students));
        fclose(fp);
        return;
    }

    count = loaded_count;
    next_id = loaded_next_id;
    fclose(fp);
    printf("  [✓] Loaded %d record(s) from %s\n", count, DATA_FILE);
}

/* ─── Core Features ──────────────────────────────────────────────────────── */

void register_student(void) {
    if (count >= MAX_STUDENTS) {
        printf("  [!] Admission registry is full (%d students).\n", MAX_STUDENTS);
        return;
    }

    Student s;
    s.id = next_id++;

    separator('-', 50);
    printf("  NEW STUDENT REGISTRATION  [ID: %d]\n", s.id);
    separator('-', 50);

    /* Name */
    printf("  Full name       : ");
    fgets(s.name, sizeof(s.name), stdin);
    s.name[strcspn(s.name, "\n")] = '\0';
    if (strlen(s.name) == 0) {
        printf("  [!] Name cannot be empty. Registration cancelled.\n");
        next_id--;
        return;
    }
    to_upper_first(s.name);

    /* Age */
    while (1) {
        printf("  Age             : ");
        if (scanf("%d", &s.age) == 1 && s.age >= 15 && s.age <= 60) {
            clear_input();
            break;
        }
        printf("  [!] Enter a valid age (15–60).\n");
        clear_input();
    }

    /* Department */
    printf("  Department      : ");
    fgets(s.department, sizeof(s.department), stdin);
    s.department[strcspn(s.department, "\n")] = '\0';
    if (strlen(s.department) == 0) strcpy(s.department, "Undecided");
    to_upper_first(s.department);

    /* Entrance Score */
    while (1) {
        printf("  Entrance score  : ");
        if (scanf("%f", &s.marks) == 1 && s.marks >= 0.0f && s.marks <= 100.0f) {
            clear_input();
            break;
        }
        printf("  [!] Score must be between 0 and 100.\n");
        clear_input();
    }

    set_status(&s);
    students[count++] = s;
    save_to_file();

    printf("\n  [✓] Student registered successfully!\n");
    printf("      Status : %s\n", s.status);
    if (strcmp(s.status, "Waitlisted") == 0)
        printf("      [i] Score below 60 — placed on waitlist.\n");
}

void search_by_id(void) {
    separator('-', 50);
    printf("  SEARCH BY STUDENT ID\n");
    separator('-', 50);

    int id;
    printf("  Enter student ID : ");
    if (scanf("%d", &id) != 1) {
        printf("  [!] Invalid ID.\n");
        clear_input();
        return;
    }
    clear_input();

    for (int i = 0; i < count; i++) {
        if (students[i].id == id) {
            Student *s = &students[i];
            printf("\n");
            separator('=', 50);
            printf("  ID         : %d\n",   s->id);
            printf("  Name       : %s\n",   s->name);
            printf("  Age        : %d\n",   s->age);
            printf("  Department : %s\n",   s->department);
            printf("  Score      : %.1f / 100\n", s->marks);
            printf("  Status     : %s\n",   s->status);
            separator('=', 50);
            return;
        }
    }
    printf("  [!] No student found with ID %d.\n", id);
}

void search_by_name(void) {
    separator('-', 50);
    printf("  SEARCH BY NAME\n");
    separator('-', 50);

    char query[60];
    printf("  Enter name (or part of it) : ");
    fgets(query, sizeof(query), stdin);
    query[strcspn(query, "\n")] = '\0';

    /* convert query to lowercase for case-insensitive match */
    char lq[60];
    for (int i = 0; query[i]; i++) lq[i] = tolower((unsigned char)query[i]);
    lq[strlen(query)] = '\0';

    int found = 0;
    for (int i = 0; i < count; i++) {
        char ln[60];
        for (int j = 0; students[i].name[j]; j++)
            ln[j] = tolower((unsigned char)students[i].name[j]);
        ln[strlen(students[i].name)] = '\0';

        if (strstr(ln, lq)) {
            Student *s = &students[i];
            printf("  [%d] %-25s | Dept: %-15s | Score: %5.1f | %s\n",
                   s->id, s->name, s->department, s->marks, s->status);
            found++;
        }
    }
    if (!found) printf("  [!] No match found for \"%s\".\n", query);
}

void view_all(void) {
    separator('=', 70);
    printf("  ALL STUDENTS (%d registered)\n", count);
    separator('=', 70);

    if (count == 0) {
        printf("  No students registered yet.\n");
        separator('=', 70);
        return;
    }

    printf("  %-6s %-25s %-5s %-15s %-7s %-12s\n",
           "ID", "Name", "Age", "Department", "Score", "Status");
    separator('-', 70);

    for (int i = 0; i < count; i++) {
        Student *s = &students[i];
        printf("  %-6d %-25s %-5d %-15s %-7.1f %-12s\n",
               s->id, s->name, s->age, s->department, s->marks, s->status);
    }
    separator('=', 70);
    printf("  Admitted: ");
    int admitted = 0, waitlisted = 0;
    for (int i = 0; i < count; i++) {
        if (strcmp(students[i].status, "Admitted") == 0) admitted++;
        else waitlisted++;
    }
    printf("%d   Waitlisted: %d\n", admitted, waitlisted);
    separator('=', 70);
}

void delete_student(void) {
    separator('-', 50);
    printf("  DELETE STUDENT RECORD\n");
    separator('-', 50);

    int id;
    printf("  Enter student ID to delete : ");
    if (scanf("%d", &id) != 1) {
        printf("  [!] Invalid ID.\n");
        clear_input();
        return;
    }
    clear_input();

    for (int i = 0; i < count; i++) {
        if (students[i].id == id) {
            char confirm;
            printf("  Delete %s (ID %d)? (y/n) : ", students[i].name, id);
            scanf(" %c", &confirm);
            clear_input();
            if (tolower(confirm) != 'y') {
                printf("  [!] Deletion cancelled.\n");
                return;
            }
            /* shift records left */
            for (int j = i; j < count - 1; j++)
                students[j] = students[j + 1];
            count--;
            save_to_file();
            printf("  [✓] Record deleted successfully.\n");
            return;
        }
    }
    printf("  [!] No student found with ID %d.\n", id);
}

void department_summary(void) {
    separator('=', 50);
    printf("  DEPARTMENT SUMMARY\n");
    separator('=', 50);

    if (count == 0) {
        printf("  No records available.\n");
        return;
    }

    char depts[MAX_STUDENTS][40];
    int  dcounts[MAX_STUDENTS];
    float dtotal[MAX_STUDENTS];
    int  nd = 0;

    for (int i = 0; i < count; i++) {
        int found = 0;
        for (int j = 0; j < nd; j++) {
            if (strcmp(depts[j], students[i].department) == 0) {
                dcounts[j]++;
                dtotal[j] += students[i].marks;
                found = 1;
                break;
            }
        }
        if (!found) {
            strcpy(depts[nd], students[i].department);
            dcounts[nd] = 1;
            dtotal[nd]  = students[i].marks;
            nd++;
        }
    }

    printf("  %-20s  %-8s  %-10s\n", "Department", "Students", "Avg Score");
    separator('-', 50);
    for (int i = 0; i < nd; i++)
        printf("  %-20s  %-8d  %.1f\n", depts[i], dcounts[i], dtotal[i] / dcounts[i]);
    separator('=', 50);
}

/* ─── Menu ───────────────────────────────────────────────────────────────── */

void print_menu(void) {
    printf("\n");
    separator('═', 50);
    printf("   STUDENT ADMISSION SYSTEM\n");
    separator('═', 50);
    printf("  [1] Register new student\n");
    printf("  [2] Search by ID\n");
    printf("  [3] Search by name\n");
    printf("  [4] View all students\n");
    printf("  [5] Delete a record\n");
    printf("  [6] Department summary\n");
    printf("  [0] Exit\n");
    separator('─', 50);
    printf("  Choice : ");
}

int main(void) {
    load_from_file();

    int choice;
    do {
        print_menu();
        if (scanf("%d", &choice) != 1) {
            clear_input();
            choice = -1;
        } else {
            clear_input();
        }

        printf("\n");

        switch (choice) {
            case 1: register_student();    break;
            case 2: search_by_id();        break;
            case 3: search_by_name();      break;
            case 4: view_all();            break;
            case 5: delete_student();      break;
            case 6: department_summary();  break;
            case 0: printf("  Goodbye!\n"); break;
            default: printf("  [!] Invalid choice. Enter 0–6.\n");
        }

    } while (choice != 0);

    return 0;
}