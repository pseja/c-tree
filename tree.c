#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include "ANSI-color-codes.h"

char *getObjectName(char *path, struct dirent *de)
{
    char *tokenize = strdup(path);

    char *token = strtok(tokenize, "/");
    char *last_token = token;

    while (token != NULL)
    {
        last_token = token;
        token = strtok(NULL, "/");
    }

    if (de->d_type == DT_DIR)
    {
        strcat(last_token, "/");
    }

    return last_token;
}

void findAllFiles(char *root_path, int indent, int *lastAtDepth, int depth)
{
    indent += 2;
    depth++;

    struct dirent *de;
    int file_count = 0;
    int current_file = 0;

    DIR *dr = opendir(root_path);

    if (dr == NULL)
    {
        fprintf(stderr, "Could not open current directory\n");
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

        for (int i = 0; i < depth - 1; i++)
        {
            if (lastAtDepth[i])
            {
                printf("  ");
            }
            else
            {
                printf("┃  ");
            }
        }

        if (current_file == file_count)
        {
            printf("┗━ "
                   "%s\n",
                   getObjectName(newPath, de));
            lastAtDepth[depth - 1] = 1;
        }
        else
        {
            printf("┣━ "
                   "%s\n",
                   getObjectName(newPath, de));
        }

        if (de->d_type == DT_DIR)
        {
            findAllFiles(newPath, indent, lastAtDepth, depth);
            lastAtDepth[depth] = 0;
        }

        free(newPath);
    }

    closedir(dr);
}

int main()
{
    setbuf(stdout, NULL);
    int *lastAtDepth = calloc(256, sizeof(int));
    if (lastAtDepth == NULL)
    {
        fprintf(stderr, "Failed to allocate memory\n");
        return -1;
    }

    printf(".\n");
    findAllFiles(".", -2, lastAtDepth, 0);

    free(lastAtDepth);

    return 0;
}