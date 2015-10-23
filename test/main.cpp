



#include "mbed-drivers/mbed.h"


#include "output-compare/OutputCompare.h"




void app_start(int, char *[])
{
    printf("hello world\r\n");

    bool result = OutputCompare(PD7, 500);

    printf("result: %d\r\n", result);
}




