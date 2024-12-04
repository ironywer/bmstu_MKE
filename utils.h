#ifndef SRC_UTILS_H
#define SRC_UTILS_H

#include <fstream>
#include <filesystem>
#include <vector>
#include <string>
#include <iostream>

void writeVectorToFile(const std::vector<double> &data, const std::vector<double> &results, const std::string &filename);
int solveGaussian(std::vector<std::vector<double>> &m, std::vector<double> &b);
std::vector<double> pod(const std::vector<double> &orig, const double add);

#endif //SRC_UTILS_H
