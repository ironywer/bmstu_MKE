#include "methods.h"
#include "utils.h"

Vector orig20, orig40;

int main()
{
#ifdef PRINT_X_VALUES

    Vector xVector1_l = generateXVector(ELEM_COUNT_1 + 1);
    Vector xVector2_l = generateXVector(ELEM_COUNT_2 + 1);
    Vector XVector1_c = generateXVector(ELEM_COUNT_1 * 3 + 1);
    Vector xVector2_c = generateXVector(ELEM_COUNT_2 * 3 + 1);

    cout << "- xValues generated" << endl;

    Vector analytical_20ns = orig20 = analyticalSolution(xVector1_l);
    Vector analytical_40ns = orig40 = analyticalSolution(xVector2_l);
    Vector linear_20ns = MKESolution(generateLinearElementModel(ELEM_COUNT_1 + 1), ELEM_COUNT_1 + 1, 'l');
    // linear_20ns = pod(orig20, 10);
    Vector linear_40ns = MKESolution(generateLinearElementModel(ELEM_COUNT_2 + 1), ELEM_COUNT_2 + 1, 'l');
    // linear_40ns = pod(orig40, 10);
    Vector cubic_20ns = MKESolution(generateCubicElementModel(ELEM_COUNT_1 * 3 + 1), ELEM_COUNT_1 * 3 + 1, 'c');
    // cubic_20ns = pod(orig20, 1);
    Vector cubic_40ns = MKESolution(generateCubicElementModel(ELEM_COUNT_2 * 3 + 1), ELEM_COUNT_2 * 3 + 1, 'c');
    // cubic_40ns = pod(orig40, 1);

    cout << "- Solved with 3 methods" << endl;
    writeVectorToFile(xVector1_l, analytical_20ns, OUTPUT_PATH("analytical_20ns.txt"));
    writeVectorToFile(xVector2_l, analytical_40ns, OUTPUT_PATH("analytical_40ns.txt"));
    writeVectorToFile(xVector1_l, linear_20ns, OUTPUT_PATH("linear_20ns.txt"));
    writeVectorToFile(xVector2_l, linear_40ns, OUTPUT_PATH("linear_40ns.txt"));
    writeVectorToFile(XVector1_c, cubic_20ns, OUTPUT_PATH("cubic_20ns.txt"));
    writeVectorToFile(xVector2_c, cubic_40ns, OUTPUT_PATH("cubic_40ns.txt"));

    double absoluteErrorLinear_20 = 0, absoluteErrorLinear_40 = 0;
    double absoluteErrorCubic_20 = 0, absoluteErrorCubic_40 = 0;
    for (size_t i = 0; i < 20; i++)
    {
        absoluteErrorLinear_20 = fmax(absoluteErrorLinear_20, fabs(analytical_20ns[i] - linear_20ns[i]));
        absoluteErrorCubic_20 = fmax(absoluteErrorCubic_20, fabs(analytical_20ns[i] - cubic_20ns[3*i]));
    }
        for (size_t i = 0; i < 40; i++)
    {
        absoluteErrorLinear_40 = fmax(absoluteErrorLinear_40, fabs(analytical_40ns[i] - linear_40ns[i]));
        absoluteErrorCubic_40 = fmax(absoluteErrorCubic_40, fabs(analytical_40ns[i] - cubic_40ns[3*i]));
    }

    cout << 20 << " nodes, максимальная абсолютная погрешность линейной формы: " << absoluteErrorLinear_20 << endl;
    cout << 20 << " nodes, максимальная абсолютная погрешность кубической формы: " << absoluteErrorCubic_20 << endl;
    cout << 40 << " nodes, максимальная абсолютная погрешность линейной формы: " << absoluteErrorLinear_40 << endl;
    cout << 40 << " nodes, максимальная абсолютная погрешность кубической формы: " << absoluteErrorCubic_40 << endl;
    cout << endl;

    cout << "- Written to files in " << OUTPUT_PATH("") << endl;
#else
    // Vector nodesCountsLog;
    // Vector absoluteErrorsLinearLog;
    // Vector absoluteErrorsCubicLog;
    // for (size_t nodesCount = 100; nodesCount <= 2000; nodesCount += 100)
    // {
    //     Vector xValues = generateXVector(nodesCount);
    //     Vector analytical = analyticalSolution(xValues);
    //     Vector linear = MKESolution(generateLinearElementModel(nodesCount), nodesCount, 'l');
    //     // linear = pod(analytical, 10);
    //     Vector cubic = MKESolution(generateCubicElementModel(nodesCount), nodesCount, 'c');
    //     // cubic = pod(analytical, 1);

    //     double absoluteErrorLinear = 0;
    //     double absoluteErrorCubic = 0;
    //     for (size_t i = 0; i < nodesCount; i++)
    //     {
    //         absoluteErrorLinear = fmax(absoluteErrorLinear, fabs(analytical[i] - linear[i]));
    //         absoluteErrorCubic = fmax(absoluteErrorCubic, fabs(analytical[i] - cubic[i]));
    //     }

    //     cout << nodesCount << " nodes, максимальная абсолютная погрешность линейной формы: " << absoluteErrorLinear << endl;
    //     cout << nodesCount << " nodes, максимальная абсолютная погрешность кубической формы: " << absoluteErrorCubic << endl;
    //     cout << endl;
    //     nodesCountsLog.push_back(nodesCount);
    //     absoluteErrorsLinearLog.push_back(absoluteErrorLinear);
    //     absoluteErrorsCubicLog.push_back(absoluteErrorCubic);

    //     writeVectorToFile(nodesCountsLog, absoluteErrorsLinearLog, OUTPUT_PATH("absoluteErrors_linearSolution_by_nodesCount.txt"));
    //     writeVectorToFile(nodesCountsLog, absoluteErrorsCubicLog, OUTPUT_PATH("absoluteErrors_cubicSolution_by_nodesCount.txt"));
    // }
#endif
#include <stdio.h>
    getchar();
    return 0;
}
