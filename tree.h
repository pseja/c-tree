#ifndef TREE_H
#define TREE_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <stdbool.h>
#include "ANSI-color-codes.h"

/**
 * @brief A structure to hold parameters.
 */
typedef struct t_parameters Parameters;

/**
 * @brief Function to get the size of a file.
 * @param file_path Path to the file.
 * @return Size of the file.
 */
int getFileSize(const char *file_path);

/**
 * @brief Function to convert a string to lower case.
 * @param str String to be converted.
 */
void strToLower(char *str);

/**
 * @brief Function to color and print special names.
 * @param file_name Name of the file.
 * @return Status of the operation.
 */
int colorAndPrintSpecialNames(char *file_name);

/**
 * @brief Function to print object name from path.
 * @param path Path to the object.
 * @param de Directory entry structure.
 * @param params Parameters structure.
 */
void printObjectNameFromPath(char *path, struct dirent *de, Parameters *params);

/**
 * @brief Function to count files in a directory.
 * @param dr Directory structure.
 * @return Number of files in the directory.
 */
int countFilesInDirectory(DIR *dr, Parameters *params);

/**
 * @brief Function to print the indentation for a file.
 * @param depth The current depth in the directory tree.
 * @param last_at_depth An array indicating whether the last file at each depth has been processed.
 * @param params The parameters structure.
 */
void printFileIndent(int depth, int *last_at_depth, Parameters *params);

/**
 * @brief Function to print the prefix for a file, including a pipe character if necessary.
 * @param current_file The index of the current file in the directory.
 * @param file_count The total number of files in the directory.
 * @param params The parameters structure.
 */
void printFilePrefixPipe(int current_file, int file_count, Parameters *params);

/**
 * @brief Function to go through files in a directory.
 * @param root_path Path to the root directory.
 * @param indent Indentation level.
 * @param last_at_depth Last depth level.
 * @param depth Current depth level.
 * @param params Parameters structure.
 * @return Status of the operation.
 */
int goThroughFiles(char *root_path, int indent, int *last_at_depth, int depth, Parameters *params);

/**
 * @brief Function to handle parameters.
 * @param argc Argument count.
 * @param argv Argument vector.
 * @param params Parameters structure.
 * @return Status of the operation.
 */
int handleParameters(int argc, char **argv, Parameters *params);

/**
 * @brief Function to setup the tree program.
 * @param argc Argument count.
 * @param argv Argument vector.
 * @param params Parameters structure.
 * @return Status of the operation.
 */
int treeSetup(int argc, char **argv, Parameters *params);

/**
 * @brief Function to initialize parameters.
 * @return Initialized parameters structure.
 */
Parameters initializeParameters();

#endif // !TREE_H
