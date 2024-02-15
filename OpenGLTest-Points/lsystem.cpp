#include "lsystem.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <array>
#include <cmath>
using namespace std;

/*LSystem*/

LSystem::LSystem() {
	//make the custom requests
	_status = "0", _drawing_variables = "0";
	_rules = { {"0", "0[0]0"} };
	_turning_angle = 3.1415f / 4.0f;
	_starting_angle = 0.0f;
}

LSystem::LSystem(const string *status, const vector<pair<string, string>> *rules, const string *drawing_variables, const float turning_angle) {
	_status = *status;
	_rules = *rules;
	_drawing_variables = *drawing_variables;
	_turning_angle = turning_angle;
	_starting_angle = 0.0f;
}

LSystem::LSystem(const char *status, const std::vector<std::pair<std::string, std::string>> rules, const char *drawing_variables, const float turning_angle) {
	_status = status;
	_rules = rules;
	_drawing_variables = drawing_variables;
	_turning_angle = turning_angle;
	_starting_angle = 0.0f;
}


string LSystem::getStatus() { return _status; }
vector<rule> LSystem::getRules() { return _rules; }
float LSystem::getStartingAngle() { return _starting_angle; }

void LSystem::setStatus(const string *status) { _status = *status; }
void LSystem::setRules(const vector<rule> *rules) { _rules = *rules; }
void LSystem::setStartingAngle(const float starting_angle) { _starting_angle = starting_angle; }
void LSystem::setDrawingVariables(const std::string *drawing_variables) { _drawing_variables = *drawing_variables; }
void LSystem::setTurningAngle(const float turning_angle) { _turning_angle = turning_angle; }

void LSystem::addRule(const std::string *condition, const std::string *expansion) {
	_rules.push_back(make_pair(*condition, *expansion));
}

bool compareRulesInstances(const pair<string, unsigned int> &a, const pair<string, unsigned int> &b) {
	return a.second < b.second;
}

vector<pair<string, unsigned int>> LSystem::getRulesInstances(){
	vector<pair<string, unsigned int>> rulesInstances;
	for (const rule &rule : _rules) {
		string condition = rule.first;
		size_t foundSubStringIndex = _status.find(condition);
		while (foundSubStringIndex != string::npos) {
			rulesInstances.push_back(make_pair(rule.second, foundSubStringIndex));
			foundSubStringIndex = _status.find(condition, foundSubStringIndex + 1);
		}
	}
	sort(rulesInstances.rbegin(), rulesInstances.rend(), compareRulesInstances);
	
	return rulesInstances;
}

void LSystem::doIterations(const unsigned int numberOfIterations) {
	for (unsigned int i = 0; i < numberOfIterations; i++) {
		vector<pair<string, unsigned int>> rulesInstances = getRulesInstances();

		for (const pair<string, unsigned int> &ruleInstance : rulesInstances) {
			_status.at(ruleInstance.second) = ruleInstance.first.at(0);
			_status.insert(ruleInstance.second + 1, &ruleInstance.first.at(1));
		}
	}
}

vector<array<float, 3>> * LSystem::translateStatus() {
	vector<pair<array<float, 3>, float>> stack; //x,y,z, alpha angle
	constexpr float M_PI = 3.14159265358979323846f;

	string status = getStatus();
	vector<array<float, 3>> *vertexArray = new vector<array<float, 3>>;
	array<float, 3> currentRootVertex = { 0.0f, 0.0f, 0.0f };
	float alpha = _starting_angle; //current 
	for (const char &current : status) {
		//if is a drawing variable set new points
		if (_drawing_variables.find(current) != string::npos){
			vertexArray->push_back(currentRootVertex);
			currentRootVertex = { currentRootVertex[0] + cosf(alpha), currentRootVertex[1] + sinf(alpha), 0.0f };
			vertexArray->push_back(currentRootVertex);
		}
		else if (current == '[') { 
			stack.push_back(make_pair(currentRootVertex, alpha));
		}
		else if (current == ']') { 
			currentRootVertex = stack.back().first;
			alpha = stack.back().second;
			stack.pop_back();
		}
		else if (current == '+') { 
			alpha += _turning_angle;
		}
		else if (current == '-') { 
			alpha -= _turning_angle;
		}
	}
	return vertexArray;
}

/*#########*/

LSystem* getDefaultLSystems(LSystemCode choice) {
	LSystem *lsystem = NULL;
	constexpr float M_PI = 3.14159265358979323846f;

	switch (choice)
	{
	case FRACTAL_TREE:
		lsystem = new LSystem("0", { { "1", "11" }, { "0", "1[+0]-0" } }, "01", M_PI / 4.0f);
		lsystem->setStartingAngle(M_PI / 2.0f);
		break;
	case SIERPINSKI_TRIANGLE:
		lsystem = new LSystem("F-G-G", {{ "F", "F-G+F+G-F" }, { "G", "GG" }}, "FG", M_PI*2.0f / 3.0f);
		lsystem->setStartingAngle(0.0f);
		break;
	case SIERPINSKI_TRIANGLE_AH:
		lsystem = new LSystem("A", { { "A", "B-A-B" }, { "B", "A+B+A" } }, "AB", M_PI/3.0f);
		lsystem->setStartingAngle(0.0f);
		break;
	case DRAGON_CURVE:
		lsystem = new LSystem("FX", { { "X", "X+YF+" }, { "Y", "-FX-Y" } },"F",M_PI/2.0f);
		lsystem->setStartingAngle(0.0f);
		break;
	case FRACTAL_PLANT:
		lsystem = new LSystem("X", { { "X", "F-[[X]+X]+F[+FX]-X" }, { "F", "FF" } },"F", M_PI*25.0f/180.0f);
		lsystem->setStartingAngle(M_PI / 2.0f);
		break;
	case HILBERT_CURVE:
		lsystem = new LSystem("A", { { "A", "-BF+AFA+FB-" }, { "B", "+AF-BFB-FA+" } },"F",M_PI/2.0f);
		lsystem->setStartingAngle(0.0f);
		break;
	case SIMPLE_CURVE1: 
		lsystem = new LSystem("F+F+F+F", { { "F", "F+F-F-FF+F+F-F" } }, "F", M_PI/2.0f);
		lsystem->setStartingAngle(0.0f);
		break;
	case SIMPLE_CURVE2:
		lsystem = new LSystem("F+F+F+F", { { "F", "FF+F-F+F+FF" } },"F",M_PI / 2.0f);
		lsystem->setStartingAngle(0.0f);
		break;
	case BUSHES1:
		lsystem = new LSystem("Y", { { "X", "X[-FFF][+FFF]FX" }, { "Y", "YFX[+Y][-Y]" } },"F", 0.4385496f);
		lsystem->setStartingAngle(M_PI / 2.0f);
		break;
	case BUSHES2:
		lsystem = new LSystem("F", { { "F", "FF+[+F-F-F]-[F+F+F]" } },"F", 0.3926991f);
		lsystem->setStartingAngle(M_PI);
		break;
	case BUSHES3:
		lsystem = new LSystem("F", {{ "F", "F[+FF][-FF]F[-F][+F]F" }},"F", 0.610865f);
		lsystem->setStartingAngle(M_PI / 2.0f);
		break;
	case BUSHES4:
		lsystem = new LSystem("VZFFF", { { "V", "[+++W][---W]YV" }, {"W", "+X[-W]Z"}, {"X", "-W[+X]Z"},{"Y", "YZ"},{"Z","[-FFF][+FFF]F"} },"F", M_PI * 20 / 180);
		lsystem->setStartingAngle(M_PI / 2.0f);
		break;
	case CRYSTALS:
		lsystem = new LSystem("F+F+F+F", { { "F", "FF+F++F+F" } }, "F", M_PI/2.0f);
		lsystem->setStartingAngle(0.0f);
		break;
	case SNOWFLAKE1:
		lsystem = new LSystem("F++F++F", { { "F", "F-F++F-Fs" }}, "F", M_PI/3.0f);
		lsystem->setStartingAngle(0.0f);
		break;
	default:
		lsystem = NULL;
	}
	return lsystem;
}

LSystem* getCustomLSystem() {
	/*input control not yet implemented*/
	LSystem *lsystem = new LSystem();
	string input;
	
	cout << "Insert starting status: ";
	cin >> input;
	lsystem->setStatus(&input);

	cout << "Insert rules (type exit to stop):" << endl;
	cout << "Ex F=FF" << endl;
	while(1) {
		cin >> input;
		if (input == "exit")
			break;
		else if(input.find('=') == string::npos) {
			cout << "INVALID RULE: NO = present" << endl;
		}
		else
			lsystem->addRule(&input.substr(0, input.find('=')), &input.substr(input.find('=') + 1));
	} 
	cout << "Insert drawing variables: ";
	cin >> input;
	lsystem->setDrawingVariables(&input);

	cout << "Insert turning angle in degrees: ";
	cin >> input;
	lsystem->setTurningAngle(std::stof(input)*3.14159265359f / 180.0f); //angle in radians
	return lsystem;
}

/*Interface function*/
string getLSystemFileName(const LSystemCode lsystemcode, const unsigned int numberOfIterations){
	string filename;
	switch (lsystemcode)
	{
	case FRACTAL_TREE:
		filename = "fractal_tree_vertices";
		break;
	case SIERPINSKI_TRIANGLE:
		filename = "sierpinski_triangle_vertices";
		break;
	case SIERPINSKI_TRIANGLE_AH:
		filename = "sierpinski_triangle_ah_vertices";
		break;
	case DRAGON_CURVE:
		filename = "dragon_curve_vertices";
		break;
	case FRACTAL_PLANT:
		filename = "fractal_plant_vertices";
		break;
	case HILBERT_CURVE:
		filename = "hilbert_curve_vertices";
		break;
	case SIMPLE_CURVE1:
		filename = "simple_curve1_vertices";
		break;
	case SIMPLE_CURVE2:
		filename = "simple_curve2_vertices";
		break;
	case BUSHES1:
		filename = "bushes1_vertices";
		break;
	case BUSHES2:
		filename = "bushes2_vertices";
		break;
	case BUSHES3:
		filename = "bushes3_vertices";
		break;
	case BUSHES4:
		filename = "bushes4_vertices";
		break;
	case CRYSTALS:
		filename = "crystals_vertices";
		break;
	case SNOWFLAKE1:
		filename = "snowflake1_vertices";
		break;
	default:
		filename = "default";
	}
	return filename + to_string(numberOfIterations) + ".bin";
}


void printLSystemOptions() {
	std::cout << "Chose one followed by the number of iterations: " << std::endl << 
		"CUSTOM_SYSTEM		0" << std::endl <<
		"FRACTAL_TREE		1" << std::endl << "SIERPINSKI_TRIANGLE	2" << std::endl <<
		"SIERPINSKI_TRIANGLE_AH	3" << std::endl << "DRAGON_CURVE		4" << std::endl <<
		"FRACTAL_PLANT		5" << std::endl << "HILBERT_CURVE		6" << std::endl <<
		"SIMPLE_CURVE1		7" << std::endl << "SIMPLE_CURVE2		8" << std::endl <<
		"BUSHES1			9" << std::endl << "BUSHES2			10" << std::endl <<
		"BUSHES3			11" << std::endl << "BUSHES4			12" << std::endl <<
		"CRYSTALS		13" << std::endl << "SNOWFLAKE1		14" << std::endl;
}

void lsGenData(unsigned int choice, unsigned int numberOfIterations, std::string output_filename) {
	cout << "Retrieving LSystem ";
	
	LSystem *lsystem = nullptr;
	if (choice == CUSTOM_SYSTEM) {
		lsystem = getCustomLSystem();
	}
	else {
		lsystem = getDefaultLSystems((LSystemCode)choice);
		if (lsystem == NULL) {
			cout << endl << "ERROR: coulnd't get selected L-System" << endl;
			return;
		}
	}

	//write only if there's no file with the same name
	//avoids wasting recreating files for implemented L-Systems
	
	cout << "Generating Points..." << endl << endl;
	lsystem->doIterations(numberOfIterations);
	vector<array<float, 3>> *vertexArray = lsystem->translateStatus();

	cout << "Finished generation of " << vertexArray->size() << " vertices..." << endl;
	cout << "Starting writing on default temporary file " << endl;
	ofstream file(output_filename, ios::out | ios::binary | ios::trunc);
	if (file.is_open()) {
		file.write((char*)&vertexArray->front()[0], sizeof(array<float, 3>)*vertexArray->size());
		file.close();
		cout << "Finished writing..closing file" << endl << endl << endl;
	}
	else
		cout << "Failed to open file..." << endl << endl << endl;

	delete vertexArray;
	delete lsystem;
}