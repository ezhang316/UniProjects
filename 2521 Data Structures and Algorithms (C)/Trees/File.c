#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// Helper function declaration
int is_punctuation(int character);

// Given a collectionFilename, copies and normalizes all files into identical files with a 
// '1' added to the end to make it different
void Normalize_files(char *collectionFilename) {

    // Opening given collection file
    FILE *collection_file = fopen(collectionFilename, "r");
    
    // Error check for opening file.
    if (collection_file == NULL) {
        perror(collectionFilename);
        exit(1);
    }
    
    // Reading entire collectionFilename
    while (fgetc(collection_file) != EOF) {
        // adjusting current position
        fseek(collection_file, -1, SEEK_CUR);
        
        // getting name of file to be opened and opening it
        char current_file_name[MAX_FILE_NAME_LENGTH];
        fgets(current_file_name, MAX_FILE_NAME_LENGTH, collection_file);
        
        // getting rid of \n in name of file
        for (int i = 0; i < MAX_FILE_NAME_LENGTH; i++) {
            if (current_file_name[i] == '\n') {
                current_file_name[i] = '\0';
                break;
            }
        }
        
        FILE *current_file = fopen(current_file_name, "r");
        if (collection_file == NULL) {
            perror(collectionFilename);
            exit(1);
        }
        
        // Creating new file for normalized words to be written to
        char normalized_file_name[MAX_FILE_NAME_LENGTH + 1];
        strcpy(normalized_file_name, current_file_name);
        int length = strlen(normalized_file_name);
        normalized_file_name[length] = '1';
        normalized_file_name[length + 1] = '\0';
        
        FILE *normalized_file = fopen(normalized_file_name, "w");
        if (collection_file == NULL) {
            perror(normalized_file_name);
            exit(1);
        }
        // Reading, normalising and writing each word in given file to 
        // normalized_file_name.
        int current_letter = fgetc(current_file);
        
        
        while (current_letter != EOF) {
            
            // skips spaces and new lines
            if (current_letter == ' ' || current_letter == '\n') {
                current_letter = fgetc(current_file);
            }
            else {
                int punctuation_before = 0;
                char current_word[MAX_WORD_LENGTH];
                int i = 0;
                while (current_letter != ' ' && current_letter != '\n' && current_letter != EOF) {
                    if (current_letter >= 'A' && current_letter <= 'Z') {
                        current_letter = current_letter - 'A' + 'a';
                        punctuation_before = 0;
                    }
                    else if (!is_punctuation(current_letter)) {
                        punctuation_before = 0;
                    }
                    else {
                        punctuation_before += 1;
                    }
                    current_word[i] = current_letter;
                    i++;
                    current_letter = fgetc(current_file);
                }
                // adding null terminator at end of current word
                current_word[i] = '\0';
                
                // Getting rid of punctuation at end of word
                if (punctuation_before) {
                    for (int j = 1; j < punctuation_before + 1; j++) {
                        current_word[i - j] = '\0';
                    }
                }
                // Writing valid normalized word + a space to new file
                if (strlen(current_word) != 0) {
                    if (fputs(current_word, normalized_file) == EOF) {
                        perror(normalized_file_name);
                        exit(1);
                    }
                    if (fputc(' ', normalized_file) == EOF) {
                        perror(normalized_file_name);
                        exit(1);
                    }
                }
            }
        }
        
        // Closing current file and it's normalised counterpart
        if(fclose(current_file) == EOF) {
            perror(current_file_name);
            exit(1);
        }
        if(fclose(normalized_file) == EOF) {
            perror(normalized_file_name);
            exit(1);
        }
    }
    if(fclose(collection_file) == EOF) {
        perror(collectionFilename);
        exit(1);
    }
}

// Checks whether a given character is classified as punctuation
int is_punctuation(int character) {
    if (character == '.' || character == ',') {
        return 1;
    }
    else if (character == ':' || character == ';') {
        return 1;
    }
    else if (character == '?' || character == '*') {
        return 1;
    }
    else {
        return 0;
    }
}


