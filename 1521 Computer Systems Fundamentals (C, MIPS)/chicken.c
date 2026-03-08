////////////////////////////////////////////////////////////////////////
// COMP1521 21T3 --- Assignment 2: `chicken', a simple file archiver
// <https://www.cse.unsw.edu.au/~cs1521/21T3/assignments/ass2/index.html>
//
// Written by Evan Zhang (z5383657) on 20-11-2021.
//
// 2021-11-08   v1.1    Team COMP1521 <cs1521 at cse.unsw.edu.au>

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include "chicken.h"


// ADD ANY extra #defines HERE


// ADD YOUR FUNCTION PROTOTYPES HERE
uint64_t find_pathname_length (FILE *output_stream);
uint64_t find_content_length (FILE *output_stream);

// print the files & directories stored in egg_pathname (subset 0)
//
// if long_listing is non-zero then file/directory permissions, formats & sizes are also printed (subset 0)

void list_egg(char *egg_pathname, int long_listing) {

    // Opening given file.
    FILE *output_stream = fopen(egg_pathname, "r");
    
    // error check for opening file.
    if (output_stream == NULL) {
        perror(egg_pathname);
        exit(1);
    }
    
    // Loop for running until end of egg (no checks).
    while (fgetc(output_stream) != EOF) {
    
        // If statement for if -L typed.
        if (long_listing) {
        
            // gets format of file.
            int format = fgetc(output_stream);
            
            // loop for gathering permissions into a string.
            char permissions[11];
            for (int i = 0; i < 10; i++) {
                permissions[i] = fgetc(output_stream);
            } permissions[10] = '\0';
            
            // using function for finding length of pathname.
            uint64_t pathname_length = find_pathname_length(output_stream);
            
            // loop for gathering pathname into a string.
            char pathname[pathname_length + 1];
            for (int i = 0; i < pathname_length; i++) {
                pathname[i] = fgetc(output_stream);
            } pathname[pathname_length] = '\0';
            
            // using function for finding size of content in bytes.
            uint64_t content_length = find_content_length(output_stream);
            
            // printing all gathered data.
            printf("%s  %c  %5lu  %s\n", permissions, format, content_length, pathname);
            
            // skip over hash to align with start of egg.
            fseek(output_stream, content_length + 1, SEEK_CUR);
        }
        
        // else for if -l was typed.
        else {
            // skips format and permissions.
            fseek(output_stream, 11, SEEK_CUR);
            
            // using function for finding length of pathname + goes to start of pathname.
            uint64_t pathname_length = find_pathname_length(output_stream);
            
            // loop for gathering pathname into string.
            char pathname[pathname_length + 1];
            for (int i = 0; i < pathname_length; i++) {
                pathname[i] = fgetc(output_stream);
            } pathname[pathname_length] = '\0';

            printf("%s\n", pathname);
            
            // using function for finding size of content in bytes + goes to start of content.
            uint64_t content_length = find_content_length(output_stream);
            
            // skips over content and hash number to align with start of next egglet or EOF.
            fseek(output_stream, content_length + 1, SEEK_CUR);
        }
    }
}


// check the files & directories stored in egg_pathname (subset 1)
//
// prints the files & directories stored in egg_pathname with a message
// either, indicating the hash byte is correct, or
// indicating the hash byte is incorrect, what the incorrect value is and the correct value would be

void check_egg(char *egg_pathname) {

    // Opening given egg.
    FILE *output_stream = fopen(egg_pathname, "r");
    
    // error check.
    if (output_stream == NULL) {
        perror(egg_pathname);
        exit(1);
    }
    
    // gets and checks magic number for validity, throws error and exits with status 1 if magic number incorrect.
    int magic_number;
    while ((magic_number = fgetc(output_stream)) != EOF) {
        if(magic_number != 0x63) {
            fprintf(stderr, "error: incorrect first egglet byte: 0x%x should be 0x63\n", magic_number);
            exit(1);
        }
        
        // skipping permissions and format.
        fseek(output_stream, 11, SEEK_CUR);
        
        // gets pathname string.
        uint64_t pathname_length = find_pathname_length(output_stream);
        char pathname[pathname_length + 1];
        for (int i = 0; i < pathname_length; i++) {
            pathname[i] = fgetc(output_stream);
        } pathname[pathname_length] = '\0';
        
        printf("%s - ", pathname);
        
        // gets content size in bytes.
        uint64_t content_length = find_content_length(output_stream);
        
        // skips content.
        fseek(output_stream, content_length + 1, SEEK_CUR);
        
        // calculates total bytes from start of egglet and goes back to it.
        uint64_t total_bytes = 20 + pathname_length + content_length;
        fseek(output_stream, -total_bytes-1, SEEK_CUR);
        
        // calculates hash value for each byte so far.
        uint8_t current_hash_value = 0;
        for (int i = 0; i < total_bytes; i++) {
            uint8_t byte_value = fgetc(output_stream);
            current_hash_value = egglet_hash(current_hash_value,byte_value);
        }
        
        // gets hash value stored in output_stream and compares it to calculated one.
        int hash_value = fgetc(output_stream);
        if (hash_value == current_hash_value) {
            printf("correct hash: 0x%x\n", hash_value);
        }
        else {
            printf("incorrect hash 0x%x should be 0x%x\n", current_hash_value, hash_value);
        }
    }
}


// extract the files/directories stored in egg_pathname (subset 2 & 3)

void extract_egg(char *egg_pathname) {

    // opens egg file.
    FILE *input_stream = fopen(egg_pathname, "r");
    
    // error check.
    if (input_stream == NULL) {
        perror(egg_pathname);
        exit(1);
    }
    
    // magic number check.
    int magic_number;
    while ((magic_number = fgetc(input_stream)) != EOF) {
        if(magic_number != 0x63) {
            fprintf(stderr, "error: incorrect first egglet byte: 0x%x should be 0x63\n", magic_number);
            exit(1);
        }
        
        // begin to calculate hash value for each byte written to egg_pathname (after every fputc, egglet_hash is called,
        // different from previous method).
        uint8_t current_hash_value = 0;
        
        // hashes magic number.
        current_hash_value = egglet_hash(current_hash_value, 99);
        
        // hashes format byte.
        current_hash_value = egglet_hash(current_hash_value, fgetc(input_stream));
        
        // if permission bytes in egg file correspond to their respective permission letter,
        // this function stores the respective byte into mode ready for chmodding.
        mode_t mode = 0;
        
        // gets type of file.
        int current_perm = fgetc(input_stream);
        current_hash_value = egglet_hash(current_hash_value, current_perm);
        
            // determines type of file.
            if (current_perm == 108) {// "l"
                mode |= S_IFLNK;
            }
            else if (current_perm == 45) {// "-"
                mode |= S_IFREG;
            }
            else if(current_perm == 98) {// "b"
                mode |= S_IFBLK;
            }
            else if (current_perm == 100) {// "d"
                mode |= S_IFDIR;
            }
            else if (current_perm == 99) {// "c"
                mode |= S_IFCHR;
            }
            else if (current_perm == 112) {// "p"
                mode |= S_IFIFO;
            }
            
        // loop gets permissions for user/owner from next three bytes.
        for (int i = 0; i < 3; i++){
            current_perm = fgetc(input_stream);
            current_hash_value = egglet_hash(current_hash_value, current_perm);
            
            // determines permissions for user/owner from next three bytes.
            if (current_perm == 114) {// "r"
                mode |= S_IRUSR;
            }
            else if (current_perm == 119) {// "w"
                mode |= S_IWUSR;
            }
            else if (current_perm == 120) {// "x"
                mode |= S_IXUSR;
            }
        }
        
        // loop gets and determines permissions for group from next three bytes.
        for (int i = 0; i < 3; i++){
            current_perm = fgetc(input_stream);
            current_hash_value = egglet_hash(current_hash_value, current_perm);
            // determines permissions for group from next three bytes.
            if (current_perm == 114) {// "r"
                mode |= S_IRGRP;
            }
            else if (current_perm == 119) {// "w"
                mode |= S_IWGRP;
            }
            else if (current_perm == 120) {// "x"
                mode |= S_IXGRP;
            }
        }
        
        // loop gets and determines permissions for anyone from next three bytes.
        for (int i = 0; i < 3; i++){
            current_perm = fgetc(input_stream);
            current_hash_value = egglet_hash(current_hash_value, current_perm);
            // determines permissions for anyone from next three bytes.
            if (current_perm == 114) {// "r"
                mode |= S_IROTH;
            }
            else if (current_perm == 119) {// "w"
                mode |= S_IWOTH;
            }
            else if (current_perm == 120) { // "x"
                mode |= S_IXOTH;
            }
        }
        
        // gets pathname length.
        uint64_t pathname_length = find_pathname_length(input_stream);
        
        // goes back to hash pathname length's two bytes.
        fseek(input_stream, -2, SEEK_CUR);
        current_hash_value = egglet_hash(current_hash_value, fgetc(input_stream));
        current_hash_value = egglet_hash(current_hash_value, fgetc(input_stream));
        
        // gets pathname.
        char pathname[pathname_length + 1];
        for (int i = 0; i < pathname_length; i++) {
            pathname[i] = fgetc(input_stream);
            current_hash_value = egglet_hash(current_hash_value, pathname[i]);
        } pathname[pathname_length] = '\0';

        printf("Extracting: %s\n", pathname);
        
        // creates file for current egglet.
        FILE *output_stream = fopen(pathname, "w");
        
        //error check.
        if (output_stream == NULL) {
            perror(pathname);
            exit(1);
        }
        
        // sets permissions and type for new file + error check.
        if (chmod(pathname, mode) == EOF) {
            perror(pathname);
            exit(1);
        }
        
        // finds content length in bytes.
        uint64_t content_length = find_content_length(input_stream);
        
        // goes back to hash content length's six bytes.
        fseek(input_stream, -6, SEEK_CUR);
        current_hash_value = egglet_hash(current_hash_value, fgetc(input_stream));
        current_hash_value = egglet_hash(current_hash_value, fgetc(input_stream));
        current_hash_value = egglet_hash(current_hash_value, fgetc(input_stream));
        current_hash_value = egglet_hash(current_hash_value, fgetc(input_stream));
        current_hash_value = egglet_hash(current_hash_value, fgetc(input_stream));
        current_hash_value = egglet_hash(current_hash_value, fgetc(input_stream));

        // copies contents into created file byte by byte while hashing each one.
        int c;
        for (int i = 0; i < content_length; i++) {
            c = fgetc(input_stream);
            fputc(c, output_stream);
            current_hash_value = egglet_hash(current_hash_value, c);
        }
        
        
        // gets hash value stored in input_stream and compares it to calculated one to print an error.
        int hash_value = fgetc(input_stream);
        if (hash_value != current_hash_value) {
            printf("%s - incorrect hash 0x%x should be 0x%x\n", pathname, current_hash_value, hash_value);
        }
        fclose(output_stream);
    }
    fclose(input_stream);
}


// create egg_pathname containing the files or directories specified in pathnames (subset 3)
//
// if append is zero egg_pathname should be over-written if it exists
// if append is non-zero egglets should be instead appended to egg_pathname if it exists
//
// format specifies the egglet format to use, it must be one EGGLET_FMT_6,EGGLET_FMT_7 or EGGLET_FMT_8

void create_egg(char *egg_pathname, int append, int format,
                int n_pathnames, char *pathnames[n_pathnames]) {
    // looped variable for changing fopen mode to append once loop has been through at least once (to preserve previous write).
    int looped = 0;
    
    // loop for each file to be archived.
    for (int p = 0; p < n_pathnames; p++) {
    
        // gathering metadata associated with current file to be archived + error check.
        struct stat s;
        
        // error check.
        if(stat(pathnames[p], &s) != 0) {
            perror(pathnames[p]);
            exit(1);
        }
        
        // if -a is not typed and loop has not been through once (to preserve previous write).
        char *command;
        if (append == 0 && looped != 1) {
            command = "w";
        }
        // if -a IS typed or the loop has been through once (to preserve previous write).
        else {
            command = "a";
        }
        
        // opens/creates egg archive file + error check.
        FILE *output_stream = fopen(egg_pathname, command);
        if (output_stream == NULL) {
            perror(egg_pathname);
            exit(1);
        }
        
        printf("Adding: %s\n", pathnames[p]);
        
        // begin to calculate hash value for each byte written to egg_pathname (after every fputc, egglet_hash is called).
        int current_hash_value = 0;
        
        // writes magic number as first byte.
        fputc(99, output_stream);
        current_hash_value = egglet_hash(current_hash_value, 99);
        
        // writes given format as second byte.
        fputc(format, output_stream);
        current_hash_value = egglet_hash(current_hash_value, format);
        
        // determines type of file being archived and writes the particular character to egg_pathname.
        if((S_IFLNK & s.st_mode) == S_IFLNK) {
            // symbolic link.
            fputc(108, output_stream);
            current_hash_value = egglet_hash(current_hash_value, 108);
        }
        else if(S_IFREG & s.st_mode) {
            // regular file.
            fputc(45, output_stream);
            current_hash_value = egglet_hash(current_hash_value, 45);
        }
        else if((S_IFBLK & s.st_mode) == S_IFBLK) {
            // block device.
            fputc(98, output_stream);
            current_hash_value = egglet_hash(current_hash_value,98);
        }
        else if(S_IFDIR & s.st_mode) {
            // directory.
            fputc(100, output_stream);
            current_hash_value = egglet_hash(current_hash_value, 100);
        }
        else if(S_IFCHR & s.st_mode) {
            // character device.
            fputc(99, output_stream);
            current_hash_value = egglet_hash(current_hash_value,99);
        }
        else if(S_IFIFO & s.st_mode) {
            // FIFO.
            fputc(112, output_stream);
            current_hash_value = egglet_hash(current_hash_value,112);
        }
        
        // determines permissions of file being archived and writes the particular character to egg_pathname.
        if(S_IRUSR & s.st_mode) {
            // owner has read permission.
            fputc(114, output_stream);
            current_hash_value = egglet_hash(current_hash_value,114);
        }
        else {
            // owner does not have read permission.
            fputc(45, output_stream);
            current_hash_value = egglet_hash(current_hash_value,45);
        }
        if(S_IWUSR & s.st_mode) {
            // owner has write permission.
            fputc(119, output_stream);
            current_hash_value = egglet_hash(current_hash_value,119);
        }
        else {
            fputc(45, output_stream);
            current_hash_value = egglet_hash(current_hash_value,45);
        }
        if(S_IXUSR & s.st_mode) {
            // owner has execute permission.
            fputc(120, output_stream);
            current_hash_value = egglet_hash(current_hash_value,120);
        }
        else {
            fputc(45, output_stream);
            current_hash_value = egglet_hash(current_hash_value,45);
        }
        if(S_IRGRP & s.st_mode) {
            // group has read permission.
            fputc(114, output_stream);
            current_hash_value = egglet_hash(current_hash_value,114);
        }
        else {
            fputc(45, output_stream);
            current_hash_value = egglet_hash(current_hash_value,45);
        }
        if(S_IWGRP & s.st_mode) {
            // group has write permission.
            fputc(119, output_stream);
            current_hash_value = egglet_hash(current_hash_value,119);
        }
        else {
            fputc(45, output_stream);
            current_hash_value = egglet_hash(current_hash_value,45);
        }
        if(S_IXGRP & s.st_mode) {
            // group has execute permission
            fputc(120, output_stream);
            current_hash_value = egglet_hash(current_hash_value,120);
        }
        else {
            fputc(45, output_stream);
            current_hash_value = egglet_hash(current_hash_value,45);
        }
        if(S_IROTH & s.st_mode) {
            // others has read permission.
            fputc(114, output_stream);
            current_hash_value = egglet_hash(current_hash_value,114);
        }
        else {
            fputc(45, output_stream);
            current_hash_value = egglet_hash(current_hash_value,45);
        }
        if(S_IWOTH & s.st_mode) {
            // others have write permission.
            fputc(119, output_stream);
            current_hash_value = egglet_hash(current_hash_value,119);
        }
        else {
            fputc(45, output_stream);
            current_hash_value = egglet_hash(current_hash_value,45);
        }
        if(S_IXOTH & s.st_mode) {
            // others have execute permission.
            fputc(120, output_stream);
            current_hash_value = egglet_hash(current_hash_value,120);
        }
        else {
            fputc(45, output_stream);
            current_hash_value = egglet_hash(current_hash_value,45);
        }
        
        // uses strlen() to calculate the number of characters in the to-be-archived file's name.
        uint64_t pathname_length = strlen(pathnames[p]);
        
        // converts pathname's length into little endian format.
        for (int i = 0; i < 2; i++) {
            // byte will be the lower byte of the pathname's length which will be written to the egg file.
            // each loop through writes the next byte.
            int byte = (pathname_length >> 8 * i) & 0xFF;
            fputc(byte, output_stream);
            current_hash_value = egglet_hash(current_hash_value, byte);
        }
        
        // writes the pathname character by character to the egg file.
        for (int i = 0; i < pathname_length; i++) {
            fputc(pathnames[p][i], output_stream);
            current_hash_value = egglet_hash(current_hash_value,pathnames[p][i]);
        }
        
        // gets the size of the to-be-archived file into total_size.
        long total_size = s.st_size;
        
        // converts total_size into little endian format.
        for (int i = 0; i < 6; i++) {
            // byte will be the lower byte of total_size which will be written to the egg file.
            // each loop through writes the next most significant byte.
            int byte = (total_size >> 8 * i) & 0xFF;
            fputc(byte, output_stream);
            current_hash_value = egglet_hash(current_hash_value,byte);
        }
        
        // opens to-be-archived file + error check.
        FILE *input_stream = fopen(pathnames[p], "r");
        if (input_stream == NULL) {
            perror(pathnames[p]);
            exit(1);
        }
        
        // copies contents byte by byte.
        int c;
        for (int i = 0; i < total_size; i++) {
            c = fgetc(input_stream);
            printf("c = %c\n", c);
            fputc(c, output_stream);
            current_hash_value = egglet_hash(current_hash_value,c);
        }
        
        // current hash should have been calculated for each written byte and is written as the final byte.
        printf("current_hash_value = %d\n", current_hash_value);
        fputc(current_hash_value, output_stream);
        
        // indicates loop has been through once so on next file opening, the egg file will not be truncated but instead appended to.
        looped = 1;
        
        // current file has been archived and to preserve alignment for next potential file to be archived, in particular
        // the egg file needs to be closed.
        fclose(output_stream);
        fclose(input_stream);
    }
}


// ADD YOUR EXTRA FUNCTIONS HERE
// If at byte 12 of egglet, navigates to start of path name and returns length of path name.
uint64_t find_pathname_length (FILE *output_stream) {
    uint64_t pathname_length = 0;
    // gets two bytes at a time and shifts them to their respective positions before OR'ing them with pathname_length.
    for (int i = 0; i < 1; i++) {
        uint64_t byte_1 = fgetc(output_stream);
        byte_1 = byte_1 << (8 * 2 * i);
        uint64_t byte_2 = fgetc(output_stream);
        byte_2 = byte_2 << (8 * (2 * i + 1));
        pathname_length = pathname_length | byte_1 | byte_2;
    }
    return pathname_length;
}

// If at byte 20 of egglet, navigates to file contents and returns length of file contents in bytes.
uint64_t find_content_length (FILE *output_stream) {
    uint64_t content_length = 0;
    // gets two bytes at a time and shifts them to their respective positions before OR'ing them with content_length.
    for (int i = 0; i < 3; i++) {
        uint64_t byte_1 = fgetc(output_stream);
        byte_1 = byte_1 << (8 * 2 * i);
        uint64_t byte_2 = fgetc(output_stream);
        byte_2 = byte_2 << (8 * (2 * i + 1));
        content_length = content_length | (byte_1 | byte_2);
    }
    return content_length;
}
