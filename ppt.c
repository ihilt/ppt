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
void output_on_time(FILE *, const char fname[]);
void output_off_time(FILE *, const char fname[]);

int main()
{
	int status; /* Container for STATUS bits 3 - 7 */

/* Flow control variables */
	char BUSY_STATE, ACK_STATE, ERR_STATE, SLCT_STATE, PE_STATE = OFF;

/* setup structure for nanosleep() */
	struct timespec delay = {0};
	delay.tv_nsec = 1000000;

/* file pointer for time data storage */
	FILE *fp;

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

	printf("status: %x\n", inb(STATUS));

	while (status = (inb(STATUS) >> 3)^0x10) {
		nanosleep(&delay, NULL);
		/* BUSY */
		if((~(status >> 4) & 0x01) == ON && (BUSY_STATE != ON)) {
			output_on_time(fp, busy);
			BUSY_STATE = ON;
		}
		else if((~(status >> 4) & 0x01) == OFF && BUSY_STATE == ON) {
			output_off_time(fp, busy);
			BUSY_STATE = OFF;
		}
		/* ACK */
		else if((~(status >> 3) & 0x01) == ON && (ACK_STATE != ON)) {
			output_on_time(fp, ack);
			ACK_STATE = ON;
		}
		else if((~(status >> 3) & 0x01) == OFF && ACK_STATE == ON) {
			output_off_time(fp, ack);
			ACK_STATE = OFF;
		}
		/* SLCT */
		else if((~(status >> 1) & 0x01) == ON && (SLCT_STATE != ON)) {
			output_on_time(fp, slct);
			SLCT_STATE = ON;
		}
		else if((~(status >> 1) & 0x01) == OFF && SLCT_STATE == ON) {
			output_off_time(fp, slct);
			SLCT_STATE = OFF;
		}
		/* ERR */
		else if(~(status & 0x01) == ON && (ERR_STATE != ON)) {
			output_on_time(fp, err);
			ERR_STATE = ON;
		}
		else if(~(status & 0x01) == OFF && ERR_STATE == ON) {
			output_off_time(fp, err);
			ERR_STATE = OFF;
		}
		/* PE */
		else if((~(status >> 2) & 0x01) == ON && (PE_STATE != ON)) {
			output_on_time(fp, pe);
			PE_STATE = ON;
		}
		else if((~(status >> 2) & 0x01) == OFF && PE_STATE == ON) {
			output_off_time(fp, pe);
			PE_STATE = OFF;
		}
	}
	if (ioperm(BASEPORT, 3, 0)) {
		perror("ioperm");
		exit(1);
	}
	exit(0);
}

void output_on_time(FILE *fp, const char fname[])
{
	fp = fopen(fname, "a");
	fprintf(fp, "%s ON, %d\n", fname, time(NULL));
	fclose(fp);
}

void output_off_time(FILE *fp, const char fname[])
{
	fp = fopen(fname, "a");
	fprintf(fp, "%s OFF, %d\n", fname, time(NULL));
	fclose(fp);
}
