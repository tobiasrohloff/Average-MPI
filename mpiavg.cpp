#include <iostream>
#include <fstream>
#include <vector>
#include <sys/time.h>
#include <cstdlib>
#include <sstream>
#include <string>
#include <cmath>

#include <mpi.h>

using namespace std;

int main(int argc, char * argv[])
{
	// arg check
	if (argc != 4) {
		fprintf(stderr, "Wrong parameter\n");
		return EXIT_FAILURE;
	}

	int myid, gid, np, ierr;
	int irank, drank;

	// create group ranks
	irank = atoi(argv[2]);
	drank = atoi(argv[3]);

	int* iranks = new int[irank];
	int* dranks = new int[drank];
	for (int i = 0; i < irank; i++)
		iranks[i] = i;
	for (int i = 0; i < drank; i++)
		dranks[i] = i+irank;

	// init MPI
	MPI_Group orig_group, new_group; 
	MPI_Comm new_comm; 

	ierr = MPI_Init (&argc, &argv); 
	if (ierr != MPI_SUCCESS) {
		cout << "MPI_Init failed: " << ierr << endl;
	}

	MPI_Comm_size(MPI_COMM_WORLD, &np);
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);

	// arg check
	if (np != irank + drank) {
		fprintf(stderr, "Wrong parameter\n");
		return EXIT_FAILURE;
	}

	// comm root measures runtime
	timeval start, end;
	if (myid == 0) {
		gettimeofday(&start, 0);
	}

	vector<double> numbers;
	int size, range;

	// group roots read numbers
	if (myid == 0) {
		string value;
		ifstream file_numbers(argv[1]);
		if (file_numbers.is_open()) {
			while (getline(file_numbers, value)) {
				istringstream i(value);
				double x;
				i >> x;
				numbers.push_back(floor(x));
			}
			file_numbers.close();
		}
		// range calculation to match data size with "irank" size
		size = numbers.size();
		range = 1;
		if (size < irank) {
			for (int i = 0; i < irank - size; i++) {
				numbers.push_back(0);
			}
		} else if (size > irank) {
			range = ceil((double) size / irank);
			for (int i = 0; i < irank * range - size; i++) {
				numbers.push_back(0);
			}
		}
	} else if (myid == irank) {
		string value;
		ifstream file_numbers(argv[1]);
		if (file_numbers.is_open()) {
			while (getline(file_numbers, value)) {
				istringstream i(value);
				double x;
				i >> x;
				numbers.push_back(x);
			}
			file_numbers.close();
		}
		// range calculation to match data size with "drank" size
		size = numbers.size();
		range = 1;
		if (size < drank) {
			for (int i = 0; i < drank - size; i++) {
				numbers.push_back(0);
			}
		} else if (size > drank) {
			range = ceil((double) size / drank);
			for (int i = 0; i < drank * range - size; i++) {
				numbers.push_back(0);
			}
		}
	}

	// extract the original group
	MPI_Comm_group(MPI_COMM_WORLD, &orig_group); 

	// divide tasks into two distinct groups based upon rank 
	if (myid < irank)
		MPI_Group_incl(orig_group, irank, iranks, &new_group);
	else
		MPI_Group_incl(orig_group, drank, dranks, &new_group); 

	// create new communicator
	MPI_Comm_create(MPI_COMM_WORLD, new_group, &new_comm);

	// spread values
	MPI_Bcast(&range, 1, MPI_INT,
		0, new_comm);
	double* values = new double[range];
	MPI_Scatter(&numbers[0], range, MPI_DOUBLE, 
		values, range, MPI_DOUBLE, 
		0, new_comm);

	// sum up values
	double local_sum = 0;
	for (int i = 0; i < range; i++)
		local_sum += values[i];

	// reduce sum
	double global_sum = 0;
	MPI_Reduce(&local_sum, &global_sum, 1, 
		MPI_DOUBLE, MPI_SUM, 
		0, new_comm);

	// roots measure average from sums
	double avg = global_sum / size;

	// roots create output file
	if (myid == 0) {
		#ifdef DEBUG
		cout << "Integer-precision Average: " << avg << endl;
		#endif
		ofstream output_file("output.txt");
		if (output_file.is_open()) {
			output_file << avg << endl;
			output_file.close();
		} 
		else
			cout << "Unable to open file" << endl;
	}
	// barrier to sync write access and the correct order of the output data
	MPI_Barrier(MPI_COMM_WORLD);
	if (myid == irank) {
		#ifdef DEBUG
		cout << "Double-precision Average: " << avg << endl;
		#endif
		ofstream output_file("output.txt", ios_base::app);
		if (output_file.is_open()) {
			output_file << avg << endl;
			output_file.close();
		} 
		else
			cout << "Unable to open file" << endl;
	}

    // comm root measures runtime, barrier to sync runtime of all nodes
    MPI_Barrier(MPI_COMM_WORLD);
	if (myid == 0) {
		gettimeofday(&end, 0);
		double elapsed = (end.tv_sec - start.tv_sec) + ((end.tv_usec - start.tv_usec)/1000000.0);
		cout << "Runtime: " << elapsed << " s" << endl;
	}

	MPI_Finalize();
}
