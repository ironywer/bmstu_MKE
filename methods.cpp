#include "methods.h"
#include "utils.h"
#include <iomanip>

int restrictType1(std::vector<std::vector<double>> &matrix, std::vector<double> &vector, int a, int len, int line, double value)
{
    for (auto &elem : matrix[line])
    {
        elem = 0;
    }
    matrix[line][line] = 1;
    vector[line] = value;
    return 0;
}

int restrictType2(std::vector<double> &matrix, std::vector<double> &vector, int a, int len, int line, double value)
{
    std::cout << vector[line] << std::endl;
    vector[line] += a * (line ? -value : value);
    std::cout << vector[line] << std::endl;
    return 0;
}

Vector generateXVector(const size_t nodesCount)
{
    const double dx = L / (nodesCount - 1);
    Vector result(nodesCount);
    for (size_t i = 0; i < nodesCount; ++i)
    {
        result[i] = X1 + i * dx;
    }
    return result;
}

Vector analyticalSolution(const Vector &xValues) // проверено
{
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
    auto computeY = [C1, C2, L2, a](double x)
    {
        return C1 + C2 * exp(L2 * x) + a * x;
    };

    // Генерация результата
    Vector result(xValues.size());
    transform(xValues.begin(), xValues.end(), result.begin(), computeY);

    return result;
}

MatrixVector generateLinearElementModel(const size_t elementsCount)
{
    const double dx = L / (elementsCount - 1);
    // cout << "dx = " << dx << "; L = " << L << "; elemets =" << elementsCount << endl;

    // Локальная матрица жесткости
    Matrix localMatrixRigidity(2, Vector(2));
    // localMatrixRigidity[0][0] = -A / dx - B / 2;
    // localMatrixRigidity[0][1] = A / dx + B / 2;
    // localMatrixRigidity[1][0] = A / dx - B / 2;
    // localMatrixRigidity[1][1] = -A / dx + B / 2;
    localMatrixRigidity[0][0] = A / dx + B / 2;
    localMatrixRigidity[0][1] = -A / dx - B / 2;
    localMatrixRigidity[1][0] = -A / dx + B / 2;
    localMatrixRigidity[1][1] = A / dx - B / 2;

    // Локальный вектор нагрузок
    Vector localLoadsVector(2);
    localLoadsVector[0] = C * dx / 2;
    localLoadsVector[1] = C * dx / 2;

    // Возвращаем результат
    return {localMatrixRigidity, localLoadsVector};
}

MatrixVector generateCubicElementModel(const size_t size)
{
    const double dx = L / (size -1.) * 3;

    Matrix localMatrixRigidity(size, Vector(size));
    // localMatrixRigidity[0][0] = A * 37 / (10 * dx) + B * 1 / 2;
    // localMatrixRigidity[0][1] = -A * 13 / (40 * dx) - B * 7 / 80;
    // localMatrixRigidity[1][0] = -A * 13 / (40 * dx) + B * 7 / 80;
    // localMatrixRigidity[1][1] = A * 37 / (10 * dx) - B * 1 / 2;

    Vector localLoadsVector(size);
    // localLoadsVector[0] = C * dx / 8;
    // localLoadsVector[1] = C * dx / 8;
    for (int i = 0; i < size - 3; i += 3)
    {
        cout << i << " " << size - 3 << endl;
        localMatrixRigidity[i][i] += -A * 37 / (10. * dx) - B / 2.;
        localMatrixRigidity[i][i + 1] += A * 189 / (40. * dx) + B * 57. / 80.;
        localMatrixRigidity[i][i + 2] += -A * 27 / (20. * dx) - B * 3. / 10.;
        localMatrixRigidity[i][i + 3] += A * 13 / (40. * dx) + B * 7. / 80.;

        localMatrixRigidity[(i + 1)][i] += A * 189 / (40. * dx) - B * 57. / 80.;
        localMatrixRigidity[(i + 1)][i + 1] += -A * 54 / (5. * dx);
        localMatrixRigidity[(i + 1)][i + 2] += A * 297 / (40. * dx) + B * 81. / 80.;
        localMatrixRigidity[(i + 1)][i + 3] += -A * 27 / (20. * dx) - B * 3. / 10.;

        localMatrixRigidity[(i + 2)][i] += -A * 27 / (20. * dx) + B * 3. / 10.;
        localMatrixRigidity[(i + 2)][i + 1] += A * 297 / (40. * dx) - B * 81. / 80.;
        localMatrixRigidity[(i + 2)][i + 2] += -A * 54 / (5 * dx);
        localMatrixRigidity[(i + 2)][i + 3] += A * 189 / (40 * dx) + B * 57. / 80.;

        localMatrixRigidity[(i + 3)][i] += A * 13 / (40 * dx) - B * 7. / 80.;
        localMatrixRigidity[(i + 3)][i + 1] += -A * 27 / (20 * dx) + B * 3. / 10.;
        localMatrixRigidity[(i + 3)][i + 2] += A * 189 / (40 * dx) - B * 57. / 80.;
        localMatrixRigidity[(i + 3)][i + 3] += -A * 37 / (10 * dx) + B / 2.;

        localLoadsVector[i] -= C * dx / 8.;
        localLoadsVector[i + 1] -= C * 3 * dx / 8.;
        localLoadsVector[i + 2] -= C * 3 * dx / 8.;
        localLoadsVector[i + 3] -= C * dx / 8.;
    }

    // Возвращаем результат
    return {localMatrixRigidity, localLoadsVector};
}

pair<Matrix, Vector> assemble(const Matrix &localMatrix, const Vector &localVector, const size_t n, char method)
{
    // Инициализация глобальной матрицы и вектора
    Matrix matrix(n + 1, Vector(n + 1, 0.0));
    Vector vector(n + 1, 0.0);

    // Ансамблирование матрицы
    for (size_t i = 0; method == 'l' && i < n; ++i)
    {
        matrix[i][i] += localMatrix[0][0];
        matrix[i][i + 1] += localMatrix[0][1];
        matrix[i + 1][i] += localMatrix[1][0];
        matrix[i + 1][i + 1] += localMatrix[1][1];
    }

    // Ансамблирование вектора
    for (size_t i = 0; method == 'l' && i < n; ++i)
    {
        vector[i] += localVector[0];
        vector[i + 1] += localVector[1];
    }

    if (method == 'c')
    {
        matrix = localMatrix;
        vector = localVector;
    }

    // Граничные условия
    // Условие на левом краю: y(X1) = Y1
    // vector[0] = Y1;
    // matrix[0][0] = 1.0;
    // matrix[0][1] = 0.0;
    // matrix[0][2] = 0.0;
    // matrix[0][3] = 0.0;

    // // Условие на правом краю: y(X2) = Y2
    // vector[n] = Y2;
    // matrix[n][n] = 1.0;
    // matrix[n][n - 1] = 0.0;
    // matrix[n][n - 2] = 0.0;
    // matrix[n][n - 3] = 0.0;

    restrictType1(matrix, vector, 2., n + 1, 0, Y1);
    restrictType1(matrix, vector, 2., n+1, n, Y2);

    for (size_t i = 0; i < n + 1; i++)
    {
        for (size_t j = 0; j < n + 1; j++)
        {
            cout << setprecision(4) << setw(3) << matrix[i][j] << " ";
        }
        cout << endl;
    }

    return {matrix, vector};
}

Vector MKESolution(const MatrixVector &elementModel, const size_t elementsCount, char method)
{
    // Ансамблирование глобальной матрицы и вектора
    auto [assembledMatrix, assembledVector] =
        assemble(elementModel.first, elementModel.second, elementsCount - 1, method);
    vector<double> resultVector(assembledVector.size(), 0);
    // Решение системы методом Гаусса
    if (solveSLAU(resultVector, assembledMatrix, assembledVector) != 0)
    {
        std::cerr << "Error: Singular matrix encountered during Gaussian elimination.\n";
        exit(EXIT_FAILURE);
    }

    // Возвращаем решение
    return resultVector; // assembledVector содержит результат
}
