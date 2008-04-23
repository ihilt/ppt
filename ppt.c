#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include </usr/src/linux/include/asm/io.h>

#define BASEPORT	0x378
#define STATUS		BASEPORT+1
#define CONTROL		BASEPORT+2
#define ON			1
#define OFF			0

int main()
{
	int status; /* Container for STATUS bits 3 - 7 */

/* Flow control variables */
	char BUSY_STATE, ACK_STATE, ERR_STATE, SLCT_STATE, PE_STATE = OFF;

/* setup structure for nanosleep() */
	struct timespec req = {0};
	req.tv_nsec = 1000000;

/* file pointer for time data storage */
	FILE *fp;

	if (ioperm(BASEPORT, 3, 1)) {
		perror("ioperm");
		exit(1);
	}

	printf("status: %x\n", inb(STATUS));

	while (status = (inb(STATUS) >> 3)^0x10) {
		nanosleep(&req, NULL);
		/* BUSY */
		if((~(status >> 4) & 0x01) == ON && (BUSY_STATE != ON)) {
			fp = fopen("BUSY", "a");
			fprintf(fp, "BUSY ON, %d\n", time(NULL));
			fclose(fp);
			BUSY_STATE = ON;
		}
		else if((~(status >> 4) & 0x01) == OFF && BUSY_STATE == ON) {
			fp = fopen("BUSY", "a");
			fprintf(fp, "BUSY OFF, %d\n", time(NULL));
			fclose(fp);
			BUSY_STATE = OFF;
		}
		/* ACK */
		else if((~(status >> 3) & 0x01) == ON && (ACK_STATE != ON)) {
			fp = fopen("ACK", "a");
			fprintf(fp, "ACK ON, %d\n", time(NULL));
			fclose(fp);
			ACK_STATE = ON;
		}
		else if((~(status >> 3) & 0x01) == OFF && ACK_STATE == ON) {
			fp = fopen("ACK", "a");
			fprintf(fp, "ACK OFF, %d\n", time(NULL));
			fclose(fp);
			ACK_STATE = OFF;
		}
		/* SLCT */
		else if((~(status >> 1) & 0x01) == ON && (SLCT_STATE != ON)) {
			fp = fopen("SLCT", "a");
			fprintf(fp, "SLCT ON, %d\n", time(NULL));
			fclose(fp);
			SLCT_STATE = ON;
		}
		else if((~(status >> 1) & 0x01) == OFF && SLCT_STATE == ON) {
			fp = fopen("SLCT", "a");
			fprintf(fp, "SLCT OFF, %d\n", time(NULL));
			fclose(fp);
			SLCT_STATE = OFF;
		}
		/* ERR */
		else if(~(status & 0x01) == ON && (ERR_STATE != ON)) {
			fp = fopen("ERR", "a");
			fprintf(fp, "ERR ON, %d\n", time(NULL));
			fclose(fp);
			ERR_STATE = ON;
		}
		else if(~(status & 0x01) == OFF && ERR_STATE == ON) {
			fp = fopen("ERR", "a");
			fprintf(fp, "ERR OFF, %d\n", time(NULL));
			fclose(fp);
			ERR_STATE = OFF;
		}
		/* PE */
		else if((~(status >> 2) & 0x01) == ON && (PE_STATE != ON)) {
			fp = fopen("PE", "a");
			fprintf(fp, "PE ON, %d\n", time(NULL));
			fclose(fp);
			PE_STATE = ON;
		}
		else if((~(status >> 2) & 0x01) == OFF && PE_STATE == ON) {
			fp = fopen("PE", "a");
			fprintf(fp, "PE OFF, %d\n", time(NULL));
			fclose(fp);
			PE_STATE = OFF;
		}
	}
	if (ioperm(BASEPORT, 3, 0)) {
		perror("ioperm");
		exit(1);
	}
	exit(0);
}
