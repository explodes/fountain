#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define N_AUDIOBUCKETS 256

struct AudioSimulator {
	uint16_t biases[N_AUDIOBUCKETS];

	void pbiases () {
		for (int index=0; index<N_AUDIOBUCKETS; index++) {
			printf("%d ", biases[index]);
		}
	}
};

int main () {
	struct AudioSimulator *sim;
	sim->pbiases();
}