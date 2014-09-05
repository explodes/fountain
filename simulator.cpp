#include <stdlib.h>
#include <stdio.h>

typedef struct {

	uint16_t *buckets;
	uint16_t *biases;
	size_t num_buckets;
	size_t tick;

	void init (size_t num_buckets) {
		// todo: make this the constructor
		this->tick = 0;
		this->num_buckets = num_buckets;
		this->buckets = (uint16_t*) calloc(num_buckets, sizeof(uint16_t));
		this->biases = (uint16_t*) calloc(num_buckets, sizeof(uint16_t));
	}

	void generate_biases (uint16_t maximum) {
		for (size_t index=0; index<this->num_buckets; index++) {
			this->biases[index] = arc4random_uniform(maximum);
		}
	}

	void generate_buckets (uint16_t maximum) {
		int distance;
		double phaseFactor;
		uint16_t amount;
		for (size_t index=0; index<this->num_buckets; index++) {
			distance = abs(index - this->tick) % this->num_buckets;
			phaseFactor = (double) distance / this->num_buckets;
			amount = (uint16_t) (maximum * phaseFactor);
			this->buckets[index] = this->biases[index] + amount;
		}
		this->tick++;
	}

	void print_biases () {
		for (size_t index=0; index<this->num_buckets; index++) {
			printf("%d ", this->biases[index]);
		}
	}

	void print_buckets () {
		for (size_t index=0; index<this->num_buckets; index++) {
			printf("%d ", this->buckets[index]);
		}
	}

	void clean () {
		// todo: make this the destructor
		free(this->buckets);
		free(this->biases);
	}

} AudioSimulator;


int main () {
	AudioSimulator *sim = new AudioSimulator();
	sim->init(16);
	sim->generate_biases(1024);
	printf("biases:");
	sim->print_biases();
	sim->generate_buckets(1024);
	printf("\nbuckets:");
	sim->print_buckets();
	sim->clean();
}