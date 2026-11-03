#include <stdio.h>
#include <string.h>
int main() {
    int n, m, start, nf, i, j, x, cur, next, idx, k, ok;
    int trans[20][10], final[20] = {0};
    char alpha[10], str[100];
    printf("Number of states (named 0 to n-1): ");
    scanf("%d", &n);
    printf("Number of input symbols: ");
    scanf("%d", &m);
    printf("Input symbols: ");
    for (i = 0; i < m; i++)
        scanf(" %c", &alpha[i]);
    /* read the transition table */
    printf("Enter the transition table:\n");
    for (i = 0; i < n; i++)
        for (j = 0; j < m; j++) {
            printf("  state %d on '%c' -> ", i, alpha[j]);
            scanf("%d", &trans[i][j]);
        }
    printf("Start state: ");
    scanf("%d", &start);
    printf("Number of final states: ");
    scanf("%d", &nf);
    printf("Final states: ");
    for (i = 0; i < nf; i++) {
        scanf("%d", &x);
        final[x] = 1;
    }
    /* display the transition table */
    printf("\nTransition Table\n");
    printf("State\t");
    for (j = 0; j < m; j++)
        printf("%c\t", alpha[j]);
    printf("\n");
    for (i = 0; i < n; i++) {
        printf("%sq%d%s\t", i == start ? "->" : "  ", i, final[i] ? "*" : " ");
        for (j = 0; j < m; j++)
            printf("q%d\t", trans[i][j]);
        printf("\n");
    }
    printf("(-> start state, * final state)\n");
    /* check strings using the table */
    while (1) {
        printf("\nEnter string to check (or 'quit'): ");
        scanf("%s", str);
        if (strcmp(str, "quit") == 0)
            break;
        cur = start;
        ok = 1;
        printf("q%d", cur);
        for (k = 0; k < (int)strlen(str); k++) {
            idx = -1;
            for (j = 0; j < m; j++)
                if (alpha[j] == str[k]) { idx = j; break; }
            if (idx == -1) {
                printf("\nInvalid symbol '%c'.\n", str[k]);
                ok = 0;
                break;
            }
            next = trans[cur][idx];
            printf(" --%c--> q%d", str[k], next);
            cur = next;
        }
        if (!ok) {
            printf("String REJECTED\n");
            continue;
        }
        if (final[cur])
            printf("\nString ACCEPTED (ended in final state q%d)\n", cur);
        else
            printf("\nString REJECTED (ended in non-final state q%d)\n", cur);
    }
    return 0;
}