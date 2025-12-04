#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DB_FILE "student_records.dat"
#define TEMP_FILE "temp_records.dat"
#define MAX_NAME 50
#define MAX_COURSE 50

typedef struct 
{
    int id;
    char fullName[MAX_NAME];
    char course[MAX_COURSE];
    float gpa;
} Student;

/*
 * -----------------------------------------------------------------------------
 * Function: clear_input
 * Description: Clears the input buffer to ensure scanning functions don't 
 * read leftover newline characters.
 * -----------------------------------------------------------------------------
 */
void clear_input()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/*
 * -----------------------------------------------------------------------------
 * Function: remove_newline
 * Description: Utility to strip the trailing newline character from strings
 * read via fgets().
 * -----------------------------------------------------------------------------
 */
void remove_newline(char* str)
{
    char *pos;
    if ((pos = strchr(str, '\n')) != NULL)
        *pos = '\0';
}

/*
 * -----------------------------------------------------------------------------
 * Function: add_student
 * Description: Collects student details from the user and appends a new 
 * binary record to the database file.
 * -----------------------------------------------------------------------------
 */
void add_student()
{
    FILE *fp = fopen(DB_FILE, "ab");
    if (!fp)
    {
        printf("!! Error: Could not access database file.\n");
        return;
    }

    Student s;
    printf("\n--- ENROLL NEW STUDENT ---\n");
    
    printf("Student ID: ");
    scanf("%d", &s.id);
    clear_input(); 

    printf("Full Name:  ");
    fgets(s.fullName, MAX_NAME, stdin);
    remove_newline(s.fullName);

    printf("Course:     ");
    fgets(s.course, MAX_COURSE, stdin);
    remove_newline(s.course);

    printf("GPA:        ");
    scanf("%f", &s.gpa);
    clear_input();

    fwrite(&s, sizeof(Student), 1, fp);
    fclose(fp);
    
    printf(">> Record saved successfully.\n");
}

/*
 * -----------------------------------------------------------------------------
 * Function: list_students
 * Description: Reads all records from the binary file and displays them in
 * a formatted table.
 * -----------------------------------------------------------------------------
 */
void list_students()
{
    FILE *fp = fopen(DB_FILE, "rb");
    if (!fp)
    {
        printf("\n>> No records found. Database is empty.\n");
        return;
    }

    Student s;
    printf("\n%-10s | %-25s | %-20s | %s\n", "ID", "Name", "Course", "GPA");
    printf("----------------------------------------------------------------------\n");

    while (fread(&s, sizeof(Student), 1, fp))
    {
        printf("%-10d | %-25s | %-20s | %.2f\n", s.id, s.fullName, s.course, s.gpa);
    }
    fclose(fp);
    printf("----------------------------------------------------------------------\n");
}

/*
 * -----------------------------------------------------------------------------
 * Function: search_student
 * Description: Prompts for a Student ID and scans the file for a matching
 * record, displaying it if found.
 * -----------------------------------------------------------------------------
 */
void search_student()
{
    FILE *fp = fopen(DB_FILE, "rb");
    if (!fp)
    {
        printf("\n>> Database inaccessible.\n");
        return;
    }

    int targetId;
    int found = 0;
    Student s;

    printf("\nEnter Student ID to Search: ");
    scanf("%d", &targetId);
    clear_input();

    while (fread(&s, sizeof(Student), 1, fp))
    {
        if (s.id == targetId)
        {
            printf("\n>> RECORD FOUND <<\n");
            printf("Name:   %s\n", s.fullName);
            printf("ID:     %d\n", s.id);
            printf("Course: %s\n", s.course);
            printf("GPA:    %.2f\n", s.gpa);
            found = 1;
            break;
        }
    }

    if (!found)
    {
        printf(">> No student found with ID %d.\n", targetId);
    }
    
    fclose(fp);
}

/*
 * -----------------------------------------------------------------------------
 * Function: delete_student
 * Description: Removes a student record by ID. It copies all non-matching
 * records to a temporary file, deletes the original, and renames the temp.
 * -----------------------------------------------------------------------------
 */
void delete_student()
{
    FILE *fp = fopen(DB_FILE, "rb");
    FILE *tempFp = fopen(TEMP_FILE, "wb");
    
    if (!fp || !tempFp)
    {
        printf("!! Error: Could not access database or create temp file.\n");
        if(fp) fclose(fp);
        if(tempFp) fclose(tempFp);
        return;
    }

    int targetId;
    int found = 0;
    Student s;

    printf("\n--- DELETE STUDENT ---\n");
    printf("Enter Student ID to Delete: ");
    scanf("%d", &targetId);
    clear_input();

    while (fread(&s, sizeof(Student), 1, fp))
    {
        if (s.id == targetId)
        {
            found = 1;
            printf(">> Deleting record for %s (ID: %d)...\n", s.fullName, s.id);
        }
        else
        {
            fwrite(&s, sizeof(Student), 1, tempFp);
        }
    }

    fclose(fp);
    fclose(tempFp);

    if (found)
    {
        remove(DB_FILE);
        rename(TEMP_FILE, DB_FILE);
        printf(">> Record successfully deleted.\n");
    }
    else
    {
        remove(TEMP_FILE);
        printf(">> No student found with ID %d. No changes made.\n", targetId);
    }
}

/*
 * -----------------------------------------------------------------------------
 * Function: main
 * Description: The main application loop handling the menu navigation and
 * user choices.
 * -----------------------------------------------------------------------------
 */
int main()
{
    int choice;

    while (1)
    {
        printf("\n============================\n");
        printf("   STUDENT ADMIN SYSTEM\n");
        printf("============================\n");
        printf("1. Add Student\n");
        printf("2. List All Students\n");
        printf("3. Search by ID\n");
        printf("4. Delete Student\n");
        printf("5. Exit\n");
        printf("Select Option: ");

        if (scanf("%d", &choice) != 1)
        {
            printf("!! Invalid Input.\n");
            clear_input();
            continue;
        }
        clear_input();

        switch (choice)
        {
            case 1:
                add_student();
                break;
            case 2:
                list_students();
                break;
            case 3:
                search_student();
                break;
            case 4:
                delete_student();
                break;
            case 5:
                printf("Exiting system. Goodbye!\n");
                return 0;
            default:
                printf(">> Unknown command.\n");
        }
    }
    return 0;
}