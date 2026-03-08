

#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Helper function declaration
InvertedIndexBST InvertedIndexNodeNew(void);
FileList FileNodeNew(void);
TfIdfList TfIdfNodenew(void);

// Part 1

InvertedIndexBST generateInvertedIndex(char *collectionFilename) {
    // Creates new files with words normalized and separated by a space
    Normalize_files(collectionFilename);
    
    InvertedIndexBST tree_head = InvertedIndexNodeNew();
    
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
        char current_file_name[MAX_FILE_NAME_LENGTH + 1];
        fgets(current_file_name, MAX_FILE_NAME_LENGTH + 1, collection_file);
        
        // getting rid of \n in name of file
        for (int i = 0; i < MAX_FILE_NAME_LENGTH; i++) {
            if (current_file_name[i] == '\n') {
                current_file_name[i] = '\0';
                break;
            }
        }
        
        // Adding a 1 to end of file name
        int length = strlen(current_file_name);
        current_file_name[length] = '1';
        current_file_name[length + 1] = '\0';

        // Opening normalized file
        FILE *current_file = fopen(current_file_name, "r");
        if (collection_file == NULL) {
            perror(collectionFilename);
            exit(1);
        }
        
        // Changing variable current_file_name to match original file's name
        current_file_name[length] = '\0';
        
        // goes through entire normalized file
        char current_word[MAX_WORD_LENGTH];
        int current_letter = fgetc(current_file);
        int word_count = 0;
        int current_word_length = 0;
        while (current_letter != EOF) {
        
            // gets next word in normalized file and puts it in current_word
            int i;
            for (i = 0; i < MAX_WORD_LENGTH; i++) {
                current_word[i] = current_letter;
                current_letter = fgetc(current_file);
                if (current_letter == ' ') {
                    break;
                }
            }
            current_word[i + 1] = '\0';
            
            // Check for empty word
            current_word_length = strlen(current_word);
            if (current_word_length == 0) {
                break;
            }
            
            
            // If BST is empty, inserts word in at root
            if (tree_head->word == NULL) {
                tree_head->word = malloc((strlen(current_word) + 1)*sizeof(char));
                if (tree_head->word == NULL) {
                    perror(current_word);
                    exit(1);
                }
                strcpy(tree_head->word, current_word);
                FileList new_file_node = FileNodeNew();
                new_file_node->filename = malloc((strlen(current_file_name) + 1)*sizeof(char));
                if (new_file_node->filename == NULL) {
                    perror(current_file_name);
                    exit(1);
                }
                strcpy(new_file_node->filename, current_file_name);
                //new_file_node->tf += 1; Commented out as it will add one later in code.
                tree_head->fileList = new_file_node;
            }
            
            // If word doesn't exist in BST, creates node and file list 
            // and inserts it in order
            InvertedIndexBST word_node = FindWordNode(current_word, tree_head);
            if ((word_node == NULL)) {
                InvertedIndexBST new_node = InvertedIndexNodeNew();
                FileList new_file_node = FileNodeNew();
                
                new_file_node->tf += 1;
                char *new_file_name = malloc((strlen(current_file_name) + 1)*sizeof(char));
                if (new_file_name == NULL) {
                    perror(current_file_name);
                    exit(1);
                }
                strcpy(new_file_name, current_file_name);
                new_file_node->filename = new_file_name;
                
                new_node->fileList = new_file_node;
                new_node->word = current_word;
                char *new_node_word = malloc((strlen(current_word) + 1)*sizeof(char));
                if (new_node_word == NULL) {
                    perror(current_word);
                    exit(1);
                }
                strcpy(new_node_word, current_word);
                new_node->word = new_node_word;
                
                InsertWordNode(current_word, tree_head, new_node);
            }
            // If word exists in BST, searches it's node's file list and adds 1 to
            // corresponding file's tf.
            else {
                FileList file_node = FindFileNode(word_node->fileList, current_file_name);
                if (file_node != NULL) {
                    file_node->tf += 1;
                }
                // If file node doesn't exist in word, creates new file node
                // and inserts it in order to the current word node's file list.
                else {
                    FileList new_file_node = FileNodeNew();
                    new_file_node->tf += 1;
                    new_file_node->filename = malloc((strlen(current_file_name) + 1)*sizeof(char));
                    if (new_file_node->filename == NULL) {
                        perror(current_file_name);
                        exit(1);
                    }
                    strcpy(new_file_node->filename, current_file_name);
                    
                    word_node->fileList = InsertFileNode(word_node->fileList, current_file_name, new_file_node);
                }
            }
            
            // Increments word count if current word is valid.
            if (current_word_length != 0) {
                word_count++;
            }
            // Gets next word's first letter or EOF.
            current_letter = fgetc(current_file);
        }
        // Navigates to every word node, goes through it's file list
        // finds the matching file node and divides it's tf by the word count.
        NavigateAndDivide(tree_head, current_file_name, word_count);
        
        if(fclose(current_file) == EOF) {
            perror(current_file_name);
            exit(1);
        }
    }
    if(fclose(collection_file) == EOF) {
        perror(collectionFilename);
        exit(1);
    }
	return tree_head;
}

void printInvertedIndex(InvertedIndexBST tree, char *filename) {
    if (tree == NULL) {
        return;
    }
    // Opening given collection file
    FILE *file = fopen(filename, "a");
    // Error check for opening file.
    if (file == NULL) {
        perror(filename);
        exit(1);
    }
    
    
    // Print left tree's information
    printInvertedIndex(tree->left, filename);
    
    
    // Print current node's information
    fputs(tree->word, file);
    FileList current = tree->fileList;
    while (current != NULL) {
        fprintf(file, " %s (%.7lf)", current->filename, current->tf);
        current = current->next;
    }
    fputc('\n', file);
    
    if(fclose(file) == EOF) {
        perror(filename);
        exit(1);
    }
    
    // Print right tree's information
    printInvertedIndex(tree->right, filename);

    return;
    
}

void freeInvertedIndex(InvertedIndexBST tree) {

    if (tree == NULL) {
        return;
    }
    // Free left tree's information
    freeInvertedIndex(tree->left);
    // Free right tree's information
    freeInvertedIndex(tree->right);
    
    // free current node's file list
    FileList previous = tree->fileList;
    FileList current = previous->next;
    while (current != NULL) {
        free(previous->filename);
        free(previous);
        previous = current;
        current = current->next;
    }
    free(previous->filename);
    free(previous);
    
    //free current node
    free(tree->word);
    free(tree);
    return;
}

// Part 2

TfIdfList searchOne(InvertedIndexBST tree, char *searchWord, int D) {

    InvertedIndexBST word_node = FindWordNode(searchWord, tree);
    
    // If word_node is not found
    if (word_node == NULL) {
        return NULL;
    }
    
    // Iterates through fileList and gets total number of files
    int num_docs_contain = 0;
    FileList current = word_node->fileList;
    while (current != NULL) {
        num_docs_contain++;
        current = current->next;
    }
    // sets current back to current word node's file list
    current = word_node->fileList;
    
    // calculating idf
    double idf = log10((double)D/num_docs_contain);
    
    // Creates new TfIdfList using current as head
    TfIdfList new_list_head = TfIdfNodenew();
	new_list_head->filename = malloc((strlen(current->filename) + 1)*sizeof(char));
	if (new_list_head->filename == NULL) {
        perror(current->filename);
        exit(1);
    }
	strcpy(new_list_head->filename, current->filename);
	new_list_head->tfIdfSum = current->tf * idf;
	new_list_head->next = NULL;
	current = current->next;

	while (current != NULL) {
	    TfIdfList new_list_node = TfIdfNodenew();
	    new_list_node->filename = malloc((strlen(current->filename) + 1)*sizeof(char));
	    if (new_list_node->filename == NULL) {
            perror(current->filename);
            exit(1);
        }
	    strcpy(new_list_node->filename, current->filename);
	    new_list_node->tfIdfSum = current->tf * idf;
	    new_list_node->next = NULL;
	    
	    new_list_head = InsertTfIdfNode(new_list_head, current->filename, new_list_node);
	    current = current->next;
	}
	return new_list_head;
}

TfIdfList searchMany(InvertedIndexBST tree, char *searchWords[], int D) {
    // Finds how many words to be searched + 1 and initializes array to match it's length
    int i = 0;
    while (searchWords[i] != NULL) {
        i++;
    }
    TfIdfList array[i];
    i = 0;
    
    // searches for each word and puts each returned file list into a slot in array
    while (searchWords[i] != NULL) {
        array[i] = searchOne(tree, searchWords[i], D);
        i++;
    }
    
    // Creates new empty TfIdfList head
    TfIdfList new_list_head = TfIdfNodenew();

    // Iterate through each word's returned file list
    int j;
    for (j = 0; j < i; j++) {
        TfIdfList current = array[j];
        // If list is empty, copies information from current file node to head of
        // file list and sets current to current->next
        if (new_list_head->filename == NULL) {
            new_list_head->filename = malloc((strlen(current->filename) + 1)*sizeof(char));
            if (new_list_head->filename == NULL) {
                perror(current->filename);
                exit(1);
            }
            strcpy(new_list_head->filename, current->filename);
            new_list_head->tfIdfSum = current->tfIdfSum;
            // Freeing node in array
            TfIdfList tmp = current;
            current = current->next;
            free(tmp->filename);
            free(tmp);
        }
        // Iterate through each file node 
        while (current != NULL) {
            // Checks to see if current file node exists in list we are going to return
            TfIdfList result = FindTfIdfNode(new_list_head, current->filename);
            // If file doesn't exist in list
            if (result == NULL) {
                // If list is not empty, copies information from current file node to new file
                // node and inserts it in order into list
                TfIdfList new_list_node = TfIdfNodenew();
                new_list_node->filename = malloc((strlen(current->filename) + 1)*sizeof(char));
                if (new_list_node->filename == NULL) {
                    perror(current->filename);
                    exit(1);
                }
                strcpy(new_list_node->filename, current->filename);
                new_list_node->tfIdfSum = current->tfIdfSum;
                new_list_head = InsertTfIdfNode(new_list_head, new_list_node->filename, new_list_node);
            }
            // If file node is found, adds current tfIdfSum to it's tfIdfSum
            else {
                result->tfIdfSum += current->tfIdfSum;
            }
            // Makes sure increment will not seg fault + frees list in array
            if (current != NULL) {
                TfIdfList tmp = current;
                current = current->next;
                free(tmp->filename);
                free(tmp);
            }
        }
    }
    // new_list_head is unordered since tfIdfSum has been added while already inserted in order
    // second list is created so InsertTfIdfNode function can be used again
    
    // Copies TfIdfList head
    TfIdfList newer_list_head = TfIdfNodenew();
    newer_list_head->filename = malloc((strlen(new_list_head->filename) + 1)*sizeof(char));
    if (newer_list_head->filename == NULL) {
        perror(new_list_head->filename);
        exit(1);
    }
    strcpy(newer_list_head->filename, new_list_head->filename);
    newer_list_head->tfIdfSum = new_list_head->tfIdfSum;
    
    // Executes if new_list_head isn't empty
    if (new_list_head != NULL) {
        TfIdfList current = new_list_head->next;
        TfIdfList previous = NULL;
        while (current != NULL) {
            previous = current;
            current = current->next;
            previous->next = NULL;
            newer_list_head = InsertTfIdfNode(newer_list_head, previous->filename, previous);
        }
    }
    free(new_list_head->filename);
    free(new_list_head);
	return newer_list_head;
}

void freeTfIdfList(TfIdfList list) {

    if (list == NULL) {
        return;
    }
    
    // free current node's file list
    TfIdfList previous = list;
    TfIdfList current = previous->next;
    while (current != NULL) {
        free(previous->filename);
        free(previous);
        previous = current;
        current = current->next;
    }
    // free last node
    free(previous->filename);
    free(previous);
    
    return;
}
// Helper function creating empty inverted node
InvertedIndexBST InvertedIndexNodeNew(void) {
    InvertedIndexBST new_BST = malloc(sizeof(*new_BST));
    if (new_BST == NULL) {
        perror("Error mallocing new_BST: ");
        exit(1);
    }
    new_BST->word = NULL;
    new_BST->fileList = NULL;
    new_BST->left = NULL;
    new_BST->right = NULL;
    return new_BST;
}
// Helper function creating empty file node
FileList FileNodeNew(void) {
    FileList new_file_node = malloc(sizeof(*new_file_node));
    if (new_file_node == NULL) {
        perror("Error mallocing new_file_node: ");
        exit(1);
    }
	new_file_node->filename = NULL;
	new_file_node->tf = 0;
	new_file_node->next = NULL;
	return new_file_node;
}
// Helper function creating empty TfIdf Node
TfIdfList TfIdfNodenew(void) {
    TfIdfList new_TfIdfNode = malloc(sizeof(*new_TfIdfNode));
    if (new_TfIdfNode == NULL) {
        perror("Error mallocing new_TfIdfNode: ");
        exit(1);
    }
	new_TfIdfNode->filename = NULL;
	new_TfIdfNode->tfIdfSum = 0;
	new_TfIdfNode->next = NULL;
	return new_TfIdfNode;
}
