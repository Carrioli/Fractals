#ifndef __MY_ERR_H__
#define __MY_ERR_H__

/* definizione di tutti i codici di errore usati nelle librerie di
  \usr\imaging\lib */

// nessun errore
#define SEAV_OK_ALL										0

// i parametri passati alla routine sono fuori range
#define SEAV_ERR_PARAMETER								-1

// memoria non allocata
#define SEAV_ERR_MEMORY									-2

// errore su file (generico)
#define SEAV_ERR_FILE									-3

// overflow nelle dimensioni dei vettori
#define SEAV_ERR_DIMENSION								-4

// file dei font non accessibile
#define SEAV_ERR_FONT_FILE								-5

// non trovato (generico)
#define SEAV_ERR_NOT_FOUND								-6

// errore inizializzazione scheda
#define SEAV_ERR_GRABBER_INIT							-7

// errore inizializzazione scheda
#define SEAV_ERR_MULTI_BOARD_INIT						-8

// errore allocazione memoria per i frame buffer
#define SEAV_ERR_GRABBER_MEMORY							-9

// errore relativo alla telecamera
#define SEAV_ERR_CAMERA									-10

// il segnale video manca o e' non OK
#define SEAV_ERR_CAMERA_SIGNAL							-11

// operazione annullata
#define SEAV_ERR_CANCEL									-12

// il n. di bit per pixel della vga non e' tra quelli previsti
#define SEAV_ERR_VGA_MODE_NOT_SUPPORTED					-13

// errore inizializzazione display su vga
#define SEAV_ERR_VGA_INIT								-14

// operazione non permessa (a causa del setting dell'hardware)
#define SEAV_ERR_OP_NOT_ALLOWED							-15

// driver portio.sys non aperto
#define SEAV_ERR_PORTIO_INIT							-16

// horizontal drive timeout: non viene ricevuto
#define SEAV_ERR_HD_TIMEOUT								-17

// timer di sistema non partito
#define SEAV_ERR_TIMER_INIT								-18

// lettura del file immagine fallita
#define SEAV_ERR_IMG_FILE_LOAD							-19

// scrittura del file immagine fallita
#define SEAV_ERR_IMG_FILE_SAVE							-20

// illuminazione insufficiente
#define SEAV_ERR_LIGHT_KO								-21

// problema con l'impostazione delle priorita' dei threed o dei processi
#define SEAV_ERR_PRIORITY								-22

// problema con l'allocazione di frame buffer aggiuntivi
#define SEAV_ERR_DMA									-23

// modello di frame grabber non riconosciuto
#define SEAV_ERR_BOARD									-24

// il performance counter non e' supportato dall'hardware
#define SEAV_ERR_PERFORMANCE_COUNTER					-25

// problemi di apprendimento o apprendimento non effettuato
#define SEAV_ERR_LEARNING								-26

// errata configurazione dei blister o qualche gruppo di alveoli è vuoto
#define SEAV_ERR_BLISTER								-27

// errata configurazione dei numeri di fila dei blister
#define SEAV_ERR_ROWS									-28

// errata impostazione dei parametri di handshake dei risultati
#define SEAV_ERR_HANDSHAKE								-29

// errata impostazione dei parametri di handshake dei risultati
#define SEAV_ERR_TOO_MANY_ROWS							-30

// errata dimensione delle finestre spia
#define SEAV_ERR_SPY_WINDOWS_SIZE						-31

// il PLC interno non e' abilitato
#define SEAV_ERR_PLC_NOT_ENABLED						-32

// l'apprendimento dei vuoti non e' stato effettuato
#define SEAV_ERR_LEARNING_EMPTY							-33

// l'apprendimento dei pieni non e' stato effettuato
#define SEAV_ERR_LEARNING_STANDARD						-34

// il numero di schede frame grabber non e' corretto
#define SEAV_ERR_GRABBER_NUMBER							-35

// l'acquisizione parallela non e' possibile
#define SEAV_ERR_PARALLEL_ACQ_NOT_POSSIBLE				-36

// il lotto non e' iniziato
#define SEAV_ERR_BATCH_NOT_INIT							-37

// ci sono caratteri doppi nel font
#define SEAV_ERR_DOUBLE_CHARS							-38

// stringa vuota
#define SEAV_ERR_EMPTY_STRING							-39

// caratteri non presenti nel font
#define SEAV_ERR_CHARS_NOT_PRESENT						-40

// dimensione eccessiva: esce dall'immagine
#define SEAV_ERR_X_TOO_LARGE							-41

// dimensione eccessiva: esce dall'immagine
#define SEAV_ERR_Y_TOO_LARGE							-42

// non c'e' lavoro da fare
#define SEAV_ERR_NOTHING_TO_DO							-43

// l'apprendimento del passo macchina non e' stato effettuato
#define SEAV_ERR_LEARNING_STEP_POSITION					-44

// le code sono di lunghezza errata
#define SEAV_ERR_QUEUE_LENGTH							-45

// le code di lunghezza 0 non sono ammesse
#define SEAV_ERR_QUEUE_LENGTH_0							-46

// problemi sulla socket
#define SEAV_ERR_SOCKET									-47

// le code di lunghezza non 0 non sono ammesse
#define SEAV_ERR_QUEUE_LENGTH_1							-48

// insufficiente numero di tv individuate
#define SEAV_ERR_FEW_CAMERAS							-49

// numero errato di tv individuate
#define SEAV_ERR_WRONG_NUMBER_OF_CAMERAS				-50


#endif /* __MY_ERR_H__ */


