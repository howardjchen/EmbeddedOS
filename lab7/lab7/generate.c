#include <stdio.h>
#include <stdlib.h>

int main(int argc, char const *argv[])
{
        FILE *fp1;
        int i;
        int num;

        fp1 = fopen("data.txt","w");

        for (i = 0; i < 10000; ++i)
        {
                num = rand() % 100;
                fprintf(fp1, "%d ",num );
        }

        return 0;
}
