#include "methods.h"
#include "utils.h"

Vector generateXVector(const size_t nodesCount) {
    const double dx = L / (nodesCount - 1);
    Vector result(nodesCount);
    for (size_t i = 0; i < nodesCount; ++i) {
        result[i] = X1 + i * dx;
    }
    return result;
}


Vector analyticalSolution(const Vector &xValues) {
    // Константы для решения
    const double L1 = 0;
    const double L2 = -B / A;

    // Частное решение ЛНДУ
    const double a = -C / B;
    const double b = 0; 

    // Вычисление C2 и C1
    const double expL2X1 = exp(L2 * X1);
    const double expL2X2 = exp(L2 * X2);
    const double C2 = (-Y2 + Y1 + a * (X2 - X1)) / (expL2X1 - expL2X2);
    const double C1 = Y1 - C2 * expL2X1 - a * X1;

    // // Вывод общего решения
    // cout << "Аналитическое решение: y(x) = " << C1 
    //      << " + " << C2 << " * e^(" << L2 << " * x) + x" << endl;

    // Лямбда-функция для вычисления y(x)
    auto computeY = [C1, C2, L2](double x) {
        return C1 + C2 * exp(L2 * x) + x;
    };

    // Генерация результата
    Vector result(xValues.size());
    transform(xValues.begin(), xValues.end(), result.begin(), computeY);

    return result;
}


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


    Matrix localMatrixRigidity(2, Vector(2));
    localMatrixRigidity[0][0] = A * 37 / (10 * dx) + B * 1 / 2;
    localMatrixRigidity[0][1] = -A * 13 / (40 * dx) - B * 7 / 80;
    localMatrixRigidity[1][0] = -A * 13 / (40 * dx) + B * 7 / 80;
    localMatrixRigidity[1][1] = A * 37 / (10 * dx) - B * 1 / 2;


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
