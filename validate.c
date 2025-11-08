/***********************************************************************
 *  File name   : validate.c
 *  Description : Validation source file for the Inverted Search project.
 *                Contains implementations for:
 *                - File size determination
 *                - File argument validation
 *                - Word index calculation
 *                - File name validation
 *                - Database format validation
 *
 *                Functions:
 *                - get_file_size()
 *                - read_and_validate_args()
 *                - get_word_index()
 *                - valid_file_name()
 *                - valid_database()
 *
 ***********************************************************************/

#include "validate.h"

/***********************************************************************
 * Function     : get_file_size
 * Description  : Returns the size of a given file in bytes.
 * Arguments    : FILE *fp - Pointer to the open file
 * Returns      : size_t   - File size in bytes
 ***********************************************************************/
size_t get_file_size(FILE *fp)
{
    size_t ptr = ftell(fp);          // Save current file pointer position
    fseek(fp, 0, SEEK_END);          // Move pointer to end of file
    size_t size = ftell(fp);         // Get file size
    fseek(fp, ptr, SEEK_SET);        // Restore original position
    return size;
}

/***********************************************************************
 * Function     : read_and_validate_args
 * Description  : Validates command-line arguments for file inputs.
 *                Checks for extension, accessibility, duplicates, 
 *                emptyness, and adds valid files to the FileList.
 * Arguments    : FileList **filelist - Linked list of files
 *                char **argv         - Command-line arguments
 *                int argc            - Argument count
 * Returns      : int (SUCCESS/FAILURE)
 ***********************************************************************/
int read_and_validate_args(FileList **filelist, char **argv, int argc)
{
    int i = 1, count = 0;

    printf("============================================================\n");
    printf("                 File Validation Summary\n");
    printf("============================================================\n");

    for (; i < argc; i++)
    {
        // Check for file extension
        if (strchr(argv[i], '.') == NULL)
        {
            fprintf(stderr, " INFO: File '%s' has no extension\n", argv[i]);
            continue;
        }
        // Validate extension (.txt only allowed)
        if (valid_file_name(argv[i]) == FAILURE)
        {
            fprintf(stderr, " INFO: File '%s' must have a .txt extension\n", argv[i]);
            continue;
        }

        // Check if file can be opened
        FILE *fp = fopen(argv[i], "r");
        if (fp == NULL)
        {
            fprintf(stderr, " INFO: File '%s' could not be opened\n", argv[i]);
            continue;
        }

        // Validate non-empty file
        if (get_file_size(fp) == 0)
        {
            fprintf(stderr, " INFO: File '%s' is empty\n", argv[i]);
            fclose(fp);
            continue;
        }
        fclose(fp);

        // Insert into file list and check duplicates
        if (fileList_insert_last(filelist, argv[i]) == DUPLICATE)
            fprintf(stderr, " INFO: File '%s' is in the list already\n", argv[i]);

        printf(" INFO: File '%s' successfully inserted in the FileList\n", argv[i]);
        count++;
    }

    // Print summary
    if (count)
        printf("\n              Valid files loaded successfully\n");
    else
        printf("\n             No valid file found in the arguments\n");

    printf("============================================================\n");
    return SUCCESS;
}

/***********************************************************************
 * Function     : get_word_index
 * Description  : Returns index based on the first character of a word.
 *                - 'A'–'Z' → 0–25
 *                - 'a'–'z' → 0–25
 *                - '0'–'9' → 26
 *                - Others  → 27
 * Arguments    : char *word - Input word
 * Returns      : char       - Index value
 ***********************************************************************/
char get_word_index(char *word)
{
    if (word[0] >= 'A' && word[0] <= 'Z')
        return word[0] - 'A';
    if (word[0] >= 'a' && word[0] <= 'z')
        return word[0] - 'a';
    if (word[0] >= '0' && word[0] <= '9')
        return 26;
    else
        return 27;
}

/***********************************************************************
 * Function     : valid_file_name
 * Description  : Validates whether a file name has a ".txt" extension.
 * Arguments    : char *filename - File name string
 * Returns      : int (SUCCESS/FAILURE)
 ***********************************************************************/
int valid_file_name(char *filename)
{
    size_t len = strlen(filename);
    if (len < 4 || strstr(filename + len - 4, ".txt") == NULL)
        return FAILURE;
    return SUCCESS;
}

/***********************************************************************
 * Function     : valid_database
 * Description  : Validates database file format.
 *                Ensures file starts and ends with a '#' character.
 * Arguments    : FILE *fp - File pointer to database file
 * Returns      : int (SUCCESS/FAILURE)
 ***********************************************************************/
int valid_database(FILE *fp)
{
    char ch;
    ch = fgetc(fp);              // First character
    if (ch != '#')
        return FAILURE;

    fseek(fp, -2, SEEK_END);     // Move to last-2 position
    ch = fgetc(fp);              // Last marker
    if (ch != '#')
        return FAILURE;

    rewind(fp);                  // Reset file pointer
    return SUCCESS;
}