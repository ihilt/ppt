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

	if (ioperm(BASEPORT, 3, 1)) {
		perror("ioperm");
		exit(1);
	}

	printf("status: %x\n", inb(STATUS));

	while (status = (inb(STATUS) >> 3)^0x10) {
		nanosleep(&req, NULL);
		if((~(status >> 4) & 0x01) == ON && (BUSY_STATE != ON)) {
			printf("BUSY ON\n");
			BUSY_STATE = ON;
		}
		else if((~(status >> 4) & 0x01) == OFF && BUSY_STATE == ON) {
			printf("BUSY OFF\n");
			BUSY_STATE = OFF;
		}
		else if((~(status >> 3) & 0x01) == ON && (ACK_STATE != ON)) {
			printf("ACK ON\n");
			ACK_STATE = ON;
		}
		else if((~(status >> 3) & 0x01) == OFF && ACK_STATE == ON) {
			printf("ACK OFF\n");
			ACK_STATE = OFF;
		}
		else if((~(status >> 1) & 0x01) == ON && (SLCT_STATE != ON)) {
			printf("SLCT ON\n");
			SLCT_STATE = ON;
		}
		else if((~(status >> 1) & 0x01) == OFF && SLCT_STATE == ON) {
			printf("SLCT OFF\n");
			SLCT_STATE = OFF;
		}
		else if(~(status & 0x01) == ON && (ERR_STATE != ON)) {
			printf("ERR ON\n");
			ERR_STATE = ON;
		}
		else if(~(status & 0x01) == OFF && ERR_STATE == ON){
			ERR_STATE = OFF;
		}
		else if((~(status >> 2) & 0x01) == ON && (PE_STATE != ON)) {
			printf("PE ON\n");
			PE_STATE = ON;
		}
		else if((~(status >> 2) & 0x01) == OFF && PE_STATE == ON) {
			printf("PE OFF\n");
			PE_STATE = OFF;
		}
	}
	if (ioperm(BASEPORT, 3, 0)) {
		perror("ioperm");
		exit(1);
	}
	exit(0);
}
