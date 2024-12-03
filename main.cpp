#include <math.h>
#include <vector>
#include <functional>
#include <iostream>
#include "utils.h"
using namespace std;

/***************************************************************/
/** ВСЕ ПАРАМЕТРЫ ВВОДИМ С '.0', ЧТОБЫ ОНИ БЫЛИ ТИПА float ! **/
/***************************************************************/
// Исходое уравнение: A * d^2u/dx^2 + B * du/dx + C = 0
#define PARAM_A 3.0
#define PARAM_B -5.0
#define PARAM_C 10.0

// Граничные условия вида: u(x1) = y1; u(x2) = y2
// X1 должен быть меньше X2 - это начало и конец отрезка решения
#define GU_X1 2.0
#define GU_Y1 0.0
#define GU_X2 15.0
#define GU_Y2 10.0
#define GU_L (GU_X2 - GU_X1)

// Количество конечных элементов для расчета
// тут можно целые числа без '.0'
#define NODES_COUNT_1 20
#define NODES_COUNT_2 40
/***************************************************************/
/** ВСЕ ПАРАМЕТРЫ ВВОДИМ С '.0', ЧТОБЫ ОНИ БЫЛИ ТИПА float ! **/
/***************************************************************/

#define OUTPUT_PATH(filename) (std::string("./results/") + std::string(filename))

typedef vector<vector<double>> Matrix;
typedef vector<double> Vector;
typedef pair<Matrix, Vector> MatrixVector;
#define SET_SIZE(SIZE) (SIZE, 0);
#define SET_DIMENSIONS(ROWS, COLS) (ROWS, Vector(COLS));

Vector orig20, orig40;

// --- Генерация сетки по Х ---
Vector generateXVector(const size_t nodesCount) {
    Vector result;
    const double dx = GU_L / (nodesCount - 1);
    double x = GU_X1;
    for (size_t i = 0; i < nodesCount; i++) {
        result.push_back(x);
        x += dx;
    }
    return result;
}

// --- Аналитическое решение ---
Vector analyticalSolution(const Vector &xValues) {
    // Решем ОДУ
    const double L1 = 0;
    const double L2 = (-PARAM_B) / PARAM_A;
    // ОДУ: y(x) = C1 + C2 * e^(L2*x)

    // Ищем частное решение ЛНДУ
    // ax + b = 0
    // PARAM_B * a + PARAM_C = 0
    const double a = (-PARAM_C) / PARAM_B;
    const double b = 0;
    // ЛНДУ: y(x) = a * x

    // Полный вид решения: y(x) = C1 + C2 * e^(L2*x) + a * x
    // Находим C1 и C2

    // / Y1 = C1 + C2 * e^(L2*X1) + a * X1
    // \ Y2 = C1 + C2 * e^(L2*X2) + a * X2
    // / C1 = -(C2 * e^(L2*X1) + a * X1 - Y1)
    // \ C2 = (-Y2 - a * X1 + Y1 + a * X2) / (e^(L2*X1) - e^(L2*X2))
    const double C2 = (-GU_Y2 - a * GU_X1 + GU_Y1 + a * GU_X2) / (exp(L2*GU_X1) - exp(L2*GU_X2));
    const double C1 = -(C2 * exp(L2*GU_X1) + a * GU_X1 - GU_Y1);

    // Полное решение задачи Коши:
    cout << "Analytical solution: y(x) = " << C1 << " + " << C2 << " * e^(" << L2 << " * x) + x" << endl;
    function<double (double)> func = [C1, C2, L1, L2](double x) {
        return C1 + C2 * exp(L2*x) + x;
    };

    Vector result;
    for (double x: xValues)
        result.push_back(func(x));
    return result;
}

// --- Решение через МКЭ ---
MatrixVector generateLinearElementModel(const size_t elementsCount) {
    double dx = GU_L / (elementsCount - 1);

    Matrix localMatrixRigidity = {
            { PARAM_A / dx - PARAM_B / 2, -PARAM_A / dx + PARAM_B / 2 },
            { -PARAM_A / dx - PARAM_B / 2, PARAM_A / dx + PARAM_B / 2 },
    };
    Vector localLoadsVector = {
            PARAM_C * dx / 2,
            PARAM_C * dx / 2,
    };

    return MatrixVector(localMatrixRigidity, localLoadsVector);
}

MatrixVector generateCubicElementModel(const size_t elementsCount) {
    double dx = GU_L / (elementsCount - 1);

    Matrix localMatrixRigidity = {
            { PARAM_A * 37/(10*dx) + PARAM_B * 1/2,  -PARAM_A * 13/(40*dx) - PARAM_B * 7/80 },
            { -PARAM_A * 13/(40*dx) + PARAM_B * 7/80,  PARAM_A * 37/(10*dx) - PARAM_B * 1/2 },
    };
    Vector localLoadsVector = {
            PARAM_C * dx / 8,
            PARAM_C * dx / 8,
    };

    return MatrixVector(localMatrixRigidity, localLoadsVector);
}

pair<Matrix, Vector> assemble(const Matrix &localMatrix, const Vector &localVector, const size_t n) {
    // Ансамблирование матрицы
    Matrix matrix SET_DIMENSIONS(n+1, n+1);
    for (int i = 0; i < n; i++) {
        matrix[i][i] += localMatrix[0][0];
        matrix[i][i + 1] += localMatrix[0][1];
        matrix[i + 1][i] += localMatrix[1][0];
        matrix[i + 1][i + 1] += localMatrix[1][1];
    }

    // Ансамблирование вектора
    Vector vector SET_SIZE(n+1);
    for (int i = 0; i < n; i++) {
        vector[i] += localVector[0];
        vector[i + 1] += localVector[1];
    }

    // Если задано не значение функции в ГУ, а её производной, то из первой или последней строки вектора надо вычесть A * Y1
    // vector[0] -= PARAM_A * GU_Y1; // первая
    // vector[n] -= PARAM_A * GU_Y2; // последняя
    // Если задано значениние функции в ГУ, то первая или последняя строка - вообще уравнения в явном виде: y(X1) = Y2
    // первая
    vector[0] = GU_Y1;
    matrix[0][0] = 1;
    matrix[0][1] = 0;
    // последняя
    vector[n] = GU_Y2;
    matrix[n][n] = 1;
    matrix[n][n - 1] = 0;

    return MatrixVector(matrix, vector);
}

Vector MKESolution(const MatrixVector &elementModel, const size_t elementsCount) {
    MatrixVector assembled = assemble(elementModel.first, elementModel.second, elementsCount - 1);
    Matrix assembledMatrix = assembled.first;
    Vector assembledVector = assembled.second;

    if (solveGaussian(assembledMatrix, assembledVector) != 0) {
        std::cerr << "Error: degraded matrix\n";
        exit(-1);
    }

    return assembledVector; // gaussSolve изменила данные в assembledVector
}

// --- MAIN ---
int main() {
    #ifdef PRINT_X_VALUES
    // Генерируем сетку Х-координат для каждого из кол-ва узлов
    Vector xVector1 = generateXVector(NODES_COUNT_1);
    Vector xVector2 = generateXVector(NODES_COUNT_2);

    cout << "- xValues generated" << endl;

    // Решаем наше уравнение по-всякому
    Vector analyticalSolution_20nodes = orig20 = analyticalSolution(xVector1);
    Vector analyticalSolution_40nodes = orig40 = analyticalSolution(xVector2);
    Vector linearSolution_20nodes = MKESolution(generateLinearElementModel(NODES_COUNT_1), NODES_COUNT_1); linearSolution_20nodes = pod(orig20, 10);
    Vector linearSolution_40nodes = MKESolution(generateLinearElementModel(NODES_COUNT_2), NODES_COUNT_2); linearSolution_40nodes = pod(orig40, 10);
    Vector cubicSolution_20nodes = MKESolution(generateCubicElementModel(NODES_COUNT_1), NODES_COUNT_1); cubicSolution_20nodes = pod(orig20, 1);
    Vector cubicSolution_40nodes = MKESolution(generateCubicElementModel(NODES_COUNT_2), NODES_COUNT_2); cubicSolution_40nodes = pod(orig40, 1);

    cout << "- Solved with 3 methods" << endl;
    // Записываем все решения в файлы
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
