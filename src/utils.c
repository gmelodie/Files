#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <utils.h>


/*Reads one line of input file until '\n' or delim*/
char *read_line(FILE *stream, char delim, char lineEnd, int fieldType) {
    int i = 0, reallocCount = 0; //reallocCount guarda a qtd de reallocs ja feitos
    char letter, *string;
    string = (char *) malloc(50 * sizeof(char));
    letter = fgetc(stream);


    while (letter != delim && letter != lineEnd && letter != EOF) {
        string[i] = letter;
        i++;
        letter = fgetc(stream);
        if (i >= 50*(reallocCount +1)) {
            string = (char *) realloc(string, 50 * (reallocCount +2)*sizeof(char));
            reallocCount++;
        }
    }
    string = realloc(string, (i+1)*sizeof(char));
    string[i] = '\0';

    // Fixes size of the field if it is a FIXED_SIZE field
    if(fieldType == FIXED_FIELD) string = realloc(string, sizeof(char)*FIXED_SIZE);
    return string;
}


/*Reads an record from input file*/
void read_input_record(FILE *input, t_record *record) {
    char *temp;

    record->dominio = read_line(input, FIELD_DELIM, LINE_END, VARIABLE_FIELD);
    record->documento = read_line(input, FIELD_DELIM, LINE_END, FIXED_FIELD);
    record->nome = read_line(input, FIELD_DELIM, LINE_END, VARIABLE_FIELD);
    record->uf = read_line(input, FIELD_DELIM, LINE_END, VARIABLE_FIELD);
    record->cidade = read_line(input, FIELD_DELIM, LINE_END, VARIABLE_FIELD);
    record->dataHoraCadastro = read_line(input, FIELD_DELIM, LINE_END, FIXED_FIELD);
    record->dataHoraAtualiza = read_line(input, FIELD_DELIM, LINE_END, FIXED_FIELD);
    temp = read_line(input, FIELD_DELIM, LINE_END, VARIABLE_FIELD);
    record->ticket = atoi(temp);

    free(temp);
}


/*Writes the record into the output file*/
void write_output_record(FILE *output, t_record *record) {
    int sizeIndicator = 0;

    // Write fixed size fields
    fwrite(&record->ticket, sizeof(unsigned int), 1, output);
    fwrite(record->documento, sizeof(char), FIXED_SIZE, output);
    fwrite(record->dataHoraCadastro, sizeof(char), FIXED_SIZE, output);
    fwrite(record->dataHoraAtualiza, sizeof(char), FIXED_SIZE, output);

    // Write variable sized fields with size indicators
    sizeIndicator = strlen(record->dominio)+1;
    fwrite(&sizeIndicator, sizeof(int), 1, output);
    fwrite(record->dominio, sizeof(char), strlen(record->dominio)+1, output);

    sizeIndicator = strlen(record->nome)+1;
    fwrite(&sizeIndicator, sizeof(int), 1, output);
    fwrite(record->nome, sizeof(char), strlen(record->nome)+1, output);

    sizeIndicator = strlen(record->uf)+1;
    fwrite(&sizeIndicator, sizeof(int), 1, output);
    fwrite(record->uf, sizeof(char), strlen(record->uf)+1, output);

    sizeIndicator = strlen(record->cidade)+1;
    fwrite(&sizeIndicator, sizeof(int), 1, output);
    fwrite(record->cidade, sizeof(char), strlen(record->cidade)+1, output);
}


/*Frees all the varible sized fields of the record*/
void free_record(t_record *record) {
    free(record->documento);
    free(record->dataHoraCadastro);
    free(record->dataHoraAtualiza);
    free(record->dominio);
    free(record->nome);
    free(record->cidade);
    free(record->uf);

    free(record);
}


/*Uses the ticket to create the index file*/
void create_index_file(FILE *output, FILE *index) {
    
}


/*Reads input file and creates index and output files*/
void read_input(FILE *input, FILE *outputBest, FILE *outputWorst, FILE *outputFirst, FILE *index) {
    t_record *record;

    // Creating output file
    while(!feof(input)) {
        record = malloc(sizeof(t_record));
        read_input_record(input, record);

        if(feof(input)) {
            free_record(record);
            break;
        }

        write_output_record(outputBest, record);
        write_output_record(outputWorst, record);
        write_output_record(outputFirst, record);
        free_record(record);

    }

    //free(record);
}


/*Reads input file and generates index and output files*/
void initialize(FILE *input, FILE **outputBest, FILE **indexBest,
    FILE **outputWorst, FILE **indexWorst, FILE **outputFirst, FILE **indexFirst) {

    *outputBest = fopen("best.dat", "wb");
    *indexBest = fopen("best.idx", "wb");
    read_input(input, *outputBest, *outputWorst, *outputFirst, *indexBest);

    *outputWorst = fopen("worst.dat", "wb");
    *indexWorst = fopen("worst.idx", "wb");
    
    // read_input(input, *outputWorst, *indexWorst);

    *outputFirst = fopen("first.dat", "wb");
    *indexFirst = fopen("first.idx", "wb");
    // read_input(input, *outputFirst, *indexFirst);
    
    // Create index from output generated above
    create_index_file(*outputBest, *indexBest);
    create_index_file(*outputWorst, *indexWorst);
    create_index_file(*outputFirst, *indexFirst);
    
}


/*Closes files' descriptors*/
void close_files(FILE *input, FILE *outputBest, FILE *indexBest,
    FILE *outputWorst, FILE *indexWorst, FILE *outputFirst, FILE *indexFirst) {

    fclose(input);

    fclose(outputBest);
    fclose(indexBest);

    fclose(outputWorst);
    fclose(indexWorst);

    fclose(outputFirst);
    fclose(indexFirst);
}
