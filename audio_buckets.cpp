#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <unistd.h>

#pragma mark -
#pragma mark System Parameters

// Number of audio buckets output by FHT
#define N_BUCKETS 16
// Difference threshold per bit.
// Example, 5/4 = New chunk must be 125%+ the previous chunk value
#define THRESH_NUMERATOR 5
#define THRESH_DENOMINATOR 4

// Bucket generator bias for lower frequency buckets
#define BUCKET_BIAS_FACTOR 100

// Bucket generator maximum amplitude
#define BUCKET_MAXIMUM 1024

#pragma mark -
#pragma mark System Constants
// Non-negotiable constants

// N_CHUNKS MUST BE 16, THIS IS NOT OPTIONAL: TWO BYTES OF OUTPUT = 16 OUTPUT DETERMINATIONS
#define N_CHUNKS 16

// How long to wait between loops in useconds_t
#define ARDUINO_LOOP_SLEEP 56000

// PI
#define PI 3.1415926

#pragma mark -
#pragma mark Arduino lifecycle

void start ();
void loop ();
int main () {
	start();
	while(true) { 
		loop(); 
		usleep(ARDUINO_LOOP_SLEEP);
	}
	return 0;
}

#pragma mark -
#pragma mark Application Variables

uint16_t BUCKETS[N_BUCKETS];
uint16_t CHUNKS[N_CHUNKS];
uint16_t PREVIOUS_CHUNKS[N_CHUNKS];
uint16_t PREVIOUS_MINIMUMS[N_CHUNKS];
//uint16_t PREVIOUS_MAXIMUMS[N_CHUNKS];
volatile uint16_t output;
static volatile uint16_t tick = 0;

#pragma mark -
#pragma mark Application Functionality

void gen_buckets () {

	for (int index=0; index<N_BUCKETS; index++) {
		int distance = abs(index - tick) % N_BUCKETS;
		double phaseFactor = (double) distance / N_BUCKETS;
		uint16_t amount = (uint16_t) (BUCKET_MAXIMUM * phaseFactor);
		// Lower-end frequency buckets tend to have higher values than higher frequency buckets
		uint16_t bias = (N_BUCKETS - index) * BUCKET_BIAS_FACTOR;
		BUCKETS[index] = amount + bias;
	}
#ifdef DEBUG
	for (int index=0; index<N_BUCKETS; index++) {
		printf("%d ", BUCKETS[index]);
	}
	printf("\n");
#endif
}

void prime_chunks () {
	memcpy(PREVIOUS_MINIMUMS, CHUNKS, N_CHUNKS * sizeof(uint16_t));
	//memcpy(PREVIOUS_MAXIMUMS, CHUNKS, N_CHUNKS * sizeof(uint16_t));
}

void gen_chunks () {
	static const int BUCKETS_PER_CHUNK = N_BUCKETS / N_CHUNKS;
	for (int chunkIndex=0; chunkIndex<N_CHUNKS; chunkIndex++) {
		CHUNKS[chunkIndex] = 0;
		for (int bucketIndex=chunkIndex*BUCKETS_PER_CHUNK; bucketIndex<(chunkIndex + 1) * BUCKETS_PER_CHUNK; bucketIndex++) {
			CHUNKS[chunkIndex] += BUCKETS[bucketIndex];
		}
	}
}

void gen_output () {
	output = 0;
	for (int index=0; index<N_CHUNKS; index++) {
		if (CHUNKS[index] < PREVIOUS_MINIMUMS[index]) { PREVIOUS_MINIMUMS[index] = CHUNKS[index]; }
		//if (CHUNKS[index] > PREVIOUS_MAXIMUMS[index]) { PREVIOUS_MAXIMUMS[index] = CHUNKS[index]; }
		if (CHUNKS[index] - PREVIOUS_MINIMUMS[index] > (PREVIOUS_CHUNKS[index] - PREVIOUS_MINIMUMS[index]) * THRESH_NUMERATOR / THRESH_DENOMINATOR) {
			//printf("%d > thresh(%d)\n", CHUNKS[index], PREVIOUS_CHUNKS[index]);
			output |= 1 << index;
		}
	}	
}

void display_digital () {
	for (int index=0; index<16; index++) {
		if (output & (1<<index)) {
			putc('1', stdout);
		} else {
			putc('0', stdout);
		}
	}
	putc('\n', stdout);
	//printf("%d %d\n", (char) output, (char) (output >> 8));
}

void save_chunks () {
	memcpy(PREVIOUS_CHUNKS, CHUNKS, N_CHUNKS * sizeof(uint16_t));
}

#pragma mark -
#pragma mark Lifecyle Implemenation

void start () {
	memset(BUCKETS, 0, N_BUCKETS * sizeof(uint16_t));
	memset(CHUNKS, 0, N_CHUNKS * sizeof(uint16_t));
	memset(PREVIOUS_CHUNKS, 0, N_CHUNKS * sizeof(uint16_t));
}

void loop () {
	gen_buckets();
	gen_chunks();
	if (!tick) {
		prime_chunks();
	} else {
		gen_output();
		display_digital();
	}
	save_chunks();
	tick++;
}


