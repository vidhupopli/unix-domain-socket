#include <stdio.h>
#include <string.h>

int main()
{
    char name[50];

    strncpy(name, "Vidhu", sizeof(name) - 1); // The third arg is, dont copy into the array memory more than space available

    printf("%s\n", name);

    return 0;
}
