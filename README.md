# OS shell

To run shell program, simply compile main.c using cc or gcc. Then run. 

The program will then prompt for a unix command ( exec() ). Additionally, this program supports functionality for when commands may be concatenated with a '&' at the end, indicating the parent process should NOT wait for the child to finish.

The program will prompt for commands until "exit" is entered. This will be counted as a command in the command counter displayed at the end of the program.
