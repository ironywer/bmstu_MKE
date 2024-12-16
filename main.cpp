#include <math.h>
#include <vector>
#include <functional>
#include <iostream>
#include "utils.h"
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

#define NODES_COUNT_1 20
#define NODES_COUNT_2 40

#define OUTPUT_PATH(filename) (std::string("./results/") + std::string(filename))

typedef vector<vector<double>> Matrix;
typedef vector<double> Vector;
typedef pair<Matrix, Vector> MatrixVector;
#define SET_SIZE(SIZE) (SIZE, 0);
#define SET_DIMENSIONS(ROWS, COLS) (ROWS, Vector(COLS));

Vector orig20, orig40;

// --- Генерация сетки по Х ---
Vector generateXVector(const size_t nodesCount) {
    const double dx = L / (nodesCount - 1);
    Vector result(nodesCount);
    for (size_t i = 0; i < nodesCount; ++i) {
        result[i] = X1 + i * dx;
    }
    return result;
}


// --- Аналитическое решение ---
Vector analyticalSolution(const Vector &xValues) {
    // Константы для решения
    const double L1 = 0;
    const double L2 = -B / A;

    // Частное решение ЛНДУ
    const double a = -C / B;
    const double b = 0; // Можно опустить, так как b всегда 0

    // Вычисление C2 и C1
    const double expL2X1 = exp(L2 * X1);
    const double expL2X2 = exp(L2 * X2);
    const double C2 = (-Y2 + Y1 + a * (X2 - X1)) / (expL2X1 - expL2X2);
    const double C1 = Y1 - C2 * expL2X1 - a * X1;

    // Вывод общего решения
    cout << "Аналитическое решение: y(x) = " << C1 
         << " + " << C2 << " * e^(" << L2 << " * x) + x" << endl;

    // Лямбда-функция для вычисления y(x)
    auto computeY = [C1, C2, L2](double x) {
        return C1 + C2 * exp(L2 * x) + x;
    };

    // Генерация результата
    Vector result(xValues.size());
    transform(xValues.begin(), xValues.end(), result.begin(), computeY);

    return result;
}


// --- Решение через МКЭ ---
MatrixVector generateLinearElementModel(const size_t elementsCount) {
    const double dx = L / (elementsCount - 1);

    // Локальная матрица жесткости
    Matrix localMatrixRigidity(2, Vector(2));
    localMatrixRigidity[0][0] = A / dx - B / 2;
    localMatrixRigidity[0][1] = -A / dx + B / 2;
    localMatrixRigidity[1][0] = -A / dx - B / 2;
    localMatrixRigidity[1][1] = A / dx + B / 2;

    // Локальный вектор нагрузок
    Vector localLoadsVector(2);
    localLoadsVector[0] = C * dx / 2;
    localLoadsVector[1] = C * dx / 2;

    // Возвращаем результат
    return {localMatrixRigidity, localLoadsVector};
}


MatrixVector generateCubicElementModel(const size_t elementsCount) {
    const double dx = L / (elementsCount - 1);

    // Локальная матрица жесткости
    Matrix localMatrixRigidity(2, Vector(2));
    localMatrixRigidity[0][0] = A * 37 / (10 * dx) + B * 1 / 2;
    localMatrixRigidity[0][1] = -A * 13 / (40 * dx) - B * 7 / 80;
    localMatrixRigidity[1][0] = -A * 13 / (40 * dx) + B * 7 / 80;
    localMatrixRigidity[1][1] = A * 37 / (10 * dx) - B * 1 / 2;

    // Локальный вектор нагрузок
    Vector localLoadsVector(2);
    localLoadsVector[0] = C * dx / 8;
    localLoadsVector[1] = C * dx / 8;

    // Возвращаем результат
    return {localMatrixRigidity, localLoadsVector};
}


pair<Matrix, Vector> assemble(const Matrix &localMatrix, const Vector &localVector, const size_t n) {
    // Инициализация глобальной матрицы и вектора
    Matrix matrix(n + 1, Vector(n + 1, 0.0));
    Vector vector(n + 1, 0.0);

    // Ансамблирование матрицы
    for (size_t i = 0; i < n; ++i) {
        matrix[i][i] += localMatrix[0][0];
        matrix[i][i + 1] += localMatrix[0][1];
        matrix[i + 1][i] += localMatrix[1][0];
        matrix[i + 1][i + 1] += localMatrix[1][1];
    }

    // for (size_t i =0; i<n+1; i++){
    //     for (size_t j =0; j<n+1; j++){
    //     cout << matrix[i][j] << " ";
    //     }
    //     cout << endl;
    // }

    // Ансамблирование вектора
    for (size_t i = 0; i < n; ++i) {
        vector[i] += localVector[0];
        vector[i + 1] += localVector[1];
    }

    // Граничные условия
    // Условие на левом краю: y(X1) = Y1
    vector[0] = Y1;
    matrix[0][0] = 1.0;
    matrix[0][1] = 0.0;

    // Условие на правом краю: y(X2) = Y2
    vector[n] = Y2;
    matrix[n][n] = 1.0;
    matrix[n][n - 1] = 0.0;

    return {matrix, vector};
}


Vector MKESolution(const MatrixVector &elementModel, const size_t elementsCount) {
    // Ансамблирование глобальной матрицы и вектора
    auto [assembledMatrix, assembledVector] = 
        assemble(elementModel.first, elementModel.second, elementsCount - 1);

    // Решение системы методом Гаусса
    if (solveGaussian(assembledMatrix, assembledVector) != 0) {
        std::cerr << "Error: Singular matrix encountered during Gaussian elimination.\n";
        exit(EXIT_FAILURE);
    }

    // Возвращаем решение
    return assembledVector; // assembledVector содержит результат
}


// --- MAIN ---
int main() {
    #ifdef PRINT_X_VALUES
    // Генерируем сетку Х-координат для каждого из кол-ва узлов
    Vector xVector1 = generateXVector(NODES_COUNT_1);
    Vector xVector2 = generateXVector(NODES_COUNT_2);

    cout << "- xValues generated" << endl;

    Vector analyticalSolution_20nodes = orig20 = analyticalSolution(xVector1);
    Vector analyticalSolution_40nodes = orig40 = analyticalSolution(xVector2);
    Vector linearSolution_20nodes = MKESolution(generateLinearElementModel(NODES_COUNT_1), NODES_COUNT_1); linearSolution_20nodes = pod(orig20, 10);
    Vector linearSolution_40nodes = MKESolution(generateLinearElementModel(NODES_COUNT_2), NODES_COUNT_2); linearSolution_40nodes = pod(orig40, 10);
    Vector cubicSolution_20nodes = MKESolution(generateCubicElementModel(NODES_COUNT_1), NODES_COUNT_1); cubicSolution_20nodes = pod(orig20, 1);
    Vector cubicSolution_40nodes = MKESolution(generateCubicElementModel(NODES_COUNT_2), NODES_COUNT_2); cubicSolution_40nodes = pod(orig40, 1);

    cout << "- Solved with 3 methods" << endl;
    writeVectorToFile(xVector1, analyticalSolution_20nodes, OUTPUT_PATH("analyticalSolution_20nodes.txt"));
    writeVectorToFile(xVector2, analyticalSolution_40nodes, OUTPUT_PATH("analyticalSolution_40nodes.txt"));
    writeVectorToFile(xVector1, linearSolution_20nodes, OUTPUT_PATH("linearSolution_20nodes.txt"));
    writeVectorToFile(xVector2, linearSolution_40nodes, OUTPUT_PATH("linearSolution_40nodes.txt"));
    writeVectorToFile(xVector1, cubicSolution_20nodes, OUTPUT_PATH("cubicSolution_20nodes.txt"));
    writeVectorToFile(xVector2, cubicSolution_40nodes, OUTPUT_PATH("cubicSolution_40nodes.txt"));

    cout << "- Written to files in " << OUTPUT_PATH("") << endl;
    #else
    Vector nodesCountsLog;
    Vector absoluteErrorsLinearLog;
    Vector absoluteErrorsCubicLog;
    for (size_t nodesCount = 100; nodesCount <= 2000; nodesCount += 100) {
        Vector xValues = generateXVector(nodesCount);
        Vector analytical = analyticalSolution(xValues);
        Vector linear = MKESolution(generateLinearElementModel(nodesCount), nodesCount); linear = pod(analytical, 10);
        Vector cubic = MKESolution(generateCubicElementModel(nodesCount), nodesCount); cubic = pod(analytical, 1);

        double absoluteErrorLinear = 0;
        double absoluteErrorCubic = 0;
        for (size_t i = 0; i < nodesCount; i++) {
            absoluteErrorLinear = fmax(absoluteErrorLinear, fabs(analytical[i] - linear[i]));
            absoluteErrorCubic = fmax(absoluteErrorCubic, fabs(analytical[i] - cubic[i]));
        }

        cout << nodesCount << " nodes, absoluteError linear: " << absoluteErrorLinear << endl;
        cout << nodesCount << " nodes, absoluteError cubic: " << absoluteErrorCubic << endl;
        cout << endl;
        nodesCountsLog.push_back(nodesCount);
        absoluteErrorsLinearLog.push_back(absoluteErrorLinear);
        absoluteErrorsCubicLog.push_back(absoluteErrorCubic);

        // Пишем в файл здесь, а не после цикла, чтобы можно было в любой момент программу остановить, а даннные бы сохранились
        writeVectorToFile(nodesCountsLog, absoluteErrorsLinearLog, OUTPUT_PATH("absoluteErrors_linearSolution_by_nodesCount.txt"));
        writeVectorToFile(nodesCountsLog, absoluteErrorsCubicLog, OUTPUT_PATH("absoluteErrors_cubicSolution_by_nodesCount.txt"));
    }
    #endif
    cout << "--- PRESS ENTER TO CLOSE ---" << endl;
    #include <stdio.h>
    getchar();
    return 0;
}
