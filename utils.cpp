#include "utils.h"
// #define PRINT_X_VALUES

void writeVectorToFile(const std::vector<double> &data, const std::vector<double> &results, const std::string &filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error occurred during opening file " << filename << std::endl;
        exit(-1);
    }

    for (size_t i = 0; i < data.size(); i++) {
        #ifdef PRINT_X_VALUES
            file << data[i] << ' ';
        #endif
        file << results[i] << std::endl;
    }

    file.close();
}

int solveGaussian(std::vector<std::vector<double>> &matrix, std::vector<double> &resultVector) { 
    int numRows = matrix.size();
    std::vector<double> solution(numRows, 0.0); // Вектор для решения
    double multiplier, sum;

    // Прямой ход метода Гаусса
    for (int pivot = 0; pivot < numRows; ++pivot) {
        for (int row = pivot + 1; row < numRows; ++row) {
            multiplier = matrix[row][pivot] / matrix[pivot][pivot]; // Вычисление множителя
            for (int col = pivot; col < numRows; ++col) {
                matrix[row][col] -= multiplier * matrix[pivot][col]; // Вычитание строк
            }
            resultVector[row] -= multiplier * resultVector[pivot]; // Обновление вектора свободных членов
        }
    }

    // Обратный ход метода Гаусса
    for (int row = numRows - 1; row >= 0; --row) {
        sum = 0.0;
        for (int col = row + 1; col < numRows; ++col) {
            sum += matrix[row][col] * solution[col]; // Подсчет суммы известных значений
        }
        solution[row] = (resultVector[row] - sum) / matrix[row][row]; // Вычисление значения переменной
    }

    // Запись результата обратно в вектор результатов
    for (int i = 0; i < numRows; ++i) {
        resultVector[i] = solution[i];
    }

    return 0; // Успешное завершение
}


std::vector<double> pod(const std::vector<double> &orig, const double add) {
    const size_t size = orig.size();
    const double eps = 0.1 * add + 0.1 * (add * add) * size / 1000;
    std::vector<double> vec;
    for (size_t i = 0; i < size; i++) {
        vec.push_back(orig[i] + (double(size-i) / size) / size * eps / size * 1000);
    }
    return vec;
}
