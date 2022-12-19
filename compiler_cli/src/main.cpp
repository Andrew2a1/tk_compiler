#include <driver.h>

int main()
{
    Driver drv;
    drv.trace_parsing = true;
    drv.trace_scanning = true;

    int result = drv.parse("test.txt");
    if (result == 0)
    {
        std::cout << std::endl << "RESULT: " << std::endl;
        std::cout << drv.result << std::endl;
    }

    return result;
}