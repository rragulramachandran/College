#include <stdio.h>
#define SIZE 2000

int main()
{
    char stuffed[SIZE], original[SIZE];
    int i, j = 0, count = 0;

    printf("Receiver - Enter stuffed binary data: ");
    scanf("%1999s", stuffed);

    for (i = 0; stuffed[i] != '\0'; i++)
    {
        original[j++] = stuffed[i];

        if (stuffed[i] == '1')
        {
            count++;

            if (count == 5)
            {
                if (stuffed[i + 1] == '0')
                    i++;

                count = 0;
            }
        }
        else
        {
            count = 0;
        }
    }

    original[j] = '\0';
    printf("Original data received: %s\n", original);

    return 0;
}
