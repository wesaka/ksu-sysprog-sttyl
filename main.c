//CS4-53203: Systems Programming
//Name: Alvacir Wesley Kalatai Alberti
//Date: 02/25/2022
//AlbertiAlvacir-CS43203-sttyl-Final.txt

#include <termios.h>

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

/*
 * SPEED Section
 */
struct speed {
    speed_t baud_value;
    char *string;
};

/*
 * Standard baud rates
 * See more: https://pubs.opengroup.org/onlinepubs/7908799/xsh/termios.h.html
 *
 * This format of table goes one step further than the example given in showtty.c
 */
struct speed baud[] = {
        {B0,      "0"},
        {B50,     "50"},
        {B75,     "75"},
        {B110,    "110"},
        {B134,    "134.5"},
        {B150,    "150"},
        {B200,    "200"},
        {B300,    "300"},
        {B600,    "600"},
        {B1200,   "1200"},
        {B1800,   "1800"},
        {B2400,   "2400"},
        {B4800,   "4800"},
        {B9600,   "9600"},
        {B19200,  "19200"},
        {B38400,  "38400"},
        {0, NULL}
};

/**
 * Checks if the speed is present in the table of speeds
 *
 * @param argv The argument
 * @return The index of the baud rate in question, or -1 if not found
 */
int valid_speed(char *argv) {
    for (int i = 0; baud[i].string; i++) {
        if (strcmp(baud[i].string, argv) == 0) {
            return i;
        }
    }
    return -1;
}

/**
 * Uses termios cfsetospeed function
 * Set the output baud rate stored in *TERMIOS_P to SPEED.
 *
 * Exits with status 1 if an error is found
 *
 * @param options The array with *TERMIOS_P
 * @param i The index in the struct with the values for baud rate
 */
void set_speed(struct termios *options, int i) {
    if (cfsetospeed(options, baud[i].baud_value) == -1) {
        printf("cfsetospeed ERROR");
        exit(1);
    }

    if (tcsetattr(0, TCSANOW, options) == -1) {
        printf("tcsetattr ERROR");
        exit(1);
    }
}

/*
 * CONTROL FLAGS Section
 */
struct control_flag {
    tcflag_t tcflag;
    char *string;
};

/*
 * Control flags I'm using
 * See more: https://pubs.opengroup.org/onlinepubs/7908799/xsh/termios.h.html
 */
struct control_flag s_values[] = {
        {VERASE, "erase"},
        {VINTR,  "intr"},
        {VKILL,  "kill"},
        {VSTART, "start"},
        {VSTOP,  "stop"},
        {0, NULL}
};

/**
 * Checks if the control flag in question is valid and present in the array
 *
 * @param argv The argument
 * @return Returns the index of the flag or -1 if not found
 */
int valid_control(char *argv) {
    for (int i = 0; s_values[i].string; i++) {
        if (strcmp(s_values[i].string, argv) == 0) {
            return i;
        }
    }
    return -1;
}

/**
 * Sets the control flag to a flag specified on the table
 *
 * @param options The terminal flags
 * @param i The index of the flag
 * @param c The string with the flag
 * @return Returns -1 if error, 0 if not
 */
int set_control(struct termios *options, int i, char *c) {
    if (isascii(*c) == 0) {
        return -1;
    }

    options->c_cc[s_values[i].tcflag] = *c;

    if (tcsetattr(0, TCSANOW, options) == -1) {
        printf("tcsetattr ERROR");
        exit(1);
    }
    return 0;
}

/**
 * Prints the character assicated with the key
 *
 * @param c The integer value fo the char
 */
void print_key(int c) {
    if (isprint(c) != 0) {
        putchar(c);
    } else if (c == 127) {
        // Special case as ctrl-H produces the DEL (127) char
        printf("^H");
    } else if (iscntrl(c) != 0) {
        printf("^%c", c - 1 + 'A');
    }
}

/**
 * Prints the control flags
 *
 * @param options
 */
void print_control(struct termios *options) {
    for (int i = 0; s_values[i].string; i++) {
        printf(s_values[i].string);
        printf(" = ");
        print_key(options->c_cc[s_values[i].tcflag]);
        printf("; ");
    }
    printf("\n");
}


/*
 * SETTINGS Section
 */
struct setting {
    // Eases the toggling
    tcflag_t tcflag;
    char *string;

    void (*printer)(struct setting *, struct termios *);

    void (*setter)(struct setting *, struct termios *, int);
};

// Define if the terminal setting is on (isTurnOn == 1) or off (isTurnOn == 0)
void set_opt(struct setting *setting,
             struct termios *options,
             tcflag_t *flag_p,
             int isTurnOn) {
    if (isTurnOn == 1)
        *flag_p |= setting->tcflag;
    else
        *flag_p &= ~(setting->tcflag);

    if (tcsetattr(0, TCSANOW, options) == -1) {
        printf("tcsetattr ERROR");
        exit(1);
    }
}

/*
 * Setting the different flag types that are used by termios
 */
void set_iflag(struct setting *row, struct termios *options, int isTurnOn) {
    set_opt(row, options, &options->c_iflag, isTurnOn);
}

void set_oflag(struct setting *row, struct termios *options, int isTurnOn) {
    set_opt(row, options, &options->c_oflag, isTurnOn);
}

void set_lflag(struct setting *row, struct termios *options, int isTurnOn) {
    set_opt(row, options, &options->c_lflag, isTurnOn);
}


/*
 * Functions to print the values
 */
void print_opt(struct setting *row, int flag) {
    if (flag & row->tcflag)
        printf(row->string);
    else
        printf("-%s", row->string);
}

/*
 * Printing the different flag types set in termios
 */
void print_iflag(struct setting *row, struct termios *options) {
    print_opt(row, options->c_iflag);
}

void print_oflag(struct setting *row, struct termios *options) {
    print_opt(row, options->c_oflag);
}

void print_lflag(struct setting *row, struct termios *options) {
    print_opt(row, options->c_lflag);
}

/*
 * Collection of common flags that can be used
 * More can be found at: https://pubs.opengroup.org/onlinepubs/7908799/xsh/termios.h.html
 */
struct setting setting_table[] = {
        {ICRNL,  "icrnl",  print_iflag, set_iflag},
        {OLCUC,  "olcuc",  print_oflag, set_oflag},
        {ONLCR,  "onlcr",  print_oflag, set_oflag},
        {OCRNL,  "ocrnl",  print_oflag, set_oflag},
        {ONOCR,  "onocr",  print_oflag, set_oflag},
        {ONLRET,  "onlret",  print_oflag, set_oflag},
        {OFILL,  "ofill",  print_oflag, set_oflag},
        {ECHO,   "echo",   print_lflag, set_lflag},
        {ECHOE,  "echoe",  print_lflag, set_lflag},
        {ECHOK,  "echok",  print_lflag, set_lflag},
        {ECHONL,  "echonl",  print_lflag, set_lflag},
        {ICANON, "icanon", print_lflag, set_lflag},
        {IEXTEN,  "iexten",  print_lflag, set_lflag},
        {ISIG,   "isig",   print_lflag, set_lflag},
        {NOFLSH,  "noflsh",  print_lflag, set_lflag},
        {TOSTOP,  "tostop",  print_lflag, set_lflag},
        {XCASE,  "xcase",  print_lflag, set_lflag},
        {0, NULL,          0,           0}
};

/**
 * Used to check if a given setting is valid
 * Goes through all the elements in the table
 *
 * @param argv The argument
 * @return Returns the index of the setting, or -1 if not found
 */
int valid_setting(char *argv) {
    for (int i = 0; setting_table[i].string; i++) {
        if (strcmp(setting_table[i].string, argv) == 0) {
            return i;
        }
    }
    return -1;
}

/**
 * Sets the setting specified in the arguments
 * This generalizes the setter for the three types of flags there are
 *
 * @param options The settings to be set
 * @param i The index of the setting in question
 * @param isTurnOff If we are turning this off (1 if yes, 0 if not)
 */
void set_setting(struct termios *options, int i, int isTurnOff) {
    setting_table[i].setter(&setting_table[i], options, !isTurnOff);
}

/**
 * Prints the setting in question to the terminal
 *
 * @param options The settings to print
 */
void print_setting(struct termios *options) {
    for (int i = 0; setting_table[i].string; i++) {
        setting_table[i].printer(&setting_table[i], options);
        printf(" ");
    }
    printf("\n");
}

// Main function that handles the argvs
void handle_argvs(struct termios *options, int argc, char *argv[]) {
    for (int i = 1; i < argc; i++) {
        // First, check if the first char is not a dash
        int isTurnOff = 0;
        if (argv[i][0] == '-') {
            isTurnOff = 1;
            ++argv[i];
        }

        // Do the remaining checks
        if (valid_speed(argv[i]) != -1) {
            // Check if trying to change speed
            set_speed(options, valid_speed(*argv));
            continue;
        }
        else if (valid_control(argv[i]) != -1) {
            // Check if it is a control flag

            // Check if it's missing the argument for erase
            if (argv[i + 1] == NULL) {
                printf("Missing argument to `erase'\n");
                exit(1);
            }

            // Argument is an integer - Invalid
            if (set_control(options, valid_control(argv[i]), argv[i + 1]) == -1) {
                printf("Invalid integer argument\n");
                exit(1);
            }
            i++;
            continue;
        } else if (valid_setting(argv[i]) != -1) {
            set_setting(options, valid_setting(argv[i]), isTurnOff);
            continue;
        }
        else {
            // Case the user has inputted invalid arguments
            printf("Invalid argument\n");
        }
    }
}

int main(int argc, char **argv) {
    struct termios options;

    // Get parameters from terminal
    if (tcgetattr(0, &options) == -1) {
        printf("%s", "tcgetattr ERROR");
        exit(1);
    } if (argc <= 1) {
        printf("speed %s baud;\n", baud[cfgetospeed(&options)].string);
        print_control(&options);
        print_setting(&options);
    } else {
        handle_argvs(&options, argc, argv);
    }
    
    exit(0);
}


