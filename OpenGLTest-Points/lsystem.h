#ifndef GEN_DATA_H
#define GEN_DATA_H

#include <string>
#include <vector>

enum LSystemCode { //raccomended number of iterations
	CUSTOM_SYSTEM,
	FRACTAL_TREE, //6
	SIERPINSKI_TRIANGLE, //6
	SIERPINSKI_TRIANGLE_AH, //arrow head curve 6
	DRAGON_CURVE,//6
	FRACTAL_PLANT, //5
	HILBERT_CURVE, //6
	SIMPLE_CURVE1, //3
	SIMPLE_CURVE2, //4
	BUSHES1, //5
	BUSHES2, //4
	BUSHES3, //3
	BUSHES4, //11
	CRYSTALS, //3
	SNOWFLAKE1, // 3
};

/*Main function*/
//generates binary file with vertices and gives back the name
//return blank string if has an error
void lsGenData(unsigned int choice, unsigned int numberOfIterations, std::string output_filename);
void printLSystemOptions();
std::string getLSystemFileName(const LSystemCode lsystemcode, const unsigned int numberOfIterations);


typedef std::pair<std::string, std::string> rule;
class LSystem {
private: 
	std::string _status, _drawing_variables; //default drawing variable F
	std::vector<rule> _rules;
	std::vector<std::pair<std::string, unsigned int>> getRulesInstances();
	float _turning_angle, _starting_angle;
public:
	LSystem();
	LSystem(const std::string *status, const std::vector<std::pair<std::string, std::string>> *rules, const std::string *drawing_variables, const float turning_angle);
	LSystem(const char *status, const std::vector<std::pair<std::string, std::string>> rules, const char *drawing_variables, const float turning_angle);
	void doIterations(const unsigned int numberOfIterations);
	std::vector<std::array<float, 3>> *translateStatus();

	void setStatus(const std::string *status);
	void setRules(const std::vector<rule> *rules);
	void addRule(const std::string *condition, const std::string *expansion);
	void setStartingAngle(const float starting_angle);
	void setTurningAngle(const float turning_angle);
	void setDrawingVariables(const std::string *drawing_variables);

	std::string getStatus();
	std::vector<std::pair<std::string, std::string>> getRules();
	float getStartingAngle();
};
#endif // !GENDATA_H
