// gpioctl.h    Include file for Port I/O Driver
//
// Define the IOCTL codes we will use.  The IOCTL code contains a command
// identifier, plus other information about the device, the type of access
// with which the file must have been opened, and the type of buffering.
//

// Device type           -- in the "User Defined" range."
#define PORTIO_TYPE 40000

// per lo start pulnix
#define PORTIO_START_TYPE_PULSE_WIDTH	0
#define PORTIO_START_TYPE_DOUBLE_PULSE	1
#define PORTIO_START_TYPE_FIXED			2
#define PORTIO_HD_NO					0
#define PORTIO_HD_YES					1

// The IOCTL function codes from 0x800 to 0xFFF are for customer use.

#define IOCTL_PORTIO_READ_PORT_UCHAR \
    CTL_CODE(PORTIO_TYPE, 0x900, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_PORTIO_WRITE_PORT_UCHAR \
    CTL_CODE(PORTIO_TYPE, 0x901, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_PORTIO_START_PULNIX \
    CTL_CODE(PORTIO_TYPE, 0x902, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_PORTIO_WAIT_H_BLANK \
    CTL_CODE(PORTIO_TYPE, 0x903, METHOD_BUFFERED, FILE_ANY_ACCESS)

typedef struct  _PORTIO_READ_WRITE {
    ULONG   PortAddress;
    UCHAR   CharData;
}   PORTIO_READ_WRITE, *PPORTIO_READ_WRITE;

typedef struct  _PORTIO_START_PULNIX {
	ULONG	StartType;  // 0:pulse width, 1:double pulse, 2:fixed
 	ULONG	HD_Installed;  // con HD installato (1) oppure senza (0)
	ULONG   PortAddressOut;
    UCHAR   MaskOut; // determina il bit del VINIT: normalmente 0x01
	ULONG   PortAddressIn;
    UCHAR   MaskIn;  // determina il bit dell'HD letto da parallela:
					 // normalmente 0x20
	ULONG	ExposureTime; // in numero di righe orizzontali
	ULONG	HD_TimeOut; // se HD e' installato indica l'avvenuto time out di
						// lettura (1) o meno (0)
}   PORTIO_START_PULNIX, *PPORTIO_START_PULNIX;

typedef struct  _PORTIO_WAIT_H_BLANK {
	ULONG   PortAddressIn;
    UCHAR   MaskIn;  // determina il bit dell'HD letto da parallela:
					 // normalmente 0x20
	ULONG	HD_Received; // indica l'avvenuta ricezione dell'HD (1) o meno (0)
}   PORTIO_WAIT_H_BLANK, *PPORTIO_WAIT_H_BLANK;
