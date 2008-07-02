#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include </usr/src/linux/include/asm/io.h>

#define BASEPORT	0x378
#define STATUS		BASEPORT+1
#define CONTROL		BASEPORT+2
#define ON		1
#define OFF		0

/*
 * These functions do what they say they do.
 *
 * output_on_time() sends the timestamp to fp with the name of the signal and
 * the "ON" string.
 *
 * output_off_time() sends the timestamp to fp with the name of the signal and
 * the "OFF" string.
 */
void output_on_time(char *output, const char *signal_name);
void output_off_time(char *output, const char *signal_name);

int main(int argc, char **argv)
{
	int status;		/* Container for STATUS bits 3 - 7 */
	char BUSY_STATE,
	     ACK_STATE,
	     ERR_STATE,
	     SLCT_STATE,
	     PE_STATE	= OFF;
	struct timespec delay = { 0 };
	delay.tv_nsec = 1000000;

	if (ioperm(BASEPORT, 3, 1)) {
		perror("ioperm");
		exit(1);
	}

/*
 * There might be a better way to do this, but for now I've collected all of
 * the signal name strings here.
 */
	const char busy[] = "BUSY";
	const char ack[] = "ACK";
	const char slct[] = "SLCT";
	const char err[] = "ERR";
	const char pe[] = "PE";

	char *of;		/* name of output file if given */

	char c;
	int i = argc;
	while (--argc > 0 && **++argv == '-')
		while (c = *++argv[0])
			switch (c) {
			case 'f':
				of = argv[i - argc];
				break;
			default:
				of = NULL;
				break;
			}

	printf("status: %x\n", inb(STATUS));

	while (status = (inb(STATUS) >> 3) ^ 0x10) {
		nanosleep(&delay, NULL);
		if ((~(status >> 4) & 0x01) == ON && (BUSY_STATE != ON)) {
			output_on_time(of, busy);
			BUSY_STATE = ON;
		} else if ((~(status >> 4) & 0x01) == OFF
			   && BUSY_STATE == ON) {
			output_off_time(of, busy);
			BUSY_STATE = OFF;
		} else if ((~(status >> 3) & 0x01) == ON
			 && (ACK_STATE != ON)) {
			output_on_time(of, ack);
			ACK_STATE = ON;
		} else if ((~(status >> 3) & 0x01) == OFF
			   && ACK_STATE == ON) {
			output_off_time(of, ack);
			ACK_STATE = OFF;
		} else if ((~(status >> 1) & 0x01) == ON
			 && (SLCT_STATE != ON)) {
			output_on_time(of, slct);
			SLCT_STATE = ON;
		} else if ((~(status >> 1) & 0x01) == OFF
			   && SLCT_STATE == ON) {
			output_off_time(of, slct);
			SLCT_STATE = OFF;
		} else if (~(status & 0x01) == ON && (ERR_STATE != ON)) {
			output_on_time(of, err);
			ERR_STATE = ON;
		} else if (~(status & 0x01) == OFF && ERR_STATE == ON) {
			output_off_time(of, err);
			ERR_STATE = OFF;
		} else if ((~(status >> 2) & 0x01) == ON && (PE_STATE != ON)) {
			output_on_time(of, pe);
			PE_STATE = ON;
		} else if ((~(status >> 2) & 0x01) == OFF
			   && PE_STATE == ON) {
			output_off_time(of, pe);
			PE_STATE = OFF;
		}
	}
	if (ioperm(BASEPORT, 3, 0)) {
		perror("ioperm");
		exit(1);
	}
	exit(0);
}

void output_on_time(char *fname, const char *sname)
{
	if (fname != NULL) {
		FILE *fp;
		fp = fopen(fname, "a");
		fprintf(fp, "%s ON, %d\n", sname, time(NULL));
		fclose(fp);
	} else {
		fprintf(stdout, "%s ON, %d\n", sname, time(NULL));
	}
}

void output_off_time(char *fname, const char *sname)
{
	if (fname != NULL) {
		FILE *fp;
		fp = fopen(fname, "a");
		fprintf(fp, "%s OFF, %d\n", sname, time(NULL));
		fclose(fp);
	} else {
		fprintf(stdout, "%s OFF, %d\n", sname, time(NULL));
	}
}
