#include <stdio.h>

#define TOTAL     10

int main(void)
{
    int digs[TOTAL] = {0};
    size_t count = 0;
    size_t sz_ar = sizeof(digs) / sizeof(*digs);

    while(count < sz_ar && scanf("%d", &digs[count]) == 1)
        count++;

    // здесь продолжайте программу
    
    int index = -1;
    for(int i = 0; i < sz_ar; i++)
        if (digs[i] == 5) {
            index = i;
            break;
        }
    
    if (index == -1) {
        for (int i = 0; i < count; i++)
            printf("%d ", digs[i]);
    }
    
    else {
        for (int i = sz_ar - 1; i > index; i--) 
            digs[i] = digs[i-1];
    
        digs[index+1] = -5;

        int min = (count < sz_ar) ? count + 1 : sz_ar;

        for (int i = 0; i < min; i++)
            printf("%d ", digs[i]);
    }
 
    

    return 0;
}