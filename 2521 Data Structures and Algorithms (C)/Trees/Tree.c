#include <stdio.h>
#include <string.h>
#include <math.h>

int compare_float(double x, double y, double epsilon);
// Finds matching word node given a word and returns it. Returns NULL otherwise.
InvertedIndexBST FindWordNode(char *w, InvertedIndexBST tree) {
    if (tree == NULL) {
        return tree;
    }
    int compare_value = strcmp(tree->word, w);
    if (compare_value == 0) {
        return tree;
    }
    else if (compare_value < 0) {
        return FindWordNode(w, tree->right);
    }
    else {
        return FindWordNode(w, tree->left);
    }
    
}
// If word doesn't exist in BST, inserts it in order as leaf node.
void InsertWordNode(char *w, InvertedIndexBST tree, InvertedIndexBST wn) {
    int compare_value = strcmp(tree->word, w);
    if (compare_value < 0 && tree->right == NULL) {
    
        tree->right = wn;
        return;
    }
    else if (compare_value > 0 && tree->left == NULL) {
    
        tree->left = wn;
        return;
    }
    
    else if (compare_value < 0) {
        InsertWordNode(w, tree->right, wn);
        return;
    }
    else if (compare_value > 0) {
        InsertWordNode(w, tree->left, wn);
        return;
    }
}
// Returns matching file node or NULL if matching file node not found
FileList FindFileNode(FileList F, char *fn) {
    FileList current = F;
    while (current != NULL) {
        if (strcmp(current->filename, fn) != 0) {
            current = current->next;
        }
        else {
            return current;
        }
    }
    return current;
}
// Inserts file node in alphabetical order into a file list, returns new file list
FileList InsertFileNode(FileList F, char *fn, FileList FN) {
    FileList current = F;
    FileList previous = NULL;
    int compare_value;
    // goes through entire file list
    while (current != NULL) {
        compare_value = strcmp(current->filename, fn);
        // If current is bigger than the file name (fn), sets FileNode (FN) as
        // previous node to current.
        if (compare_value > 0) {
            if (previous == NULL) {
                FN->next = current;
                return FN;
            }
            else {
                previous->next = FN;
                FN->next = current;
                return F;
            }
        }
        previous = current;
        current = current->next;
    }
    previous->next = FN;
    return F;
}
// Navigates to every word node in order, goes through it's file list
// finds the matching file node and divides it's tf by the word count.
void NavigateAndDivide(InvertedIndexBST t, char *fn, int count) {
    if (t == NULL) {
        return;
    }
    NavigateAndDivide(t->left, fn, count);
    
    FileList current = FindFileNode(t->fileList, fn);
    if (current != NULL) {
        current->tf = current->tf/count;
    }
    
    NavigateAndDivide(t->right, fn, count);
    
    return;
}

// Inserts file node in descending order of tfIdfSum and if equal to a range of 0.000001, will insert
// in alphabetical order instead. Returns list with inserted item.
// Assumes non-empty list
TfIdfList InsertTfIdfNode(TfIdfList L, char *fn, TfIdfList TIN) {
    TfIdfList current = L;
    TfIdfList previous = NULL;
    int compare_value;
    // goes through entire file list
    while (current != NULL) {
        if(compare_float(current->tfIdfSum, TIN->tfIdfSum, 0.000001)){
           compare_value = strcmp(current->filename, fn);
            // If current is bigger than the file name (fn), sets TIN (TIN) as
            // previous node to current.
            if (compare_value > 0) {
                if (previous == NULL) {
                    TIN->next = current;
                    return TIN;
                }
                else {
                    previous->next = TIN;
                    TIN->next = current;
                    return L;
                }
            }
        }
        else if (current->tfIdfSum < TIN->tfIdfSum) {
            if (previous == NULL) {
                TIN->next = current;
                return TIN;
            }
            else {
                previous->next = TIN;
                TIN->next = current;
                return L;
            }
        }
        previous = current;
        current = current->next;
    }
    // when end of list has been reached and no tfidf is bigger than current,
    // inserts it at end.
    previous->next = TIN;
    return L;
}
// Finds matching TfIdfNode
TfIdfList FindTfIdfNode(TfIdfList L, char *fn) {
    TfIdfList current = L;
    while (current != NULL) {
        if (strcmp(current->filename, fn) != 0) {
            current = current->next;
        }
        else {
            return current;
        }
    }
    return current;
}

//https://www.tutorialspoint.com/what-is-the-most-effective-way-for-float-and-double-comparison-in-c-cplusplus
int compare_float(double x, double y, double epsilon){
    if(fabs(x - y) < epsilon) {
        return 1; //they are same
    }
    else {
        return 0; //they are not same
    }
}

