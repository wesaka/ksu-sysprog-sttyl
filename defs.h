/*
 * defs.h - function declarations, and file descriptor definitions
 */

#ifndef FD
#define FD 0	// Assume standard input for all calls to tcsetattr()
#endif

#ifndef DEFS_H
#define DEFS_H
/* speed.c */
int valid_speed(char *av );
void set_speed( struct termios *ttyopts, int i );
void print_speed( struct termios *ttyopts );
/* control_flag.c */
int valid_control(char *av );
int set_control(struct termios *ttyopts, int i, char *c);
void print_control( struct termios *ttyopts );
/* setting.c */
int valid_setting(char *av );
void set_setting( struct termios *ttyopts, int i, int on );
void print_setting( struct termios *ttyopts );
#endif

