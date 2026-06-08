#include <stdio.h>
#include <stdlib.h>

int main() {
    FILE *file = fopen("doctors.csv", "r");
    if (!file) return EXIT_FAILURE;

    char id[10], name[50], spec[50];


    // Read until End of File (EOF)
    // ^, means "read everything up to a comma"
    while (fscanf(file, " %[^,],%[^,],%[^\n]", id, name, spec) == 3) {
        printf("ID: %s | Doctor: %s | Spec: %s\n", id, name, spec);
    }

    fclose(file);
    return 0;
}