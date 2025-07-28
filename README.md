# Console-Based Notepad Application

This repository contains the source code and documentation for a console-based notepad application, developed as a major assignment. The project was built from the ground up in C++, with a core focus on implementing complex editor functionalities using fundamental data structures without relying on standard libraries.

## Project Overview

The application is a fully functional text editor that runs in the Windows console. It supports character-by-character text insertion, deletion, complex cursor navigation, and word wrapping. The central data structure is a custom **two-dimensional (2D) doubly linked list**, which allows for a flexible and dynamic text buffer.

## Key Features

-   **Dynamic Text Editing**: Insert and delete characters anywhere in the document. Text reflows automatically.
-   **Word Wrap**: Words that exceed the console width are automatically moved to the next line, preventing words from being split.
-   **2D Cursor Navigation**: Full cursor control using the arrow keys (`Up`, `Down`, `Left`, `Right`) to navigate the text grid.
-   **Undo/Redo Functionality**: A stack-based implementation tracks the last 5 word-based actions (insertions/deletions) for undo (`Ctrl+Z`) and redo (`Ctrl+Y`).
-   **File Operations**: Users can create new files, load existing files, and save their work. The program automatically creates a new file if a specified file doesn't exist on load.
-   **Custom Data Structures**: The entire application is built on custom-implemented data structures, including the 2D linked list and stacks, as per assignment constraints.

## Data Structures Implemented

-   **2D Doubly Linked List**: Each `Node` holds a character and pointers to its four neighbors (`up`, `down`, `left`, `right`), forming a dynamic grid that represents the document.
-   **Stack (via Linked List)**: Two stacks are used to manage the history for the undo and redo operations, storing word-level changes.

## Repository Contents

-   `CursorMovement.cpp`: The complete C++ source code for the notepad application, including the main function.
-   `Assignment_Statement.pdf`: The original assignment specification document.
-   `Report.pdf`: The detailed project report explaining the design, data structures, and implementation.
-   `README.md`: This file, providing an overview of the project.

## How to Compile and Run

This project was developed using **Visual Studio 2022**.

1.  Create a new C++ Console App project in Visual Studio.
2.  Replace the contents of the main `.cpp` file with the code from `CursorMovement.cpp`.
3.  Compile and run the project (F5).
