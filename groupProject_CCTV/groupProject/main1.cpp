#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <ctime>
#include <fstream>
using namespace std;

// constants
const int MAP_SIZE = 15;
const int EX = 2;
const int NUM_VALUEABLE = 5;
const int valuable[MAP_SIZE] = { 0, 9, 0, 2, 0, 0, 10, 0, 0, 0, 0, 2, 8, 0, 0 };
const int GENE = MAP_SIZE;
const int POP_SIZE = 10;
const float CROSSOVER_PROBABILITY = 1;
const float MUTATION_PROBABILITY = 0.9;
const int MAXIMUM_GENERATION = 1;

// data storage
char valuable_map[MAP_SIZE + 2 * EX][MAP_SIZE + 2 * EX] = {};
int chromosome[POP_SIZE][GENE] = {};
double fitness_value[POP_SIZE] = {};
int parents[2][GENE] = {};
int children[POP_SIZE][GENE];
int survivor[POP_SIZE][GENE];
int counterSurvivor;
double averageFitness;
double bestFitness = -1;
int bestChromosome[GENE];
ofstream ACP_File("ACP_File.txt");
ofstream BSF_File("BSF_File.txt");
ofstream bestChromosome_File("BestChromosome_File.txt");

// function prototype
void initial_map();
void initialize_population();
void print_Chromosome();
void evaluate_chromosome();
void select_parent();
void crossover();
void mutation();
void survivalSelection();
void copyChromosome();
void calculateAverageFitness();
void recordBestFitness();

int main() {

	cout << "Map of Museum" << endl;
	initial_map();
	cout << endl;
	cout << "Initial Population" << endl;
	initialize_population();
	cout << endl;
	cout << "\n\n";
	for (int j = 0; j < MAXIMUM_GENERATION; j++)
	{
		cout << "\n***GENERATION " << j + 1 << "*" << endl;
		print_Chromosome();
		cout << "\n\n";
		cout << "Calculation of fitness: " << endl;
		evaluate_chromosome();
		calculateAverageFitness();
		recordBestFitness();
		counterSurvivor = 0;

		for (int i = 0; i < (POP_SIZE / 2); i++) {
			cout << "\n\n";
			cout << "Parent Selection: " << endl;
			select_parent();
			cout << "\n\n";
			cout << "Crossover: ";
			crossover();
			cout << "\n\n";
			cout << "Mutation: " << endl;
			mutation();
			cout << "\n\n";
			cout << "Survival Selection: " << endl;
			survivalSelection();
		}
		cout << "\n\n";
		cout << "Copy Chromosome: " << endl;
		copyChromosome();
	}
	ACP_File.close();
	BSF_File.close();
	bestChromosome_File.close();
	return 0;

	return 0;
}

void initial_map() {
	// intital map to all '-';
	for (int i = 0; i < MAP_SIZE + 2 * EX; i++) {
		for (int j = 0; j < MAP_SIZE + 2 * EX; j++) {
			valuable_map[i][j] = '-';
		}
	}

	// mark position of valuable item
	for (int i = EX; i < MAP_SIZE + EX; i++) {
		if (valuable[i - EX] > 0)
			valuable_map[valuable[i - EX] + EX - 1][i] = 'V';
	}


	// show full map
	for (int i = 0; i < MAP_SIZE + 2*EX; i++) {
		for (int j = 0; j < MAP_SIZE + 2*EX; j++) {
			cout << valuable_map[i][j] << " ";
		}
		cout << endl;
	}
	cout << endl;



	// show map
	for (int i = EX; i < MAP_SIZE + EX; i++) {
		for (int j = EX; j < MAP_SIZE + EX; j++) {
			cout << valuable_map[i][j] << " ";
		}
		cout << endl;
	}
}

void initialize_population() {
	//srand(time(0)); // initialise random with time
	for (int c = 0; c < POP_SIZE; c++) {
		for (int g = 0; g < GENE; g++) {
			chromosome[c][g] = rand() % (MAP_SIZE + 1);
		}
	}

	// print chromosome
	for (int c = 0; c < POP_SIZE; c++) {
		for (int g = 0; g < GENE; g++) {
			cout << setw(2) << chromosome[c][g] << " ";
		}
		cout << endl;
	}
}

void print_Chromosome() {

	for (int i = 0; i < POP_SIZE; i++) {
		cout << "Chromosome " << i + 1 << " : ";
		for (int g = 0; g < GENE; g++) {

			cout << setw(2) << chromosome[i][g] << " ";
		}
		cout << endl;
	}

}

void evaluate_chromosome() {
	int num_cctv;
	double f1, f2;
	for (int p = 0; p < POP_SIZE; p++) {
		num_cctv = 0;
		f1 = 0.0, f2 = 0.0;
		double vision_map[MAP_SIZE + 2 * EX][MAP_SIZE + 2 * EX] = {};

		cout << "Chromosome " << p + 1 << endl;
		for (int g = 0; g < GENE; g++) {
			cout << setw(3) << chromosome[p][g] << " ";
			if (chromosome[p][g] != 0) {
				num_cctv++;
				int y = chromosome[p][g] + EX - 1;
				int x = g + EX;

				// center
				vision_map[y][x] = max(2.0, vision_map[y][x]);

				// 1 unit away
				for (int a = y - 1; a <= y + 1; a++) {
					for (int b = x - 1; b <= x + 1; b++)
						vision_map[a][b] = max(1.0, vision_map[a][b]);
				}

				// 2 unit away
				for (int a = y - 2; a <= y + 2; a++) {
					for (int b = x - 2; b <= x + 2; b++)
						vision_map[a][b] = max(0.5, vision_map[a][b]);
				}
			}
		}
		cout << endl << endl;

		
		// show initial value
		cout << "Before Valuable" << endl;
		for (int i = EX; i < MAP_SIZE + EX; i++) {
			for (int j = EX; j < MAP_SIZE + EX; j++) {
				cout << setw(3) << vision_map[i][j] << " ";
			}
			cout << "\t" << i - EX + 1 << endl;
		}
		cout << endl;
		

		// increment for valuable item
		for (int i = EX; i < MAP_SIZE + EX; i++) {
			if (valuable[i - EX] > 0)
				vision_map[valuable[i - EX] + EX - 1][i] *= 2.0;
		}

		
		// show value added with valuable item
		cout << "After Valuable" << endl;
		for (int i = EX; i < MAP_SIZE + EX; i++) {
			for (int j = EX; j < MAP_SIZE + EX; j++) {
				cout << setw(3) << vision_map[i][j] << " ";
			}
			cout << "\t" << i - EX + 1 << endl;
		}
		cout << endl;
		

		// calculate fitness 1
		for (int i = EX; i < MAP_SIZE + EX; i++) {
			for (int j = EX; j < MAP_SIZE + EX; j++) {
				f1 += vision_map[i][j];
			}
		}
		f1 = f1 /((2.0 * MAP_SIZE * MAP_SIZE) + (2.0 * NUM_VALUEABLE));
		//cout << "Fitness 1 : " << f1 << endl;

		// calculate fitness 2
		f2 = ((num_cctv) / (1.0 * GENE));
		//cout << "Fitness 2 : " << f2 << endl;

		// fitness value
		fitness_value[p] = ((0.6 * f1) + (0.4 * f2));
		cout << "Total Fitness = " << fitness_value[p] << endl;

		cout << endl;
	}
}

void select_parent() {
	int indexPlayer1 = 0, indexPlayer2 = 0;
	int winner[2] = {};

	// tournament selection
	do {
		for (int p = 0; p < 2; p++) {
			indexPlayer1 = rand() % POP_SIZE;
			do {
				indexPlayer2 = rand() % POP_SIZE;
			} while (indexPlayer1 == indexPlayer2);

			cout << "Player " << indexPlayer1 + 1 << " vs Player " << indexPlayer2 + 1 << endl;

			// determine winner
			if (fitness_value[indexPlayer1] > fitness_value[indexPlayer2]) {
				winner[p] = indexPlayer1;
			}
			else {
				winner[p] = indexPlayer2;
			}

			cout << "\tWinner " << p + 1 << " is player " << winner[p] + 1 << endl << endl;
		} // end of tournament
	} while (winner[0] == winner[1]); // restart tournament if same winner

	// print winner (parents)
	for (int r = 0; r < 2; r++) {
		cout << "Parent " << r + 1 << " (C" << winner[r] + 1 << ")\t\t: ";
		for (int g = 0; g < GENE; g++) {
			parents[r][g] = chromosome[winner[r]][g];
			cout << setw(2) << parents[r][g] << " ";
		}
		cout << endl;
	}

	cout << endl;
}

void crossover() {
	int crossover_point;
	double probability = 0.0;

	//copy parents to children
	for (int p = 0; p < 2; p++) {
		for (int g = 0; g < GENE; g++) {
			children[p][g] = parents[p][g];
		}
	}

	//generate a probability
	probability = (rand() % 11) / 10.0;
	cout << "\n\nProbability = " << probability << endl;
	if (probability <= CROSSOVER_PROBABILITY) //crossover happen
	{
		int coins[2] = {};

		// Assign parents to head and tail
		coins[0] = 0;
		coins[1] = 1;
		// show the head and tail parent
		cout << "\nHead = Parent " << coins[0] + 1 << endl;
		cout << "Tail = Parent " << coins[1] + 1 << endl;
		int flip;
		for (int g = 0; g < GENE; g++) {
			flip = rand() % 2; // determine which gene do the first child obtain

			children[0][g] = parents[coins[flip]][g]; // assigned the gene obtain to the first child
			children[1][g] = parents[coins[1 - flip]][g]; // assign another gene to the second child
		}
		cout << endl;
	}

	// display the result of crossover
	for (int p = 0; p < 2; p++)
	{
		cout << "Children " << p + 1 << ":\t";
		for (int g = 0; g < GENE; g++) {
			cout << setw(2) << children[p][g] << " ";
		}
		cout << endl;
	}
}

void mutation() {
	int a;
	float randomValue;
	int mutation_bit_1, mutation_bit_2;

	for (int child = 0; child < 2; child++) {
		randomValue = (rand() % 11) / 10.0;
		cout << "\nValue of random number generated for children " << child + 1 << " is: " << randomValue << endl;
		if (randomValue < MUTATION_PROBABILITY) {
			mutation_bit_1 = rand() % GENE;
			mutation_bit_2 = rand() % GENE;
			cout << "The first random mutation position generated is: " << mutation_bit_1 << endl;
			cout << "The second random mutation position generated is: " << mutation_bit_2 << endl;
			if (mutation_bit_2 == mutation_bit_1) {
				mutation_bit_2 = rand() % GENE;
				cout << "The second random mutation position generated is: " << mutation_bit_2 << endl;
			}

			a = children[child][mutation_bit_1];
			children[child][mutation_bit_1] = children[child][mutation_bit_2];
			children[child][mutation_bit_2] = a;
		}
	}

	cout << "\n";

	for (int c = 0; c < 2; c++) {
		cout << "Mutated chromosome " << c + 1 << " is: ";
		for (int g = 0; g < GENE; g++) {
			cout << children[c][g] << " ";
		}
		cout << "\n";
	}
}

void survivalSelection() {
	//Copy children to the survival chromosome array
	for (int p = 0; p < 2; p++) {
		for (int g = 0; g < GENE; g++) {
			survivor[counterSurvivor][g] = children[p][g];
		}
		counterSurvivor++;
	}

	//Update array counter
	//Print chromosome in the new population
	for (int p = 0; p < POP_SIZE; p++) {
		cout << "\nSurvivor " << p + 1 << " is: ";
		for (int g = 0; g < POP_SIZE; g++) {
			cout << survivor[p][g] << " ";
		}
		cout << "\n";
	}
}

void copyChromosome() {
	//Copy newChromosome to chromosome
	for (int p = 0; p < POP_SIZE; p++) {
		for (int g = 0; g < GENE; g++) {
			chromosome[p][g] = survivor[p][g];
		}
	}

	for (int p = 0; p < POP_SIZE; p++) {
		cout << "\nNew chromosome " << p + 1 << " is: ";
		for (int g = 0; g < GENE; g++) {
			cout << chromosome[p][g] << " ";
		}
		cout << "\n";
	}
}

void calculateAverageFitness() {

	float totalFitness = 0;
	for (int i = 0; i < POP_SIZE; i++) {
		totalFitness = totalFitness + fitness_value[i];
	}
	averageFitness = totalFitness / POP_SIZE;
	cout << " \n\t Average fitness = : " << averageFitness;
	ACP_File << averageFitness << endl;


}

void recordBestFitness()
{
	for (int i = 0; i < POP_SIZE; i++) {
		if (fitness_value[i] > bestFitness)
		{
			bestFitness = fitness_value[i];
			for (int j = 0; j < GENE; j++)
			{
				bestChromosome[j] = chromosome[i][j];
			}
		}
	}

	cout << "\n\tBest Chromosome = " << bestFitness;
	BSF_File << bestFitness << endl;
	cout << "\n\t Best Chromosome : ";
	for (int j = 0; j < GENE; j++)
	{
		bestChromosome_File << bestChromosome[j] << " ";
	}
	bestChromosome_File << endl;
}