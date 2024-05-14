# Tree
Recursive directory listing program with depth-indented listing of files in C

## Usage
`make`
`./tree [-r] [PATH ...]`
- if no path is specified, default path is used, which is path to the current directory

### Options
`-r` -> Recursively walk down the directory file tree
`-F` -> Append a '/' for directories, a '=' for socket files and a '|' for FIFO's, same as `ls -F`
