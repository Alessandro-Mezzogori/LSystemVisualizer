#include "generateData.h"

string lsGenData(unsigned int choice, unsigned int numberOfIterations) {
	cout << "Retrieving LSystem ";

	LSystem *lsystem = nullptr;
	string output_filename = "default.bin";
	if (choice == CUSTOM_SYSTEM) {
		lsystem = getCustomLSystem();
	}
	else {
		lsystem = getDefaultLSystems((LSystemCode)choice);
		cout << "test" << endl;
		if (lsystem == NULL) {
			cout << endl << "ERROR: coulnd't get selected L-System" << endl;
			return nullptr;
		}
		output_filename = getLSystemFileName((LSystemCode)choice, numberOfIterations);
	}

	bool write = true;
	if (choice != CUSTOM_SYSTEM) {
		ifstream file(output_filename, ios::in);
		if (file.is_open())
			write = false;
		file.close();
	}

	//write only if there's no file with the same name
	//avoids wasting recreating files for implemented L-Systems
	if (write) {
		cout << "Generating Points..." << endl << endl;
		lsystem->doIterations(numberOfIterations);
		vector<array<float, 3>> *vertexArray = lsystem->translateStatus();

		cout << "Finished generation of " << vertexArray->size() << " vertices..." << endl;
		cout << "Starting writing on default temporary file " << endl;
		ofstream file("default.bin", ios::out | ios::binary | ios::trunc);
		if (file.is_open()) {
			file.write((char*)&vertexArray->front()[0], sizeof(array<float, 3>)*vertexArray->size());
			file.close();
			cout << "Finished writing..closing file" << endl << endl << endl;
		}
		else
			cout << "Failed to open file..." << endl << endl << endl;

		delete vertexArray;
	}

	delete lsystem;
	return output_filename;
}