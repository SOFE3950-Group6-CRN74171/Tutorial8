# SOFE 3950U: Operating Systems

## TUTORIAL #8: Signals and Data Structures Part II

### Objectives
- Learn the fundamentals of signals and data structures in C
- Gain experience writing multiprocessor code and data structures

### Important Notes
- Work in groups of four students
- All reports must be submitted as a PDF on blackboard, if source code is included submit everything as an archive (e.g. zip, tar.gz)
- Save the file as `<tutorial_number>_<first student’s id>.pdf` (e.g. tutorial8_100123456.pdf)
  If you cannot submit the document on Blackboard then please contact the TA (Omar Hemied) [Omar.hemied@ontariotechu.net](mailto:Omar.hemied@ontariotechu.net)

### Notice
It is recommended for this lab activity and others that you save/bookmark the following resources as they are very useful for C programming.
- [http://en.cppreference.com/w/c](http://en.cppreference.com/w/c)
- [http://www.cplusplus.com/reference/clibrary/](http://www.cplusplus.com/reference/clibrary/)
- [http://users.ece.utexas.edu/~adnan/c-refcard.pdf](http://users.ece.utexas.edu/~adnan/c-refcard.pdf)
- [http://gribblelab.org/CBootcamp](http://gribblelab.org/CBootcamp)

The following resources are helpful as you will need to use signals and data structures to complete the task.
- [http://www.gnu.org/software/libc/manual/html_node/Standard-Signals.html#Standard-Signals](http://www.gnu.org/software/libc/manual/html_node/Standard-Signals.html#Standard-Signals)
- [http://www.gnu.org/software/libc/manual/html_node/Signaling-Another-Process.html](http://www.gnu.org/software/libc/manual/html_node/Signaling-Another-Process.html)
- [http://www.gnu.org/software/libc/manual/html_node/Process-Completion.html#Process-Completion](http://www.gnu.org/software/libc/manual/html_node/Process-Completion.html#Process-Completion)
- [http://www.gnu.org/software/libc/manual/html_node/Signaling-Another-Process.html](http://www.gnu.org/software/libc/manual/html_node/Signaling-Another-Process.html)
- [http://www.thegeekstuff.com/2013/02/c-binary-tree/](http://www.thegeekstuff.com/2013/02/c-binary-tree/)
- [http://www.learn-c.org/en/Binary_trees](http://www.learn-c.org/en/Binary_trees)

### Conceptual Questions
1. What is an Abstract Data Type (ADT)?
2. Explain the difference between a queue (FIFO) and a stack (LIFO).
3. Name and briefly explain three types of data structures.
4. Explain what a binary tree is, what are some common operations of a binary tree?
5. Explain what a hash table (dictionary) is, what are common operations of a hash table?

### Application Questions

All of your programs for this activity can be completed using the template provided, where you fill in the remaining content. A makefile is not necessary, to compile your programs use the following command in the terminal. If you do not have clang then replace clang with gcc, if you are still having issues please use -std=gnu99 instead of c99.

```bash
clang -Wall -Wextra -std=c99 <program name>.c -o <program name>
```
