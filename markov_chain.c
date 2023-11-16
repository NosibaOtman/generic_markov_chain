#include "markov_chain.h"
#include "string.h"

/**
* Get random number between 0 and max_number [0, max_number).
* @param max_number maximal number to return (not including)
* @return Random number
*/
int get_random_number (int max_number)
{
  return rand () % max_number;
}


/**
 * this function return false if
 * the element we checks is not NULL
 * else return false
 * and print a messege ERROR
 * @param to_cheack what we want to check
 * @return false if it is NULL
 * else return true
 */
bool check_allocate(void* to_cheack)
{
  if(to_cheack ==NULL)
    {
      fprintf (stdout,ALLOCATION_ERROR_MASSAGE);
      return false;
    }
  return true;
}



/**
* If data_ptr in markov_chain, return it's node. Otherwise, create new
 * node, add to end of markov_chain's database and return it.
 * @param markov_chain the chain to look in its database
 * @param data_ptr the state to look for
 * @return markov_node wrapping given data_ptr in given chain's database,
 * returns NULL in case of memory allocation failure.
 */
Node* add_to_database(MarkovChain *markov_chain, void *data_ptr)
{
  Node* to_return_from_data_base =
      get_node_from_database(markov_chain,data_ptr);
  if (to_return_from_data_base != NULL)
    {return to_return_from_data_base;}
  else{
      MarkovNode *new_markov_node = malloc (sizeof (MarkovNode));
      if(check_allocate (new_markov_node)==false){
          return NULL;
        }
      void *word = markov_chain->copy_func(data_ptr);
      *new_markov_node = (MarkovNode)
          {word, NULL, 0};
      int add_result = add
          (markov_chain->database, new_markov_node);
      if (add_result == 1)
        {
          fprintf (stdout,ALLOCATION_ERROR_MASSAGE);
          markov_chain->free_data(word);
          word=NULL;
          free (new_markov_node);
          new_markov_node = NULL;
          return NULL;
        }
    }
  return markov_chain->database->last;
}



/**
* Check if data_ptr is in database. If so,
 * return the markov_node wrapping it in
 * the markov_chain, otherwise return NULL.
 * @param markov_chain the chain to look in its database
 * @param data_ptr the state to look for
 * @return Pointer to the Node wrapping
 * given state, NULL if state not in
 * database.
 */
Node* get_node_from_database
    (MarkovChain *markov_chain, void *data_ptr)
{
  // Search for the data_ptr in the markov_chain's database
  Node *current_node = markov_chain->database->first;
  while (current_node != NULL) {
      MarkovNode *markov_node = (MarkovNode*) current_node->data;
//      if (strcmp(markov_node->data, data_ptr) == 0) {
//          return current_node;
//        }
      if (markov_chain->comp_func(markov_node->data, data_ptr) == 0) {
          return current_node;
        }
      current_node = current_node->next;
    }
  // If the data_ptr is not found, return NULL
  return NULL;
}


/**
 * Add the second markov_node to the frequency
 * list of the first markov_node.
 * If already in list, update it's occurrence frequency value.
 * @param first_node
 * @param second_node
 * @return success/failure: true if the process was successful, false if in
 * case of allocation error.
 */
bool add_node_to_frequencies_list
    (MarkovNode *first_node, MarkovNode *second_node,MarkovChain* markov_chain)
{if (first_node == NULL || second_node == NULL) {return false;}
  if (first_node->frequencies_list == NULL) {MarkovNodeFrequency*
  frequencies_list=(MarkovNodeFrequency *) malloc(sizeof(MarkovNodeFrequency));
      if(!frequencies_list){fprintf (stdout,ALLOCATION_ERROR_MASSAGE);
          return false;} //#TODO add if stop program
      frequencies_list->frequency = 1;
      frequencies_list->markov_node = second_node;
      first_node->frequencies_list = frequencies_list;
      first_node->length_of_frequencies_list += 1;
      return true;}
  int i = 0;
  while (i <= first_node->length_of_frequencies_list-1)
//    {int strcmp_res = strcmp
//      (first_node->frequencies_list[i].markov_node->data,second_node->data);
//      if (strcmp_res == 0){first_node->frequencies_list[i].frequency++;
//          return true;}
    {int strcmp_res = markov_chain->comp_func
      (first_node->frequencies_list[i].markov_node->data,second_node->data);
      if (strcmp_res == 0){first_node->frequencies_list[i].frequency++;
          return true;}
      i++;
      if (i == first_node->length_of_frequencies_list){
          MarkovNodeFrequency *temp = realloc((first_node->frequencies_list),
        sizeof (MarkovNode) * (first_node->length_of_frequencies_list + 1));
          if(!temp){return false;}
          first_node->frequencies_list = temp;
          first_node->frequencies_list[i].frequency = 1;
          first_node->frequencies_list[i].markov_node = second_node;
          first_node->length_of_frequencies_list++;
          return true;
        }}
  return false;}




/**
 * Free markov_chain and all of it's content from memory
 * @param markov_chain markov_chain to free
 */
void free_database(MarkovChain ** ptr_chain)
{
  if (ptr_chain == NULL || *ptr_chain == NULL) {
      return; // nothing to free
    }
  LinkedList * database = (*ptr_chain)->database;
  while (database->first != NULL )
    {
      free (database->first->data->frequencies_list);
      database->first->data->frequencies_list = NULL;
      (*ptr_chain)->free_data(database->first->data->data);
      database->first->data->data = NULL;
      free (database->first->data);
      database->first->data = NULL;
      Node *next = database->first->next;
      free(database->first);
      database->first = NULL;
      database->first = next;
    }
  free(database);
  database = NULL;
  free (*ptr_chain);
  *ptr_chain = NULL;
}





/**
 * Receive markov_chain, generate and print random sentence out of it. The
 * sentence most have at least 2 words in it.
 * @param markov_chain
 * @param first_node markov_node to start with,
 *                   if NULL- choose a random markov_node
 * @param  max_length maximum length of chain to generate
 */
void generate_tweet
    (MarkovChain *markov_chain, MarkovNode *first_node, int max_length)
{
  if (first_node == NULL){
      first_node = get_first_random_node (markov_chain);
    }
  markov_chain->print_func(first_node->data);
  bool res = markov_chain->is_last (first_node->data);
  int i = 0;
  while (res == false && i < max_length)
    {
      printf (" ");
      MarkovNode *next_node = get_next_random_node (first_node);
      if(next_node == NULL)
        {break;}
      markov_chain->print_func(next_node->data);
      first_node = next_node;
      res = markov_chain->is_last (first_node->data);
      i++;
    }printf ("\n");
}


/**
 * this function checks if the last
 * char in data is dot
 * @param data string to check
 * @return true if the last char
 * in data is string , false other wise
 */
bool last_char_dot(char *data)
{
  int length = (int)strlen(data);
  if(data[length-1] == '.')
    {
      return true;
    }
  return false;
}

/**
 * Get one random state from the given markov_chain's database.
 * @param markov_chain
 * @return
 */
MarkovNode* get_first_random_node(MarkovChain *markov_chain){
  int size = markov_chain->database->size;
  int index = get_random_number (size);
  Node *node = markov_chain->database->first;
  int counter = 0;
  while (node != NULL)
    {
      if (counter == index)
        {
          MarkovNode *markov_node_to_return = node->data;
          bool to_check_word;
          to_check_word = markov_chain->is_last (markov_node_to_return->data);
          if (!to_check_word)
            {
              return markov_node_to_return;
            }
          else
            {
              index = get_random_number (size);
              node = markov_chain->database->first;
              counter = 0;
              continue;
            }
        }
      counter++;
      node = node->next;
    }
  return NULL;
}

/**
 * Choose randomly the next state,
 * depend on it's occurrence frequency.
 * @param state_struct_ptr MarkovNode to choose from
 * @return MarkovNode of the chosen state
 */
MarkovNode* get_next_random_node(MarkovNode *state_struct_ptr)
{
  int total_frequency = 0;
  int len_list = state_struct_ptr->length_of_frequencies_list;
  for (int i = 0; i < len_list; i++) //(0 to len_list-1)
    {
      total_frequency += state_struct_ptr->frequencies_list[i].frequency;
    }
  if (state_struct_ptr == NULL
      || len_list < 0 || len_list > total_frequency) {
      return NULL;
    }
  const MarkovNodeFrequency *current = state_struct_ptr->frequencies_list;
  int i = get_random_number (total_frequency);
  while (i >= current->frequency)
    {
      i -= current->frequency;
      current++;
    }
  return current->markov_node;
}
