# Inode File System Simulation

This repository contains a C++ implementation of an inode file system simulation. The code simulates essential file system operations using inodes, blocks, and disk management.

## Description

The inode file system simulation mimics a simplified version of a file system's functionality. It includes the following features:

- **File Operations**: Create, open, read, write, and delete files within the simulated file system.
- **Block Management**: Allocate and manage disk blocks for file storage using an inode-based structure.
- **Directory Handling**: Organize files and directories, check for file existence, and rename files within directories.
- **Disk Management**: Allocate and free disk blocks, maintain a bit vector for block tracking, and manage disk space.

## Usage

To use the inode file system simulation, follow these steps:

1. Compile the C++ code using a compatible compiler.
2. Run the compiled executable to interact with the simulated file system.

   ```bash
   ./inode_file_system
   ```

3. Use the provided commands within the simulation to perform file operations, manage disk space, and manipulate directories.

## Features

- **File Creation**: Create new files with specified names and allocate space for their content on the disk.
- **File Read/Write**: Read from and write to existing files, managing data storage across disk blocks.
- **Directory Management**: Create, list, rename, and delete directories to organize files within the system.
- **Disk Space Management**: Track used and free disk blocks using a bit vector, allocating and freeing blocks as needed.

## Dependencies

The code relies on standard C++ libraries for file I/O, string manipulation, and basic data structures.

## Example

Here's an example of using the inode file system simulation:

1. Create a new file named "example.txt":

   ```
   create example.txt
   ```

2. Write content to the file:

   ```
   write example.txt "This is an example content."
   ```

3. Read the content of the file:

   ```
   read example.txt
   ```

4. List all files in the current directory:

   ```
   ls
   ```

5. Delete the file:

   ```
   delete example.txt
   ```

## Notes

- The simulation provides a simplified view of a file system and may not include all features of a real-world file system.
- Ensure proper understanding of file system commands and operations before using the simulation extensively.
