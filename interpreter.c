#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#define ENVIRONMENT_SIZE 32768

typedef struct {
    uint8_t *position;
    FILE *in;
    FILE *out;
} environment;

char *lex_file(FILE *src_file);
int interpret(char *src_code, environment *env);


int main(int argc, char **argv) {
    if (argc != 2) {
        printf("usage: %s <source.bf>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    FILE* f = fopen(argv[1], "r");
    if (f == NULL) {
        fputs("Error opening file.\n", stderr);
        exit(EXIT_FAILURE);
    }

    char* src_code = lex_file(f);

    environment *env = malloc(sizeof(environment));
    env->position = calloc(ENVIRONMENT_SIZE, 1);
    env->in = stdin;
    env->out = stdout;


    return interpret(src_code, env);
}


char *lex_file(FILE *src_file) {
    size_t file_size;
    char* buffer;

    fseek(src_file, 0, SEEK_END);
    file_size = ftell(src_file);
    rewind(src_file);

    buffer = calloc(sizeof(char), file_size+1);
    if (buffer == NULL) {
        fputs("Unable to allocate memory for file.\n", stderr);
        fclose(src_file);
        exit(EXIT_FAILURE);
    }

    fread(buffer, file_size, 1, src_file);
    fclose(src_file);
    return buffer;
}


int interpret(char *src_code, environment *env) {
    char command = 1;
    for (int i = 0; (command = src_code[i]) != '\0'; i++) {
        switch (command) {
            case '>' :
                (env->position)++;
                break;
            
            case '<':
                (env->position)--;
                break;
            
            case '+':
                (*env->position)++;
                break;
            
            case '-':
                (*env->position)--;
                break;
            
            case '.':
                fputc(*env->position, env->out);
                break;
            
            case ',':
                while ((*env->position = getc(env->in)) == '\n');
                while (getc(env->in) != '\n');
                break;
            case '[':
                int start = i;

                size_t buff_size = 0;
                while (src_code[start+buff_size] != ']') buff_size++, i++;
                char *buffer = calloc(buff_size, sizeof(char));
                for (int j = 0; src_code[start+1+j] != ']'; j++) buffer[j] = src_code[start+1+j];
                uint8_t *save_curr_pos = env->position;
                while (*save_curr_pos != 0) interpret(buffer, env);
                break;

            default:
                break;
        }
    }
    return EXIT_SUCCESS;
}
