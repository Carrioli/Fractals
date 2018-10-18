

/* gestione del driver per l'accesso agli indirizzi di I/O */

#include <stdio.h>
#include <stdlib.h>
#include <util_nt.h>
#ifndef __WATCOMC__
	#include <windows.h>
	#include <winioctl.h>
	#include <pioctl.h>      // This defines the IOCTL constants.

	static HANDLE  hndFile; // Handle to device, obtain from CreateFile
	static BOOL device_aperto;
	static void output_error_message(void);
#else
	#include <conio.h>
#endif /*__WATCOMC__*/

/* ritorna il codice di errore */

#ifndef __WATCOMC__
short
util_portio_init(
	void
)
{
	if(device_aperto)
		return SEAV_OK_ALL;
	hndFile = CreateFile(
		"\\\\.\\PortIoDev",
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		0,
		NULL
	);
                
    if (hndFile == INVALID_HANDLE_VALUE) {
		return SEAV_ERR_PORTIO_INIT;
	} else {
		device_aperto = TRUE;
		return SEAV_OK_ALL;
	}
}

/**********************************************************************/

void
util_portio_close(
	void
)
{
	if(device_aperto) {
		CloseHandle(hndFile);
		device_aperto = FALSE;
	}
}
/**********************************************************************/

static void
output_error_message(
	void
)
{
	LPVOID MsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR) &MsgBuf,
		0,
		NULL
	);
	MessageBox(
		NULL, (const char *) MsgBuf, "GetLastError",
		MB_OK|MB_ICONINFORMATION
	);
	LocalFree(MsgBuf);
}

/**********************************************************************/

short util_portio_is_device_open(void)
{
	return (short) device_aperto;
}

/**********************************************************************/

/* ritorna OK se non c'e' stato time-out di lettura dell'HD, altrimenti
   ritorna NON_TROVATO, se l'HD non e' installato ritorna sempre OK */

short
util_start_pulnix(
	long start_type,   // 0:PW, 1:DP, 2:Fix
	long HD_installed,
	long address_out,  // per il VINIT
	unsigned char mask_out,
	long address_in,   // per leggere l'HD
	unsigned char mask_in,
	long exposure_time // numero di righe
)
{
	PORTIO_START_PULNIX Ms;
	DWORD cbReturned;
	if(!device_aperto)
		return SEAV_ERR_PORTIO_INIT;
	Ms.StartType = start_type;
	Ms.HD_Installed = HD_installed;
	Ms.PortAddressOut = address_out;
	Ms.MaskOut = mask_out;
	Ms.PortAddressIn = address_in;
	Ms.MaskIn = mask_in;
	Ms.ExposureTime = exposure_time;
	if(!DeviceIoControl(
		hndFile,                            // Handle to device
		(DWORD) IOCTL_PORTIO_START_PULNIX,
		&Ms,                                // Buffer to driver.
		sizeof(PORTIO_START_PULNIX),        // Length of buffer in bytes.
		&Ms,                                // Buffer from driver.
		sizeof(PORTIO_START_PULNIX),        // Length of buffer in bytes.
		&cbReturned,                        // Bytes placed in DataBuffer.
		NULL                                // NULL means wait till op. completes.
	)) {
//		output_error_message();
	}
	if(HD_installed) {
		if(Ms.HD_TimeOut)
			return SEAV_ERR_HD_TIMEOUT;
		else
			return SEAV_OK_ALL;
	} else
		return SEAV_OK_ALL;
}

/**********************************************************************/

/* ritorna SEAV_OK_ALL se non c'e' stato time-out di lettura dell'HD, altrimenti
   ritorna SEAV_ERR_HD_TIMEOUT */

short
util_wait_h_blank(
	long address_in,   // per leggere l'HD
	unsigned char mask_in
)
{
	PORTIO_WAIT_H_BLANK Ms;
	DWORD cbReturned;
	if(!device_aperto)
		return SEAV_ERR_PORTIO_INIT;
	Ms.PortAddressIn = address_in;
	Ms.MaskIn = mask_in;
	if(!DeviceIoControl(
		hndFile,                            // Handle to device
		(DWORD) IOCTL_PORTIO_WAIT_H_BLANK,
		&Ms,                                // Buffer to driver.
		sizeof(PORTIO_WAIT_H_BLANK),        // Length of buffer in bytes.
		&Ms,                                // Buffer from driver.
		sizeof(PORTIO_WAIT_H_BLANK),        // Length of buffer in bytes.
		&cbReturned,                        // Bytes placed in DataBuffer.
		NULL                                // NULL means wait till op. completes.
	)) {
//		output_error_message();
	}
	if(!Ms.HD_Received)
		return SEAV_ERR_HD_TIMEOUT;
	else
		return SEAV_OK_ALL;
}

#endif /*__WATCOMC__*/

/**********************************************************************/

void
util_portio_outp(
	long address,
	long val
)
{
#ifndef __WATCOMC__
	PORTIO_READ_WRITE Ms;
	DWORD cbReturned;
	if(!device_aperto)
		return;
	Ms.PortAddress = address;
	Ms.CharData = (unsigned char) val;
	if(!DeviceIoControl(
		hndFile,                            // Handle to device
		(DWORD) IOCTL_PORTIO_WRITE_PORT_UCHAR,      // IO Control code for Read
		&Ms,                                // Buffer to driver.
		sizeof(PORTIO_READ_WRITE),          // Length of buffer in bytes.
		&Ms,                                // Buffer from driver.
		sizeof(PORTIO_READ_WRITE),          // Length of buffer in bytes.
		&cbReturned,                        // Bytes placed in DataBuffer.
		NULL                                // NULL means wait till op. completes.
	)) {
//		output_error_message();
	}
#else
	outp(address, val);
#endif /*__WATCOMC__*/
}

/**********************************************************************/

long
util_portio_inp(
	long address
)
{
#ifndef __WATCOMC__
	PORTIO_READ_WRITE Ms;
	DWORD cbReturned;
	if(!device_aperto)
		return 0L;
	Ms.PortAddress = address;
	if(!DeviceIoControl(
		hndFile,                            // Handle to device
		(DWORD) IOCTL_PORTIO_READ_PORT_UCHAR,       // IO Control code for Read
		&Ms,                                // Buffer to driver.
		sizeof(PORTIO_READ_WRITE),          // Length of buffer in bytes.
		&Ms,                                // Buffer from driver.
		sizeof(PORTIO_READ_WRITE),          // Length of buffer in bytes.
		&cbReturned,                        // Bytes placed in DataBuffer.
		NULL                                // NULL means wait till op. completes.
	)) {
//		output_error_message();
	}
	return (long) Ms.CharData;
#else
	return (long) inp(address);
#endif /*__WATCOMC__*/
}


