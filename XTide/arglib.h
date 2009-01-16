/* arglib.h - external declarations for arglib.c */

#define MAXARGLEN 256

extern int moreargs ();
extern int dataarg ();
extern char *poparg ();
extern void push_command_line (int, char **);
extern int push_config_file (char *);
