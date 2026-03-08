// Assignment 2 21T2 COMP1511: Pokedex
// pokedex.c
//
// This program was written by Evan Zhang (z5383657)
// on 28/07/2021
//
// Version 1.0.0: Release
// Version 1.0.1: Fixed references to create_new_dexnode in comments and error messages

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "ext_save.h"

// Note you are not allowed to use the strstr function in this assignment
// There are some techniques we would like you to learn to code
// that this function makes easier.

// ----------------------------------------------
// Add any extra #includes your code needs here.
// ----------------------------------------------

#include "pokedex.h"


// ----------------------------------------------
// Add your own #defines here.
// ----------------------------------------------
#define FOUND           1
#define NOT_FOUND       0
#define MAX_NAME_LENGTH 20

// Note you are not permitted to use arrays in struct pokedex,
// you must use a linked list.
//
// The only exception is that char arrays are permitted for
// search_pokemon and functions it may call, as well as the string
// returned by pokemon_name (from pokemon.h).
//
// You will need to add fields to struct pokedex.
// You may change or delete the head field.
struct pokedex {
    struct dexnode *head;
    struct dexnode *selected;
};


// You don't have to use the provided struct dexnode, you are free to
// make your own struct instead.
//
// If you use the provided struct dexnode, you will need to add fields
// to it to store other information.
struct dexnode {
    Pokemon pokemon;
    struct dexnode *next;
    int found;
};


// ----------------------------------------------
// Add your own structs here.
// ----------------------------------------------




// ----------------------------------------------
// Add prototypes for your own functions here.
// ----------------------------------------------
void print_unknown(Pokemon pokemon);

// Creates a new dexnode struct and returns a pointer to it.
static struct dexnode *create_new_dexnode(Pokemon pokemon);


// You need to implement the following functions in their stages.
// In other words, replace the lines calling fprintf & exit with your code.
// You can find descriptions of what each function should do in pokedex.h


// Creates a new Pokedex, and returns a pointer to it.
// Note: you will need to modify this function.
Pokedex new_pokedex(void) {
    // Malloc memory for a new Pokedex, and check that the memory
    // allocation succeeded.
    Pokedex pokedex = malloc(sizeof(struct pokedex));
    assert(pokedex != NULL);

    // Set the head of the linked list to be NULL.
    // (i.e. set the Pokedex to be empty)
    pokedex->head = NULL;

    // TODO: Add your own code here.
    pokedex->selected = NULL;

    return pokedex;
}

// Create a new dexnode struct and returns a pointer to it.
//
// This function should allocate memory for a dexnode, set its pokemon
// pointer to point to the specified pokemon, and return a pointer to
// the allocated memory.
static struct dexnode *create_new_dexnode(Pokemon pokemon) {

    struct dexnode *new_dexnode = malloc (sizeof(struct dexnode));
    new_dexnode->pokemon = pokemon;
    new_dexnode->next = NULL;
    new_dexnode->found = FOUND;

    return new_dexnode;
}

//////////////////////////////////////////////////////////////////////
//                         Stage 1 Functions                        //
//////////////////////////////////////////////////////////////////////

// Add a new Pokemon to the Pokedex.
void add_pokemon(Pokedex pokedex, Pokemon pokemon) {

    
    
    struct dexnode *last_dexnode = pokedex->head;
    // Creates and adds a new pokemon to the end of the list.
    // Additionally , if the pokedex is empty, 
    // gives the added pokemon 'selected' status.
    if (last_dexnode == NULL) {
        last_dexnode = create_new_dexnode(pokemon);
        pokedex->selected = last_dexnode;
        pokedex->head = last_dexnode;
        return;
    }
    else {
        // Makes last_dexnode point to the last dexnode in the list.
        while (last_dexnode->next != NULL) {
            last_dexnode = last_dexnode->next;
        }
        last_dexnode->next = create_new_dexnode(pokemon);
    }
}

// Print out all of the Pokemon in the Pokedex.
void print_pokedex(Pokedex pokedex) {
    struct dexnode *current_dexnode = pokedex->head;
    while (current_dexnode != NULL) {
        if (current_dexnode->found == FOUND) {
            if (pokedex->selected == current_dexnode) {
                printf("--> #%03d: %s\n", 
                pokemon_id (current_dexnode->pokemon), 
                pokemon_name(current_dexnode->pokemon)
                );
            }
            else {
                printf("    #%03d: %s\n", 
                pokemon_id (current_dexnode->pokemon), 
                pokemon_name(current_dexnode->pokemon)
                );
            }
        }
        else {
            if (pokedex->selected == current_dexnode) {
                printf("--> #%03d: ", 
                pokemon_id (current_dexnode->pokemon)
                );
                print_unknown(current_dexnode->pokemon);
                printf("\n");
            }
            else {
                printf("    #%03d: ", 
                pokemon_id (current_dexnode->pokemon)
                );
                print_unknown(current_dexnode->pokemon);
                printf("\n");
            }
        }
        current_dexnode = current_dexnode->next;
    }
}

// Print out the details of the currently selected Pokemon.
void detail_current_pokemon(Pokedex pokedex) {
    
    struct dexnode *selected = pokedex->selected;
    if (selected == NULL) {
        return;
    }

    if (selected->found == FOUND) {
        printf("ID: %03d\n", pokemon_id(selected->pokemon));
        printf("Name: %s\n", pokemon_name(selected->pokemon));
        printf("Height: %lf m\n", pokemon_height(selected->pokemon));
        printf("Weight: %lf kg\n", pokemon_weight(selected->pokemon));
        printf("Type: %s", 
        type_code_to_str(pokemon_first_type(selected->pokemon))
        );
        if (pokemon_second_type(selected->pokemon) != NONE_TYPE) {
            printf(" %s\n", 
            type_code_to_str(pokemon_second_type(selected->pokemon))
            );
        }
        else {
            printf("\n");
        }
    }
    else {
        printf("ID: %03d\n", pokemon_id(selected->pokemon));
        printf("Name: ");
        print_unknown(selected->pokemon);
        printf("\n");
        printf("Height: --\n");
        printf("Weight: --\n");
        printf("Type: --\n");
    }
}

// Return the currently selected Pokemon.
Pokemon get_current_pokemon(Pokedex pokedex) {
    if (pokedex->selected != NULL) {   
        return pokedex->selected->pokemon;
    }
    else {
        return NULL;
    }
}

// Change the currently selected Pokemon to be the next Pokemon in the Pokedex.
void next_pokemon(Pokedex pokedex) {
    struct dexnode *next = pokedex->selected;
    if (next == NULL || next->next == NULL) {
        return;
    }
    else {
        pokedex->selected = next->next;
    }
}

// Change the currently selected Pokemon to be the previous Pokemon in the Pokedex.
void prev_pokemon(Pokedex pokedex) {
    
    // Returns nothing if there is one or less item in the list.
    struct dexnode *current = pokedex->selected;
    struct dexnode *previous = NULL;
    struct dexnode *scroll_through = pokedex->head;
    if (scroll_through == NULL || scroll_through->next == NULL) {
        return;
    }
    // Activates if there is more than one item, making scroll_through
    // equal the selected dexnode/pokemon and previous equal the one
    // before it in the list.
    else {
        while (scroll_through != current) {
            previous = scroll_through;
            scroll_through = scroll_through->next;
        }
        // If the selected pokemon is the head of the list, nothing will
        // happen. If it isn't then it changes the selected pokemon to
        // the one before the currently selected one.
        if (previous != NULL) {
            pokedex->selected = previous;
        }
    }
}

// Change the currently selected Pokemon to be the Pokemon with the ID `id`.
void jump_pokemon(Pokedex pokedex, int id) {
    // Returns nothing if the list is empty.
    if (pokedex->head == NULL) {
        return;
    }
    // Starting from the head of the list, continues down until it finds the
    // pokemon with a matching ID, then making it selected.
    struct dexnode *current_dexnode = pokedex->head;
    while (current_dexnode != NULL) {
        if (pokemon_id(current_dexnode->pokemon) == id) {
            pokedex->selected = current_dexnode;
            return;
        }
        current_dexnode = current_dexnode->next;
    }
    // You will only reach this point if a positive match was not found.
    if (current_dexnode == NULL) {
        printf("Could not find a pokemon by that ID\n");
    }
    
}

//////////////////////////////////////////////////////////////////////
//                         Stage 2 Functions                        //
//////////////////////////////////////////////////////////////////////

// Sets the currently selected pokemon's "found" status
void set_current_pokemon_found(Pokedex pokedex, int found) {
    // If the list is not empty, makes the selected pokemon "found".
    if (pokedex->selected != NULL) {
        pokedex->selected->found = found;
    }
}

// Add a pokemon before the current pokemon.
void insert_pokemon_before_current(Pokedex pokedex, Pokemon pokemon) {
    // Activates when list is empty, creating a new head.
    if (pokedex->head == NULL) {
        pokedex->head = create_new_dexnode(pokemon);
        pokedex->head->found = NOT_FOUND;
        pokedex->selected = pokedex->head;
        return;
    }
    // Activates when list has one item, creating a new head.
    else if (pokedex->head->next == NULL) {
        struct dexnode *new_head = create_new_dexnode(pokemon);
        new_head->found = NOT_FOUND;
        pokedex->selected = new_head;
        new_head->next = pokedex->head;
        pokedex->head = new_head;
        return;
    }
    // Activates when list has one or more items.
    else {
        // Makes current equal the selected dexnode/pokemon.
        // Makes previous equal the one before it.
        struct dexnode *current = pokedex->head;
        struct dexnode *previous = NULL;
        while (current != pokedex->selected) {
            previous = current;
            current = current->next;
        }
        // Activates when the selected dexnode/pokemon is the head.
        if (previous == NULL) {
            struct dexnode *new_head = create_new_dexnode(pokemon);
            new_head->found = NOT_FOUND;
            pokedex->selected = new_head;
            new_head->next = pokedex->head;
            pokedex->head = new_head;
        }
        // Activates when the selected dexnode/pokemon is not the head.
        else {
            previous->next = create_new_dexnode(pokemon);
            previous->next->found = NOT_FOUND;
            pokedex->selected = previous->next;
            previous->next->next = current;
        }
    }
}

// Return the total number of Pokemon in the Pokedex.
int count_pokemon_of_type(Pokedex pokedex, char *type) {

    int number_of = 0;
    struct dexnode *current_node = pokedex->head;
    
    // Returns nothing if list is empty.
    if (current_node == NULL) {
        return 0;
    }
    else {
    // Activates when list contains one or more items and continues until
    // current == NULL.
        while (current_node != NULL) {
            // Activates when a pokemon has either type given by the user.
            if (type_str_to_code(type) == pokemon_second_type(current_node->pokemon)
            ||
            type_str_to_code(type) == pokemon_first_type(current_node->pokemon)
                ) {
                number_of++;
            }
            current_node = current_node->next;
        }
        return number_of;
    }
}

//////////////////////////////////////////////////////////////////////
//                         Stage 3 Functions                        //
//////////////////////////////////////////////////////////////////////


// Remove the currently selected Pokemon from the Pokedex.
void remove_pokemon(Pokedex pokedex) {

    // Returns nothing if the list is empty.
    if (pokedex->head == NULL) {
        return;
    }
    // Activates when there is one or more items in the list.
    else {
        struct dexnode *current = pokedex->head;
        struct dexnode *previous = NULL;
        while (current != pokedex->selected) {
            previous = current;
            current = current->next;
        }
        // Activates if the selected pokemon is the head of the list.
        if (previous == NULL) {
            pokedex->head = current->next;
            pokedex->selected = current->next;
            destroy_pokemon(current->pokemon);
            free(current);
        }
        // Activates if the selected pokemon is the last in the list.
        else if (current->next == NULL) {
            previous->next = NULL;
            pokedex->selected = pokedex->head;
            destroy_pokemon(current->pokemon);
            free(current);
        }
        // Activates if the selected pokemon is neither first nor last.
        else {
            previous->next = current->next;
            destroy_pokemon(current->pokemon);
            free(current);
        }
    }
}

// Destroy the given Pokedex and free all associated memory.
void destroy_pokedex(Pokedex pokedex) {

    // Frees the pokedex given if already empty.
    if (pokedex->head == NULL) {
        free(pokedex);
        return;
    }
    // Activates only when one item is in the list.
    else if (pokedex->head->next == NULL) {
        destroy_pokemon(pokedex->head->pokemon);
        free(pokedex->head);
        free(pokedex);
    }
    // Activates when two or more items are in the list.
    else {
        struct dexnode *current = pokedex->head;
        struct dexnode *previous = NULL;
        while (current != NULL) {
            previous = current;
            current = current->next;
            destroy_pokemon(previous->pokemon);
            free(previous);
        }
        free(pokedex);
    }
}

// Prints out all types of found Pokemon stored in the Pokedex
void show_types(Pokedex pokedex) {

    // The way I describe how this function works is through brute force. 
    // A pokemon's first type will fall into one of the if/else statements.
    // Once there, it will either print or move on depending on whether it has
    // already been printed out that type already (0 == not yet printed and
    // 1 == already printed out).
    // The pokemon's second type will fall into the last half of the if/else
    // statements, which are nearly identical to the first and do the exact
    // same thing but only for the pokemon's second type.
    // The while loop does this for every pokemon in the list until 
    // current == NULL.
    
    int normal = 0;
    int fire = 0;
    int fighting = 0;
    int water = 0;
    int flying = 0;
    int grass = 0;
    int poison = 0;
    int electric = 0;
    int ground = 0;
    int psychic = 0;
    int rock = 0;
    int ice = 0;
    int bug = 0;
    int dragon = 0;
    int ghost = 0;
    int dark = 0;
    int steel = 0;
    int fairy = 0;
    // Returns nothing if list is empty.
    if (pokedex->head == NULL) {
        return;
    }
    else {
        struct dexnode *current = pokedex->head;
        while (current != NULL) {
            
            if (pokemon_first_type(current->pokemon) == NORMAL_TYPE) {
                if (normal == 0) {
                    printf("%s\n", type_code_to_str(NORMAL_TYPE));
                    normal = 1;
                }
            }
            else if (pokemon_first_type(current->pokemon) == FIRE_TYPE) {
                if (fire == 0) {
                    printf("%s\n", type_code_to_str(FIRE_TYPE));
                    fire = 1;
                }
            }
            else if (pokemon_first_type(current->pokemon) == FIGHTING_TYPE) {
                if (fighting == 0) {
                    printf("%s\n", type_code_to_str(FIGHTING_TYPE));
                    fighting = 1;
                }
            }
            else if (pokemon_first_type(current->pokemon) == WATER_TYPE) {
                if (water == 0) {
                    printf("%s\n", type_code_to_str(WATER_TYPE));
                    water = 1;
                }
            }
            else if (pokemon_first_type(current->pokemon) == FLYING_TYPE) {
                if (flying == 0) {
                    printf("%s\n", type_code_to_str(FLYING_TYPE));
                    flying = 1;
                }
            }
            else if (pokemon_first_type(current->pokemon) == GRASS_TYPE) {
                if (grass == 0) {
                    printf("%s\n", type_code_to_str(GRASS_TYPE));
                    grass = 1;
                }
            }
            else if (pokemon_first_type(current->pokemon) == POISON_TYPE) {
                if (poison == 0) {
                    printf("%s\n", type_code_to_str(POISON_TYPE));
                    poison = 1;
                }
            }
            else if (pokemon_first_type(current->pokemon) == ELECTRIC_TYPE) {
                if (electric == 0) {
                    printf("%s\n", type_code_to_str(ELECTRIC_TYPE));
                    electric = 1;
                }
            }
            else if (pokemon_first_type(current->pokemon) == GROUND_TYPE) {
                if (ground == 0) {
                    printf("%s\n", type_code_to_str(GROUND_TYPE));
                    ground = 1;
                }
            }
            else if (pokemon_first_type(current->pokemon) == PSYCHIC_TYPE) {
                if (psychic == 0) {
                    printf("%s\n", type_code_to_str(PSYCHIC_TYPE));
                    psychic = 1;
                }
            }
            else if (pokemon_first_type(current->pokemon) == ROCK_TYPE) {
                if (rock == 0) {
                    printf("%s\n", type_code_to_str(ROCK_TYPE));
                    rock = 1;
                }
            }
            else if (pokemon_first_type(current->pokemon) == ICE_TYPE) {
                if (ice == 0) {
                    printf("%s\n", type_code_to_str(ICE_TYPE));
                    ice = 1;
                }
            }
            else if (pokemon_first_type(current->pokemon) == BUG_TYPE) {
                if (bug == 0) {
                    printf("%s\n", type_code_to_str(BUG_TYPE));
                    bug = 1;
                }
            }
            else if (pokemon_first_type(current->pokemon) == DRAGON_TYPE) {
                if (dragon == 0) {
                    printf("%s\n", type_code_to_str(DRAGON_TYPE));
                    dragon = 1;
                }
            }
            else if (pokemon_first_type(current->pokemon) == GHOST_TYPE) {
                if (ghost == 0) {
                    printf("%s\n", type_code_to_str(GHOST_TYPE));
                    ghost = 1;
                }
            }
            else if (pokemon_first_type(current->pokemon) == DARK_TYPE) {
                if (dark == 0) {
                    printf("%s\n", type_code_to_str(DARK_TYPE));
                    dark = 1;
                }
            }
            else if (pokemon_first_type(current->pokemon) == STEEL_TYPE) {
                if (steel == 0) {
                    printf("%s\n", type_code_to_str(STEEL_TYPE));
                    steel = 1;
                }
            }
            else if (pokemon_first_type(current->pokemon) == FAIRY_TYPE) {
                if (fairy == 0) {
                    printf("%s\n", type_code_to_str(FAIRY_TYPE));
                    fairy = 1;
                }
            }
            
            if (pokemon_second_type(current->pokemon) == NORMAL_TYPE) {
                if (normal == 0) {
                    printf("%s\n", type_code_to_str(NORMAL_TYPE));
                    normal = 1;
                }
            }
            else if (pokemon_second_type(current->pokemon) == FIRE_TYPE) {
                if (fire == 0) {
                    printf("%s\n", type_code_to_str(FIRE_TYPE));
                    fire = 1;
                }
            }
            else if (pokemon_second_type(current->pokemon) == FIGHTING_TYPE) {
                if (fighting == 0) {
                    printf("%s\n", type_code_to_str(FIGHTING_TYPE));
                    fighting = 1;
                }
            }
            else if (pokemon_second_type(current->pokemon) == WATER_TYPE) {
                if (water == 0) {
                    printf("%s\n", type_code_to_str(WATER_TYPE));
                    water = 1;
                }
            }
            else if (pokemon_second_type(current->pokemon) == FLYING_TYPE) {
                if (flying == 0) {
                    printf("%s\n", type_code_to_str(FLYING_TYPE));
                    flying = 1;
                }
            }
            else if (pokemon_second_type(current->pokemon) == GRASS_TYPE) {
                if (grass == 0) {
                    printf("%s\n", type_code_to_str(GRASS_TYPE));
                    grass = 1;
                }
            }
            else if (pokemon_second_type(current->pokemon) == POISON_TYPE) {
                if (poison == 0) {
                    printf("%s\n", type_code_to_str(POISON_TYPE));
                    poison = 1;
                }
            }
            else if (pokemon_second_type(current->pokemon) == ELECTRIC_TYPE) {
                if (electric == 0) {
                    printf("%s\n", type_code_to_str(ELECTRIC_TYPE));
                    electric = 1;
                }
            }
            else if (pokemon_second_type(current->pokemon) == GROUND_TYPE) {
                if (ground == 0) {
                    printf("%s\n", type_code_to_str(GROUND_TYPE));
                    ground = 1;
                }
            }
            else if (pokemon_second_type(current->pokemon) == PSYCHIC_TYPE) {
                if (psychic == 0) {
                    printf("%s\n", type_code_to_str(PSYCHIC_TYPE));
                    psychic = 1;
                }
            }
            else if (pokemon_second_type(current->pokemon) == ROCK_TYPE) {
                if (rock == 0) {
                    printf("%s\n", type_code_to_str(ROCK_TYPE));
                    rock = 1;
                }
            }
            else if (pokemon_second_type(current->pokemon) == ICE_TYPE) {
                if (ice == 0) {
                    printf("%s\n", type_code_to_str(ICE_TYPE));
                    ice = 1;
                }
            }
            else if (pokemon_second_type(current->pokemon) == BUG_TYPE) {
                if (bug == 0) {
                    printf("%s\n", type_code_to_str(BUG_TYPE));
                    bug = 1;
                }
            }
            else if (pokemon_second_type(current->pokemon) == DRAGON_TYPE) {
                if (dragon == 0) {
                    printf("%s\n", type_code_to_str(DRAGON_TYPE));
                    dragon = 1;
                }
            }
            else if (pokemon_second_type(current->pokemon) == GHOST_TYPE) {
                if (ghost == 0) {
                    printf("%s\n", type_code_to_str(GHOST_TYPE));
                    ghost = 1;
                }
            }
            else if (pokemon_second_type(current->pokemon) == DARK_TYPE) {
                if (dark == 0) {
                    printf("%s\n", type_code_to_str(DARK_TYPE));
                    dark = 1;
                }
            }
            else if (pokemon_second_type(current->pokemon) == STEEL_TYPE) {
                if (steel == 0) {
                    printf("%s\n", type_code_to_str(STEEL_TYPE));
                    steel = 1;
                }
            }
            else if (pokemon_second_type(current->pokemon) == FAIRY_TYPE) {
                if (fairy == 0) {
                    printf("%s\n", type_code_to_str(FAIRY_TYPE));
                    fairy = 1;
                }
            }
            current = current->next;
        }
    }
}

// Return the number of Pokemon in the Pokedex that have been found.
int count_found_pokemon(Pokedex pokedex) {
    int many_found = 0;
    // Returns 0 if list is empty.
    if (pokedex->head == NULL) {
        return 0;
    }
    else {
        struct dexnode *current = pokedex->head;
        while (current != NULL) {
            if (current->found == FOUND) {
                many_found++;
            }
            current = current->next;
        }
    }
    return many_found;
}

// Return the total number of Pokemon in the Pokedex.
int count_total_pokemon(Pokedex pokedex) {
    int how_many = 0;
    // Returns 0 if list is empty.
    if (pokedex->head == NULL) {
        return 0;
    }
    else {
        struct dexnode *current = pokedex->head;
        while (current != NULL) {
            how_many++;
            current = current->next;
        }
    }
    return how_many;
}

// NOTE: Make sure you submit ass2_pokedex before completing these.
//////////////////////////////////////////////////////////////////////
//                     Stage 4 Extension Functions                  //
//////////////////////////////////////////////////////////////////////

/*
 * Saves a pokedex to a text file
 * Use the functions in ext_save.h to save the contents to a file
 */
void save_pokedex(Pokedex pokedex, char* filename) {
    fprintf(stderr, "exiting because you have not implemented the save_pokedex function\n");
    exit(1);

}

/*
 * Loads a pokedex from a text file
 * Use the functions in ext_save.h to load the text from a file
 */
Pokedex load_pokedex(char* filename) {
    fprintf(stderr, "exiting because you have not implemented the load_pokedex function\n");
    exit(1);

    return NULL;
}

//////////////////////////////////////////////////////////////////////
//                     Stage 5 Extension Functions                  //
//////////////////////////////////////////////////////////////////////

// Create a new Pokedex which contains only the Pokemon of a specified
// type from the original Pokedex.
Pokedex get_pokemon_of_type(Pokedex pokedex, pokemon_type type) {
    fprintf(stderr, "exiting because you have not implemented the get_pokemon_of_type function\n");
    exit(1);
}

// Create a new Pokedex which contains only the Pokemon that have
// previously been 'found' from the original Pokedex.
Pokedex get_found_pokemon(Pokedex pokedex) {
    fprintf(stderr, "exiting because you have not implemented the get_found_pokemon function\n");
    exit(1);
}

// Create a new Pokedex containing only the Pokemon from the original
// Pokedex which have the given string appearing in its name.
Pokedex search_pokemon(Pokedex pokedex, char *text) {
    fprintf(stderr, "exiting because you have not implemented the search_pokemon function\n");
    exit(1);
}

// Free's the current sub-list and returns the original pokedex state, 
// prior to the search
Pokedex end_search(Pokedex sub_pokedex) {
    fprintf(stderr, "exiting because you have not implemented the end_search function\n");
    exit(1);
}

// Add definitions for your own functions here.

void print_unknown(Pokemon pokemon) {
    char *name = pokemon_name(pokemon);
    int i = 0;
    while (name[i] != '\0') {
        printf("*");
        i++;
    }
}


