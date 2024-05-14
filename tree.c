#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <stdbool.h>
#include "ANSI-color-codes.h"

typedef struct t_parameters
{
    bool recursive;
    bool lsF;
    bool dont_indent;
} Parameters;

void strToLower(char *str)
{
    int i = 0;
    while (str[i] != '\0')
    {
        str[i] = tolower(str[i]);
        i++;
    }
}

int colorAndPrintSpecialNames(char *file_name)
{
    char lower_case_file_name[256];
    strcpy(lower_case_file_name, file_name);

    strToLower(lower_case_file_name);

    if (strcmp(lower_case_file_name, "makefile") == 0 ||
        strcmp(lower_case_file_name, "readme.md") == 0)
    {
        printf(UYEL "%s\n" COLOR_RESET, file_name);
        return 0;
    }

    return 1;
}

void printObjectNameFromPath(char *path, struct dirent *de, Parameters *params)
{
    char *last_slash = strrchr(path, '/');

    if (!last_slash)
    {
        printf("%s\n", path);
    }

    switch (de->d_type)
    {
    case DT_DIR: // directory
        if (params->lsF)
        {
            strcat(last_slash + 1, "/");
        }
        printf(BLU "%s\n" COLOR_RESET, last_slash + 1);
        break;
    case DT_FIFO: // named pipe or FIFO
        if (params->lsF)
        {
            strcat(last_slash + 1, "|");
        }
        printf(MAG "%s\n" COLOR_RESET, last_slash + 1);
        break;
    case DT_SOCK: // local-domain socket
        if (params->lsF)
        {
            strcat(last_slash + 1, "=");
        }
        printf(RED "%s\n" COLOR_RESET, last_slash + 1);
        break;
    case DT_CHR: // character device
        printf(GRN "%s\n" COLOR_RESET, last_slash + 1);
        break;
    case DT_BLK: // block device
        printf(YEL "%s\n" COLOR_RESET, last_slash + 1);
        break;
    case DT_LNK: // symbolic link
        printf(CYN "%s\n" COLOR_RESET, last_slash + 1);
        break;

    default:
        if (colorAndPrintSpecialNames(last_slash + 1) != 0)
        {
            printf("%s\n", last_slash + 1);
        }
        break;
    }
}

void goThroughFiles(char *root_path, int indent, int *last_at_depth, int depth, Parameters *params)
{
    indent += 2;
    depth++;

    struct dirent *de;
    int file_count = 0;
    int current_file = 0;

    DIR *dr = opendir(root_path);

    if (dr == NULL)
    {
        fprintf(stderr, RED "Could not open '%s' directory\n" COLOR_RESET, root_path);
        return;
    }

    while ((de = readdir(dr)) != NULL)
    {
        if (strcmp(de->d_name, ".") != 0 && strcmp(de->d_name, "..") != 0)
        {
            file_count++;
        }
    }

    rewinddir(dr);

    static bool print_root_dir = false;
    if (!print_root_dir)
    {
        printf(BLU "%s\n" COLOR_RESET, root_path);
        print_root_dir = true;
    }

    while ((de = readdir(dr)) != NULL)
    {
        if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0)
        {
            continue;
        }

        current_file++;

        char *newPath = malloc(strlen(root_path) + strlen(de->d_name) + 2);
        strcpy(newPath, root_path);
        strcat(newPath, "/");
        strcat(newPath, de->d_name);

        if (!params->dont_indent)
        {
            for (int i = 0; i < depth - 1; i++)
            {
                if (last_at_depth[i])
                {
                    printf("  ");
                }
                else
                {
                    printf(BLK "┃  " COLOR_RESET);
                }
            }
        }

        if (current_file == file_count)
        {
            if (!params->dont_indent)
            {
                printf(BLK "┗━ " COLOR_RESET);
            }

            printObjectNameFromPath(newPath, de, params);

            last_at_depth[depth - 1] = 1;
        }
        else
        {
            if (!params->dont_indent)
            {
                printf(BLK "┣━ " COLOR_RESET);
            }

            printObjectNameFromPath(newPath, de, params);
        }

        if (de->d_type == DT_DIR && params->recursive)
        {
            goThroughFiles(newPath, indent, last_at_depth, depth, params);
            last_at_depth[depth] = 0;
        }

        free(newPath);
    }

    closedir(dr);
}

int handleParameters(int argc, char **argv, Parameters *params)
{
    int option = 0;
    char *options = "riF";

    while ((option = getopt(argc, argv, options)) != -1)
    {
        switch (option)
        {
        case 'r':
            params->recursive = true;
            break;
        case 'i':
            params->dont_indent = true;
            break;
        case 'F':
            params->lsF = true;
            break;

        default:
            fprintf(stderr, "Usage: %s [-%s] [path]\n", argv[0], options);
            return 1;
        }
    }

    return 0;
}

int treeSetup(int argc, char **argv, Parameters *params)
{
    int *last_at_depth = calloc(256, sizeof(int));
    if (last_at_depth == NULL)
    {
        fprintf(stderr, RED "Failed to allocate memory\n" COLOR_RESET);
        return -1;
    }

    char *path = argv[argc - 1];
    if ((strcmp(argv[0], argv[argc - 1])) == 0 || argv[argc - 1][0] == '-')
    {
        strcpy(path, ".");
    }

    goThroughFiles(path, -2, last_at_depth, 0, params);

    free(last_at_depth);

    return 0;
}

int main(int argc, char **argv)
{
    Parameters params;

    if (handleParameters(argc, argv, &params) != 0)
    {
        return 1;
    }

    if (treeSetup(argc, argv, &params) != 0)
    {
        return 1;
    }
    return 0;
}