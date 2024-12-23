#ifndef SRC_METHODS_H
#define SRC_METHODS_H

#include <math.h>
#include <vector>
#include <functional>
#include <iostream>
using namespace std;

// Исходое уравнение: A * d^2u/dx^2 + B * du/dx + C = 0
#define A 3.0
#define B -5.0
#define C 10.0

// Граничные условия вида: u(x1) = y1; u(x2) = y2
// X1 должен быть меньше X2 - это начало и конец отрезка решения
#define X1 2.0
#define Y1 0.0
#define X2 15.0
#define Y2 10.0
#define L (X2 - X1)

#define ELEM_COUNT_1 20
#define ELEM_COUNT_2 40

#define OUTPUT_PATH(filename) (std::string("./results/") + std::string(filename))

typedef vector<vector<double>> Matrix;
typedef vector<double> Vector;
typedef pair<Matrix, Vector> MatrixVector;
#define SET_SIZE(SIZE) (SIZE, 0);
#define SET_DIMENSIONS(ROWS, COLS) (ROWS, Vector(COLS));



Vector generateXVector(const size_t nodesCount);
Vector analyticalSolution(const Vector &xValues);
MatrixVector generateLinearElementModel(const size_t elementsCount);
MatrixVector generateCubicElementModel(const size_t elementsCount);
pair<Matrix, Vector> assemble(const Matrix &localMatrix, const Vector &localVector, const size_t n, char method);
Vector MKESolution(const MatrixVector &elementModel, const size_t elementsCount, char method);


#endif