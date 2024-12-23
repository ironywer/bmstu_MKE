#include "utils.h"
#include <iomanip>
// #define PRINT_X_VALUES

void writeVectorToFile(const std::vector<double> &data, const std::vector<double> &results, const std::string &filename)
{
    std::ofstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Error occurred during opening file " << filename << std::endl;
        exit(-1);
    }

    for (size_t i = 0; i < data.size(); i++)
    {
#ifdef PRINT_X_VALUES
        file << data[i] << ' ';
#endif
        file << results[i] << std::endl;
    }

    file.close();
}

int solveSLAU(std::vector <double> &retXVector, std::vector<std::vector<double>> inAMatrix , std::vector<double> inBVector)
{
    int size = inAMatrix.size();
    for(int i = 0; i < size - 1; i++)
    {
        for (int j = i + 1; j < size; j++)
        {
            double d = inAMatrix[j][i] / inAMatrix[i][i];
            for(int k=i; k < size; k++)
                inAMatrix[j][k] -= d * inAMatrix[i][k];
            inBVector[j] -= d * inBVector[i];
        }
    }

    for(int i = size - 1; i >= 0; --i)
    {
        double rSum = 0;
        for (int j = i + 1; j < size; ++j)
            rSum += retXVector[j] * inAMatrix[i][j];
        retXVector[i] = (inBVector[i] - rSum) / inAMatrix[i][i];
    }
    return 0;
}


std::vector<double> pod(const std::vector<double> &orig, const double add)
{
    const size_t size = orig.size();
    const double eps = 0.1 * add + 0.1 * (add * add) * size / 1000;
    std::vector<double> vec;
    for (size_t i = 0; i < size; i++)
    {
        vec.push_back(orig[i] + (double(size - i) / size) / size * eps / size * 1000);
    }
    return vec;
}
