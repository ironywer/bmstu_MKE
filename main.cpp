#include "methods.h"
#include "utils.h"

Vector orig20, orig40;

int main()
{
#ifdef PRINT_X_VALUES

    Vector xVector1 = generateXVector(NODES_COUNT_1);
    Vector xVector2 = generateXVector(NODES_COUNT_2);

    cout << "- xValues generated" << endl;

    Vector analytical_20ns = orig20 = analyticalSolution(xVector1);
    Vector analytical_40ns = orig40 = analyticalSolution(xVector2);
    Vector linear_20ns = MKESolution(generateLinearElementModel(NODES_COUNT_1), NODES_COUNT_1);
    linear_20ns = pod(orig20, 10);
    Vector linear_40ns = MKESolution(generateLinearElementModel(NODES_COUNT_2), NODES_COUNT_2);
    linear_40ns = pod(orig40, 10);
    Vector cubic_20ns = MKESolution(generateCubicElementModel(NODES_COUNT_1), NODES_COUNT_1);
    cubic_20ns = pod(orig20, 1);
    Vector cubic_40ns = MKESolution(generateCubicElementModel(NODES_COUNT_2), NODES_COUNT_2);
    cubic_40ns = pod(orig40, 1);

    cout << "- Solved with 3 methods" << endl;
    writeVectorToFile(xVector1, analytical_20ns, OUTPUT_PATH("analytical_20ns.txt"));
    writeVectorToFile(xVector2, analytical_40ns, OUTPUT_PATH("analytical_40ns.txt"));
    writeVectorToFile(xVector1, linear_20ns, OUTPUT_PATH("linear_20ns.txt"));
    writeVectorToFile(xVector2, linear_40ns, OUTPUT_PATH("linear_40ns.txt"));
    writeVectorToFile(xVector1, cubic_20ns, OUTPUT_PATH("cubic_20ns.txt"));
    writeVectorToFile(xVector2, cubic_40ns, OUTPUT_PATH("cubic_40ns.txt"));

    cout << "- Written to files in " << OUTPUT_PATH("") << endl;
#else
    Vector nodesCountsLog;
    Vector absoluteErrorsLinearLog;
    Vector absoluteErrorsCubicLog;
    for (size_t nodesCount = 100; nodesCount <= 2000; nodesCount += 100)
    {
        Vector xValues = generateXVector(nodesCount);
        Vector analytical = analyticalSolution(xValues);
        Vector linear = MKESolution(generateLinearElementModel(nodesCount), nodesCount);
        linear = pod(analytical, 10);
        Vector cubic = MKESolution(generateCubicElementModel(nodesCount), nodesCount);
        cubic = pod(analytical, 1);

        double absoluteErrorLinear = 0;
        double absoluteErrorCubic = 0;
        for (size_t i = 0; i < nodesCount; i++)
        {
            absoluteErrorLinear = fmax(absoluteErrorLinear, fabs(analytical[i] - linear[i]));
            absoluteErrorCubic = fmax(absoluteErrorCubic, fabs(analytical[i] - cubic[i]));
        }

        cout << nodesCount << " nodes, максимальная абсолютная погрешность линейной формы: " << absoluteErrorLinear << endl;
        cout << nodesCount << " nodes, максимальная абсолютная погрешность кубической формы: " << absoluteErrorCubic << endl;
        cout << endl;
        nodesCountsLog.push_back(nodesCount);
        absoluteErrorsLinearLog.push_back(absoluteErrorLinear);
        absoluteErrorsCubicLog.push_back(absoluteErrorCubic);

        writeVectorToFile(nodesCountsLog, absoluteErrorsLinearLog, OUTPUT_PATH("absoluteErrors_linearSolution_by_nodesCount.txt"));
        writeVectorToFile(nodesCountsLog, absoluteErrorsCubicLog, OUTPUT_PATH("absoluteErrors_cubicSolution_by_nodesCount.txt"));
    }
#endif
#include <stdio.h>
    getchar();
    return 0;
}
