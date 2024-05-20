// Author Kyle Koeller
// Created on May 20, 2024
// This script is meant to show the speed of multi-threading caluclations vs single threading calculations

#include <iostream>
#include <thread>
#include <vector>
#include <cmath>

using namespace std;

// function to calculate the square of each element in a subarray
void calculateSquare( const vector<int>&input, vector<int>&output, int start, int end ) {
	for (int i = start; i < end; ++i) {
		output[i] = input[i] * input[i];
	}
}

int main() {
	const int numElements = 210000000;

	vector<int> numbers(numElements);
	for (int i = 0; i < numElements; ++i) {
		numbers[i] = i + 1;
	}

	// vector to store results
	vector<int> squares(numElements);

	// single threaded results
	auto startSingle = chrono::high_resolution_clock::now();
	for (int i = 0; i < numElements; ++i) {
		squares[i] = numbers[i] * sqrt(numbers[i]);
	}
	auto endSingle = chrono::high_resolution_clock::now();
	chrono::duration<double> singleDuration = endSingle - startSingle;

	const int numThreads = thread::hardware_concurrency(); //number of CPU cores
	cout << "Number of Threads: " << numThreads << endl;
	// vector to store the thread objects
	vector<thread> threads;

	// divide the work among the threads
	int chunkSize = numElements / numThreads;
	int start = 0;
	int end = 0;

	auto startMulti = chrono::high_resolution_clock::now();
	for (int i = 0; i < numThreads; ++i) {
		start = i * chunkSize;
		end = (i == numThreads - 1) ? numElements : (i + 1) * chunkSize;
		threads.push_back(thread(calculateSquare, ref(numbers), ref(squares), start, end));
	}

	//join all the threads with the main thread
	for (auto& thread : threads) {
		thread.join();
	}
	auto endMulti = chrono::high_resolution_clock::now();
	chrono::duration<double> multiDuration = endMulti - startMulti;


	cout << "Single-Threaded computation took " << singleDuration.count() << " seconds." << endl;
	cout << "Multi-Threaded computation took " << multiDuration.count() << " seconds." << endl;
	/* output results
	for (int i = 0; i < numElements; ++i) {
		cout << "Square of " << numbers[i] << " is " << squares[i] << endl;
	}
	*/
	return 0;
}
