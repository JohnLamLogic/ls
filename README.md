# LS25(1)                               User Commands                                LS25(1)

# Name
ls25 - directory listing tool

# SYNOPSIS
**./ls25 [-aFs]** *directory_path*

# DESCRIPTION
The **ls25** utility is a simple command-line tool that lists the contents of a specified directory. It provides information about files and subdirectories, including their size, owner, group, and last modification time. It supports several options to customize the output, such as showing hidden files, displaying file type indicators, and sorting the results.

# FUNCTIONS
- **file_type_chcode(const struct stat \*st)**
  - Returns a single-character code representing the file type.
  - Codes used:
    - `/`  Directory
    - `@`  Symbolic link
    - `|`  Named pipe (FIFO)
    - `*`  Executable file (any execute bit set)
    - ` `  Regular file

- **readDirectory(const char \*path, int \*num)**
  - Opens the specified directory and reads all entries twice: first to count them, then to allocate memory and populate an array of `struct dentry`.
  - For each entry, it gathers metadata with **lstat(2)** including size, owner, group, last modification time, and file type code.
  - Returns a pointer to the allocated array and writes the number of entries into *num*.

- **cmp_dname(const void \*a, const void \*b)**
  - Comparison function for **qsort(3)** that sorts two `struct dentry` objects alphabetically by their `dname` field.

# OPTIONS
- **-a**
  - Display all directory entries, including those that start with a dot (`.`), which are normally hidden. Equivalent to `ls -a`.

- **-F**
  - Append a character to the filename to indicate file type:
    - `/` for directories
    - `@` for symbolic links
    - `|` for named pipes (FIFOs)
    - `*` for executable files
    - ` ` for regular files
  - Equivalent to `ls -F`.

- **-s**
  - Sort directory entries alphabetically by name using **qsort(3)**.

# EXAMPLES
- List the contents of the current directory, including hidden files:
  - `./ls25 -a .`

- List the contents of `/usr/bin` and sort alphabetically:
  - `./ls25 -s /usr/bin`

- List the contents of `/home/user/documents`, show file type indicators, and include hidden files:
  - `./ls25 -aF /home/user/documents`

# NOTES
- **ls25** processes a single directory path as its argument.
- If the specified directory cannot be accessed, an error message is printed to `stderr` and the program exits.
- Uses **lstat(2)** so symbolic links are identified without being followed.
