
#ifndef WPD_COMMAND
#define WPD_COMMAND


#define WPD_CMD( md, sy, sb ) md | sy | sb

#define IS_REQUEST( a ) (( a & MASK_MODE ) == MODE_REQUEST )
#define IS_COMMAND( a ) (( a & MASK_MODE ) == MODE_COMMAND )

#define MODE( a ) ( a & MASK_MODE )
#define SYS( a ) ( a & MASK_SYS )
#define SUB( a ) ( a & MASK_SUB )

#define COMMAND_LENGTH sizeof(int)

/* A command is made up of two parts:
 * A COMMAND_CLASS and a CLASS_WHATTODO
 */
#define MASK_MODE 0xf0000000
#define MASK_SYS  0x0f000000
#define MASK_SUB  0x000000ff

/* command classes are: */
#define MODE_REQUEST     0x10000000
#define MODE_COMMAND     0x20000000

#define KEEP_CONNECTION  0x40000000
#define STREAM           0x80000000

#define SYS_BATTERY      0x01000000
#define SYS_CPU          0x02000000
#define SYS_SYSTEM       0x04000000


/* actions in each class: */
#define PRESENT_RATE       0x00000001
#define REMAINING_CAPACITY 0x00000002
#define MAX_CAPACITY       0x00000004
#define AC_STATE           0x00000008
#define BATTERY_PRESENT    0x00000010

/* req */
#define FREQUENCY          0x00000001
#define GOVERNOR           0x00000010
#define AVAIL_FREQUENCIES  0x00000020
#define CPU_TEMP           0x00000040
/* comm */
#define CPU_SLOW           0x00000002
#define CPU_FAST           0x00000004
#define CPU_AUTO           0x00000008
#define CPU_CONSERVATIVE   0x00000010

/* req */
#define TIMEREMAINING      0x00000002
#define TIME_FORMATTED     0x00000004
/* comm */
#define LCD_ON             0x00000008
#define DISPLAY_ON         0x00000010
#define LCD_DISPLAY_ON     0x00000020
#define SYSTEM_SUSPEND     0x00000001

/* define which is the maximum number of cpu frequencies returned by WPD */
#define WPD_MAX_N_FREQUENCIES 50

/* Set the port WPD is lsitening to. Call this before any
 * other function, if required (port is defaulted to 1978) */
void wpd_setup(int);

/* send a request and read an integer as response, 
 * the second paraneters is a socket. 
 * Can be "-1" if no stable connection is required.
 * The last parameter is 0 if all is ok, 1 if the socket has failed */
int send_request(int, int, int*);

/* send a request and read a string as response, 
 * the third paraneters is a socket. 
 * Can be "-1" if no stable connection is required */
int send_request_str( int, char*, int );

/* send a request and read a integer array as response, 
 * the third paraneters is a socket. 
 * Can be "-1" if no stable connection is required */
int send_request_array( int, int*, int );

int send_command( int, int, int, int* );

/* setup a stable connection if required. 
 * Return the socket */
int setup_connection();


/* Send a request and read an integer as response
 * Auto manage connection. 
 * The second parameter is the reference of a socket
 * which (the socket) can be "-1" at first call. */
int keep_send_request(int, int*);

/* Send a request and read a string as response
 * Auto manage connection. 
 * The second parameter is the reference of a socket
 * which (the socket) can be "-1" at first call. */
int keep_send_request_str( int, char*, int* );

/* Send a request and read an integer array as response
 * Auto manage connection. 
 * The second parameter is the reference of a socket
 * which (the socket) can be "-1" at first call. */
int keep_send_request_array( int, int*, int* );

int keep_send_command( int, int, int* );

#endif
