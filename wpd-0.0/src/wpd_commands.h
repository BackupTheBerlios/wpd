
#ifndef WPD_COMMAND
#define WPD_COMMAND

/* A command is made up of two parts:
 * A COMMAND_CLASS and a CLASS_WHATTODO
 */

/* command classes are: */
#define COMMAND_LENGTH 0x04
#define COMMAND_BATTERY         0x01000000
#define COMMAND_CPU             0x02000000
#define COMMAND_SYSTEM          0x04000000
#define COMMAND_KEEP_CONNECTION 0x08000000

/* actions in each class: */
#define BATTERY_PRESENT_RATE       0x00000001
#define BATTERY_REMAINING_CAPACITY 0x00000002
#define BATTERY_MAX_CAPACITY       0x00000004
#define BATTERY_AC_STATE           0x00000008
#define BATTERY_PRESENT            0x00000010

#define CPU_FREQ                   0x00000001
#define CPU_SLOW                   0x00000002
#define CPU_FAST                   0x00000004
#define CPU_AUTO                   0x00000008
#define CPU_GOVERNOR               0x00000010
#define CPU_AVAIL_FREQ             0x00000020
#define CPU_TEMP                   0x00000040

#define SYSTEM_SUSPEND             0x00000001
#define SYSTEM_TIMEREMAINING       0x00000002
#define SYSTEM_TIME_FORMATTED      0x00000004
#define SYSTEM_LCD_ON              0x00000008
#define SYSTEM_DISPLAY_ON          0x00000010
#define SYSTEM_BOTH_ON             0x00000020

/* define which is the maximum number of cpu frequencies returned by WPD */
#define WPD_MAX_N_FREQUENCIES 50

/* Set the port WPD is lsitening to. Call this before any
 * other function, if required (port is defaulted to 1978) */
void wpd_setup(int);

/* send a command and read an integer as response, 
 * the second paraneters is a socket. 
 * Can be "-1" if no stable connection is required.
 * The last parameter is 0 if all is ok, 1 if the socket has failed */
int send_command(int, int, int*);

/* send a command and read a string as response, 
 * the third paraneters is a socket. 
 * Can be "-1" if no stable connection is required */
int send_command_str( int, char*, int );

/* send a command and read a integer array as response, 
 * the third paraneters is a socket. 
 * Can be "-1" if no stable connection is required */
int send_command_array( int, int*, int );

/* setup a stable connection if required. 
 * Return the socket */
int setup_connection();


/* Send a command and read an integer as response
 * Auto manage connection. 
 * The second parameter is the reference of a socket
 * which (the socket) can be "-1" at first call. */
int keep_send_command(int, int*);

/* Send a command and read a string as response
 * Auto manage connection. 
 * The second parameter is the reference of a socket
 * which (the socket) can be "-1" at first call. */
int keep_send_command_str( int, char*, int* );

/* Send a command and read an integer array as response
 * Auto manage connection. 
 * The second parameter is the reference of a socket
 * which (the socket) can be "-1" at first call. */
int keep_send_command_array( int, int*, int* );


#endif
