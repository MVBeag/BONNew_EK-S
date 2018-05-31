#ifndef _BSL_COMMON_H_
#define _BSL_COMMON_H_

#define QUEUE_SIZE                      (512)
#define MAX_UART_FRAME_SIZE             (QUEUE_SIZE)
#define UART_BAUDRATE                   (115200)
#define UART_PARITY                     (0)
#define TIMEOUT_MS                      (40)

#define TIMEOUT_ERROR                   (0xEE)
#define PROG_MODE_MIN_RESP              (13)
#define PROG_MODE_VER_OFF               (3)
#define BSL_BUFFER_SIZE                 (240)

#define PM_BUFFER_SIZE                  (128)
#define QS_ESC                          (0x7d)
#define QS_FRAME                        (0x7e)
#define BSL_MODE                        (0x01)

#define BSL_ACK_TIMEOUT_MS              50
#define BSL_HEADER_TIMEOUT_MS           300
#define BSL_PAYLOAD_TIMEOUT_MS          10

#define BSL_ASN_RSP_TIMEOUT_MS          1000

#define BSL_HEADER                      0x80

#define BSL_RX_PASS_REPEAT              (10)
#define BSL_RX_TXT_FILE_REPEAT          (3)

//Received commands
#define RX_DATA_BLOCK                   0x10
#define RX_PASSWORD                     0x11
#define ERASE_SEGMENT                   0x12
#define TOGGLE_INFO                     0x13
#define ERASE_BLOCK                     0x14
#define MASS_ERASE                      0x15
#define CRC_CHECK                       0x16
#define LOAD_PC                         0x17
#define TX_DATA_BLOCK                   0x18
#define TX_BSL_VERSION                  0x19
#define TX_BUFFER_SIZE                  0x1A
#define RX_DATA_BLOCK_FAST              0x1B

// PI
#define CHANGE_BAUD_RATE                0x52
#define BAUD430_4800                    0x01
#define BAUD430_9600                    0x02
#define BAUD430_19200                   0x03
#define BAUD430_38400                   0x04
#define BAUD430_57600                   0x05
#define BAUD430_115200                  0x06

//Responses
#define BSL_DATA_REPLY                  0x3A
#define BSL_MESSAGE_REPLY               0x3B

// Error Codes
// - From the API
#define SUCCESSFUL_OPERATION            0x00
#define NO_EXCEPTIONS                   0x00
#define FLASH_WRITE_CHECK_FAILED        0x01
#define FLASH_FAIL_BIT_SET              0x02
#define VOLTAGE_CHANGE_DURING_PROGRAM   0x03
#define BSL_LOCKED                      0x04
#define BSL_PASSWORD_ERROR              0x05
#define BYTE_WRITE_FORBIDDEN            0x06
// - From the Command Interpreter
#define UNKNOWN_COMMAND                 0x07
#define LENGTH_TOO_BIG_FOR_BUFFER       0x08

#define HEADER_INCORRECT                0x51
#define CHECKSUM_INCORRECT              0x52
#define PACKET_SIZE_ZERO                0x53
#define PACKET_SIZE_TOO_BIG             0x54
#define UNKNOWN_ERROR                   0x55
// errors for PI commands
#define UNKNOWN_BAUD_RATE               0x56

#define FLASH_ACK                       SUCCESSFUL_OPERATION

#define BSL_ACK                         0x00

typedef unsigned char BYTE;

#endif // _BSL_COMMON_H_
