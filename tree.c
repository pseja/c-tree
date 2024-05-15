#include "tree.h"

struct t_parameters
{
    bool recursive;
    bool lsF;
    bool dont_indent;
    bool full_path;
    bool directories_only;
    bool print_file_size;
    bool ascii_pipes;
    bool print_hidden;
};

int getFileSize(const char *file_path)
{
    FILE *fp = fopen(file_path, "rb");
    if (fp == NULL)
    {
        fprintf(stderr, RED "Failed to read %s\n" COLOR_RESET, file_path);
        return -1;
    }

    struct stat buf;
    int fd = fileno(fp);
    fstat(fd, &buf);
    off_t size = buf.st_size;

    if (fclose(fp) == EOF)
    {
        fprintf(stderr, RED "Failed to close %s\n" COLOR_RESET, file_path);
        return -1;
    }

    return size;
}

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

    char object_path[4096];
    if (params->full_path)
    {
        strcpy(object_path, path);
    }
    else
    {
        strcpy(object_path, last_slash + 1);
    }

    if (params->print_file_size)
    {
        int file_size = getFileSize(path);
        if (file_size != -1)
        {
            printf("[%*d] ", 11, file_size);
        }
    }

    switch (de->d_type)
    {
    case DT_DIR: // directory
        if (params->lsF)
        {
            strcat(object_path, "/");
        }
        printf(BLU "%s\n" COLOR_RESET, object_path);
        break;
    case DT_FIFO: // named pipe or FIFO
        if (params->lsF)
        {
            strcat(object_path, "|");
        }
        printf(MAG "%s\n" COLOR_RESET, object_path);
        break;
    case DT_SOCK: // local-domain socket
        if (params->lsF)
        {
            strcat(object_path, "=");
        }
        printf(RED "%s\n" COLOR_RESET, object_path);
        break;
    case DT_CHR: // character device
        printf(GRN "%s\n" COLOR_RESET, object_path);
        break;
    case DT_BLK: // block device
        printf(YEL "%s\n" COLOR_RESET, object_path);
        break;
    case DT_LNK: // symbolic link
        printf(CYN "%s\n" COLOR_RESET, object_path);
        break;

    default:
        if (colorAndPrintSpecialNames(object_path) != 0)
        {
            printf("%s\n", object_path);
        }
        break;
    }
}

int countFilesInDirectory(DIR *dr, Parameters *params)
{
    struct dirent *de;
    int file_count = 0;

    while ((de = readdir(dr)) != NULL)
    {
        if ((strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0) || (!params->print_hidden && de->d_name[0] == '.'))
        {
            continue;
        }

        file_count++;
    }
    rewinddir(dr);

    return file_count;
}

void printFileIndent(int depth, int *last_at_depth, Parameters *params)
{
    if (!params->dont_indent)
    {
        char *vertical_pipe_type = params->ascii_pipes ? "|  " : "┃  ";

        for (int i = 0; i < depth - 1; i++)
        {
            printf(last_at_depth[i] ? "  " : BLK "%s" COLOR_RESET, vertical_pipe_type);
        }
    }
}

void printFilePrefixPipe(int current_file, int file_count, Parameters *params)
{
    if (!params->dont_indent)
    {
        char *pipes_type = params->ascii_pipes ? "+- " : (current_file == file_count ? "┗━ " : "┣━ ");
        printf(BLK "%s" COLOR_RESET, pipes_type);
    }
}

int goThroughFiles(char *root_path, int indent, int *last_at_depth, int depth, Parameters *params)
{
    indent += 2;
    depth++;

    struct dirent *de;
    int current_file = 0;
    int err_code = 0;

    DIR *dr = opendir(root_path);
    if (dr == NULL)
    {
        fprintf(stderr, RED "Could not open '%s' directory\n" COLOR_RESET, root_path);
        return -1;
    }

    int file_count = countFilesInDirectory(dr, params);

    static bool printed_root_dir = false;
    if (!printed_root_dir)
    {
        printf(BLU "%s\n" COLOR_RESET, root_path);
        printed_root_dir = true;
    }

    while ((de = readdir(dr)) != NULL)
    {
        if ((strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0) || (!params->print_hidden && de->d_name[0] == '.'))
        {
            continue;
        }

        char *newPath = malloc(strlen(root_path) + strlen(de->d_name) + 2);
        if (newPath == NULL)
        {
            fprintf(stderr, RED "Failed to allocate memory for new path\n" COLOR_RESET);
            return -1;
        }
        strcpy(newPath, root_path);
        strcat(newPath, "/");
        strcat(newPath, de->d_name);

        current_file++;

        printFileIndent(depth, last_at_depth, params);
        printFilePrefixPipe(current_file, file_count, params);
        printObjectNameFromPath(newPath, de, params);

        if (current_file == file_count)
        {
            last_at_depth[depth - 1] = 1;
        }

        if (de->d_type == DT_DIR && params->recursive)
        {
            err_code = goThroughFiles(newPath, indent, last_at_depth, depth, params);
            last_at_depth[depth] = 0;
        }

        free(newPath);
    }

    closedir(dr);

    return err_code;
}

int handleParameters(int argc, char **argv, Parameters *params)
{
    int option = 0;
    char *options = "arifsAF";

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
        case 'f':
            params->full_path = true;
            break;
        case 's':
            params->print_file_size = true;
            break;
        case 'A':
            params->ascii_pipes = true;
            break;
        case 'a':
            params->print_hidden = true;
            break;

        default:
            fprintf(stderr, "Usage: %s [-%s] [PATH ...]\n", argv[0], options);
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

    int err_code = goThroughFiles(path, -2, last_at_depth, 0, params);

    free(last_at_depth);

    return err_code;
}

Parameters initializeParameters()
{
    Parameters params;

    params.directories_only = false;
    params.dont_indent = false;
    params.full_path = false;
    params.lsF = false;
    params.print_file_size = false;
    params.recursive = false;
    params.ascii_pipes = false;
    params.print_hidden = false;

    return params;
}

int main(int argc, char **argv)
{
    Parameters params = initializeParameters();

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
