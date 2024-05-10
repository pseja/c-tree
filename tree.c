#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include "ANSI-color-codes.h"

char *getObjectName(char *path)
{
    char *tokenize = strdup(path);

    char *token = strtok(tokenize, "/");
    char *last_token = token;

    while (token != NULL)
    {
        last_token = token;
        token = strtok(NULL, "/");
    }

    return last_token;
}

void findAllFiles(char *root_path, int indent)
{
    indent += 4;

    struct dirent *de;

    DIR *dr = opendir(root_path);

    if (dr == NULL)
    {
        fprintf(stderr, "Could not open current directory\n");
        return;
    }

    while ((de = readdir(dr)) != NULL)
    {
        if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0)
        {
            continue;
        }

        char *newPath = malloc(strlen(root_path) + strlen(de->d_name) + 2);
        strcpy(newPath, root_path);
        strcat(newPath, "/");
        strcat(newPath, de->d_name);

        if (de->d_type == DT_DIR)
        {
            printf(GRN "%*s%s\n" COLOR_RESET, indent, "", getObjectName(newPath));
            findAllFiles(newPath, indent);
        }
        else
        {
            printf(YEL "%*s%s\n" COLOR_RESET, indent, "", getObjectName(newPath));
        }

        free(newPath);
    }

    closedir(dr);
}

int main()
{
    setbuf(stdout, NULL);
    findAllFiles(".", -4);

    return 0;
}