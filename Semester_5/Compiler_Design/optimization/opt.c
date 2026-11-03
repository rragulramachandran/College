#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUADS 100

struct quadruple
{
    char result[32];
    char operator[32];
    char operand1[32];
    char operand2[32];
};

void display(struct quadruple *q, int n)
{
    int i;
    printf("RI\tOP\tO1\tO2\tRES\n");
    for (i = 0; i < n; i++)
    {
        printf("%d\t%s\t%s\t%s\t%s\n", i, q[i].operator, q[i].operand1, q[i].operand2, q[i].result);
    }
}

int readNumber(const char *text, int *value)
{
    int i = 0;

    if (text[i] == '+' || text[i] == '-')
        i++;

    if (text[i] == '\0')
        return 0;

    for (; text[i] != '\0'; i++)
    {
        if (!isdigit((unsigned char)text[i]))
            return 0;
    }

    *value = atoi(text);
    return 1;
}

void applyConstantFolding(struct quadruple *q, int n)
{
    int i, j, a, b;
    int value;
    for (i = 0; i < n; i++)
    {
        if (readNumber(q[i].operand1, &a) && readNumber(q[i].operand2, &b))
        {
            switch (q[i].operator[0])
            {
            case '+':
                value = a + b;
                break;
            case '-':
                value = a - b;
                break;
            case '*':
                value = a * b;
                break;
            case '/':
                if (b == 0 || (a == INT_MIN && b == -1))
                {
                    continue;
                }
                value = a / b;
                break;
            default:
                continue;
            }
            sprintf(q[i].operand1, "%d", value);
            strcpy(q[i].operand2, "---");
            strcpy(q[i].operator, "=");
        }
        if (strcmp(q[i].operator, "=") != 0 || !readNumber(q[i].operand1, &a))
        {
            continue;
        }
        for (j = i + 1; j < n; j++)
        {
            if (strcmp(q[j].operand1, q[i].result) == 0)
            {
                strcpy(q[j].operand1, q[i].operand1);
            }
            if (strcmp(q[j].operand2, q[i].result) == 0)
            {
                strcpy(q[j].operand2, q[i].operand1);
            }
            if (strcmp(q[j].result, q[i].result) == 0)
            {
                break;
            }
        }
    }
}

void eliminate(struct quadruple *q, int n)
{
    int i, j, k;
    for (i = 0; i < n - 1; i++)
    {
        if (strcmp(q[i].operand2, "---") != 0 && strcmp(q[i].result, q[i].operand1) != 0 &&
            strcmp(q[i].result, q[i].operand2) != 0)
        {
            for (j = i + 1; j < n; j++)
            {
                if (strcmp(q[i].operand1, q[j].operand1) == 0 &&
                    strcmp(q[i].operand2, q[j].operand2) == 0 &&
                    strcmp(q[i].operator, q[j].operator) == 0)
                {
                    int flag = 0;
                    for (k = i; k < j - 1; k++)
                    {
                        if (strcmp(q[i].result, q[k + 1].result) == 0 ||
                            strcmp(q[i].operand1, q[k + 1].result) == 0 ||
                            strcmp(q[i].operand2, q[k + 1].result) == 0)
                        {
                            flag = 1;
                        }
                    }
                    if (flag == 0)
                    {
                        strcpy(q[j].operand1, q[i].result);
                        strcpy(q[j].operand2, "---");
                        strcpy(q[j].operator, "=");
                    }
                }
            }
        }
    }
}

void applyCopyPropagation(struct quadruple *q, int n)
{
    int i, j;

    for (i = 0; i < n; i++)
    {
        if (strcmp(q[i].operator, "=") == 0)
        {
            for (j = i + 1; j < n; j++)
            {
                if (strcmp(q[j].operand1, q[i].result) == 0)
                {
                    strcpy(q[j].operand1, q[i].operand1);
                }

                if (strcmp(q[j].operand2, q[i].result) == 0)
                {
                    strcpy(q[j].operand2, q[i].operand1);
                }

                if (strcmp(q[j].result, q[i].result) == 0 ||
                    strcmp(q[j].result, q[i].operand1) == 0)
                {
                    break;
                }
            }
        }
    }
}

void applyDeadCodeElimination(struct quadruple *q, int *n)
{
    int i, j, used;

    for (i = *n - 1; i >= 0; i--)
    {
        used = 0;

        for (j = i + 1; j < *n; j++)
        {
            if (strcmp(q[i].result, q[j].operand1) == 0 || strcmp(q[i].result, q[j].operand2) == 0)
            {
                used = 1;
                break;
            }

            if (strcmp(q[i].result, q[j].result) == 0)
            {
                break;
            }
        }

        if (used == 0 && q[i].result[0] == 't')
        {
            for (j = i; j < *n - 1; j++)
            {
                q[j] = q[j + 1];
            }

            (*n)--;
        }
    }
}

int main(void)
{
    struct quadruple quad[MAX_QUADS];
    int n, i;

    printf("Enter the number of quadruples: ");
    if (scanf("%d", &n) != 1 || n < 1 || n > MAX_QUADS)
    {
        fprintf(stderr, "Enter a number from 1 to %d.\n", MAX_QUADS);
        return 1;
    }
    printf("Format: Operator Operand-1 Operand-2 Result\n");
    printf("Use --- for an unused operand; names beginning with t are temporary.\n");
    for (i = 0; i < n; i++)
    {
        printf("Enter the quadruple [%d]: ", i);
        if (scanf("%31s %31s %31s %31s", quad[i].operator, quad[i].operand1, quad[i].operand2,
                  quad[i].result) != 4)
        {
            fprintf(stderr, "Invalid quadruple.\n");
            return 1;
        }
    }

    printf("\nTAC in Quadruples\n");
    display(quad, n);
    eliminate(quad, n);
    printf("\nAfter Common Subexpression Elimination\n");
    display(quad, n);
    applyConstantFolding(quad, n);
    printf("\nAfter Constant Folding\n");
    display(quad, n);
    applyCopyPropagation(quad, n);
    printf("\nAfter Copy Propagation\n");
    display(quad, n);
    applyDeadCodeElimination(quad, &n);
    printf("\nAfter Dead Code Elimination\n");
    display(quad, n);
    return 0;
}