#ifndef _MARKOV_CHAIN_H
#define _MARKOV_CHAIN_H

#include "linked_list.h"
#include <stdio.h>  // For printf(), sscanf()
#include <stdlib.h> // For exit(), malloc()
#include <stdbool.h> // for bool
#define C100 100


#define ALLOCATION_ERROR_MASSAGE \
"Allocation failure: Failed to allocate new memory\n"



/***************************/
/*   insert typedefs here  */
/***************************/

typedef void (*print_func_b)(void *);
typedef int (*comp_func_b)(void *,void *);
typedef void (*free_data_b)(void *);
typedef void* (*copy_func_b)(void *);
typedef bool (*is_last_b)(void *); //TO DO
/***************************/



/***************************/
/*        STRUCTS          */
/***************************/
typedef struct MarkovNodeFrequency {
    struct MarkovNode* markov_node;
    int frequency;
}MarkovNodeFrequency;


typedef struct MarkovNode {
    void *data;
    struct MarkovNodeFrequency *frequencies_list;
    int length_of_frequencies_list;
} MarkovNode;


/* DO NOT ADD or CHANGE variable names in this struct */
typedef struct MarkovChain {
    LinkedList *database;


    print_func_b print_func;


   comp_func_b comp_func;


   free_data_b free_data;


    copy_func_b copy_func;

    is_last_b is_last;
} MarkovChain;

/**
 * Get one random state from the given markov_chain's database.
 * @param markov_chain
 * @return
 */
MarkovNode* get_first_random_node(MarkovChain *markov_chain);

/**
 * Choose randomly the next state, depend on it's occurrence frequency.
 * @param state_struct_ptr MarkovNode to choose from
 * @return MarkovNode of the chosen state
 */
MarkovNode* get_next_random_node(MarkovNode *state_struct_ptr);

/**
 * Receive markov_chain, generate and print random sentence out of it. The
 * sentence most have at least 2 words in it.
 * @param markov_chain
 * @param first_node markov_node to start with, if NULL- choose a
 * random markov_node
 * @param  max_length maximum length of chain to generate
 */
void generate_tweet(MarkovChain *markov_chain, MarkovNode *
first_node, int max_length);

/**
 * Free markov_chain and all of it's content from memory
 * @param markov_chain markov_chain to free
 */
void free_database(MarkovChain **markov_chain);

/**
 * Add the second markov_node to the counter list of the first markov_node.
 * If already in list, update it's counter value.
 * @param first_node
 * @param second_node
 * @param markov_chain
 * @return success/failure: true if the process was successful, false if in
 * case of allocation error.
 */
bool add_node_to_frequencies_list(MarkovNode *first_node, MarkovNode
*second_node, MarkovChain *markov_chain);

/**
* Check if data_ptr is in database. If so, return the
 * markov_node wrapping it in
 * the markov_chain, otherwise return NULL.
 * @param markov_chain the chain to look in its database
 * @param data_ptr the state to look for
 * @return Pointer to the Node wrapping given state, NULL if state not in
 * database.
 */
Node* get_node_from_database(MarkovChain *markov_chain, void *data_ptr);

/**
* If data_ptr in markov_chain, return it's node. Otherwise, create new
 * node, add to end of markov_chain's database and return it.
 * @param markov_chain the chain to look in its database
 * @param data_ptr the state to look for
 * @return node wrapping given data_ptr in given chain's database
 */
Node* add_to_database(MarkovChain *markov_chain, void *data_ptr);

#endif /* MARKOV_CHAIN_H */
