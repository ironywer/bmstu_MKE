#ifndef SRC_UTILS_H
#define SRC_UTILS_H

#include <fstream>
#include <filesystem>
#include <vector>
#include <string>
#include <iostream>

void writeVectorToFile(const std::vector<double> &data, const std::vector<double> &results, const std::string &filename);
int solveSLAU(std::vector <double> &retXVector, std::vector<std::vector<double>>inAMatrix , std::vector<double> inBVector);
std::vector<double> pod(const std::vector<double> &orig, const double add);

#endif //SRC_UTILS_H
