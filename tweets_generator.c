#include <string.h>
#include "markov_chain.h"
#define C5 5
#define C4 4
#define C2 2
#define C3 3
#define C20 20
#define C10 10
#define C1000 1000
#define BASE 10
#define DELIMITERS " \n\r"

#define MESSAGE2 "Error: file can't be opened ,please check the file path\n "
#define MESSAGE3 "Usage: please check the the number of the arguments \n "


int fill_database (FILE *fp, int words_to_read, MarkovChain *markov_chain);
void fill_and_generate_sequence
    (char *const *argv, FILE *file_to_read,int num_word_to_read,
     MarkovChain *markov_chain);



/**
 * this is the print function of tweet
 * it is print the pointer to stdout
 * @param pointer  char to print to stdout
 */
static void print_func_tweet(void *pointer)
{
  char *c_pointer = (char *)pointer;
  fprintf (stdout, "%s", c_pointer);

}

/**
 * this is the copy function for the tweet
 * it is allocate a new memory for char*
 * copy the content of pointer to the newly
 * allocated memory
 * @param pointer a chr* to copy
 * @return EXIT_FAILURE if it is fail,or
 * the new copy if it succeed
 */
static void* copy_func_tweet(void *pointer)
{
  char *original = (char *)pointer;
  size_t length = strlen(original);
  char *copy = malloc((length + 1) * sizeof(char));
  if (copy != NULL) {
      strcpy(copy, original);
    } else {
      printf(ALLOCATION_ERROR_MASSAGE);
      exit(EXIT_FAILURE);
    }
  return copy;
}

/**
 * this is the free data for the tweet
 * it is free the pointer
 * @param pointer to free
 */
static void free_data_tweet(void *pointer)
{
  char *str = (char *)pointer;
  free(str);
  str = NULL;
}

/**
 * this is the compare function for the tweet
 * compare between two char*
 * @param pointer1 char * to compare
 * @param pointer2 char * to compare
 * @return strcmp result
 */
static int comp_func_tweet(void*pointer1,void* pointer2)
{
  char *char1 = (char *)pointer1;
  char *char2 = (char *)pointer2;
  return strcmp (char1,char2);
}
static bool is_last_tweet(void *pointer){
  char *data = (char *)pointer;
  int length = (int)strlen(data);
  if(data[length-1] == '.')
    {
      return true;
    }
  return false;
}



/**
 * this function get positive number and return
 * EXIT_SUCCSES if the program get
 * 4 or 5 number of argc ,else return EXIT_FAILURE
 * @param argc the arguments number
 * @return EXIT_FAILURE or EXIT SUCCSES
 */
int check_valid_argc(int argc){
  if(argc != C4 && argc!= C5){
      fprintf (stdout,MESSAGE3);
      return EXIT_FAILURE;
    }
  return EXIT_SUCCESS;
}

/**
 * this function fill the data base
 * in the main fub-nction
 * and then it generate_random_sequence
 * and print the random_sequence
 * @param argv
 * @param file_to_read
 * @param num_word_to_read
 * @param markov_chain
 */
void fill_and_generate_sequence (char *const *argv, FILE *file_to_read,
  int num_word_to_read, MarkovChain *markov_chain)
{
  LinkedList *linked_list = malloc (sizeof (LinkedList));
  if(!linked_list){
      fprintf (stdout,ALLOCATION_ERROR_MASSAGE);
      return ;}
  markov_chain->database = linked_list;
  markov_chain->database->first=NULL;
  markov_chain->database->last=NULL;
  markov_chain->database->size=0;
  markov_chain->print_func = print_func_tweet;
  markov_chain->comp_func = comp_func_tweet;
  markov_chain->copy_func = copy_func_tweet;
  markov_chain->free_data = free_data_tweet;
  markov_chain->is_last = is_last_tweet;

  fill_database (file_to_read,num_word_to_read,markov_chain);
  int tweet_num = strtol (argv[C2], NULL, BASE);
  for (int i=0;i<tweet_num;i++)
    {
      fprintf (stdout,"Tweet %d: ",i+1);
      generate_tweet (markov_chain,
                      get_first_random_node
                          (markov_chain),C20);
    }
}


/**
 *this is used in the main function when the program get argc ==4
 * @param argv
 * @param file_to_read file to read words from it
 */
void argc_is_4( char *argv[], FILE*file_to_read)
{
  int num_word_to_read = -1;
  MarkovChain *markov_chain = malloc(sizeof (MarkovChain));
  if(markov_chain==NULL)
    {
      fprintf (stdout,ALLOCATION_ERROR_MASSAGE);
    }
//  fill_markov_chain_tweet(markov_chain);
  fill_and_generate_sequence
      (argv, file_to_read, num_word_to_read, markov_chain);

  fclose (file_to_read);
  free_database (&markov_chain);
}



/**
 *this is used in the main function when the program get argc ==5
 * @param argv
 * @param file_to_read file to read words from it
 */
void args_is_5( char *argv[], FILE*file_to_read)
{
  int num_word_to_read = strtol (argv[C4], NULL, BASE);
  MarkovChain *markov_chain = malloc (sizeof (MarkovChain));
  if(markov_chain==NULL)
    {
      fprintf (stdout,ALLOCATION_ERROR_MASSAGE);    }
  fill_and_generate_sequence
      (argv, file_to_read, num_word_to_read, markov_chain);
  fclose (file_to_read);
  free_database (&markov_chain);
}

bool last_char_dot_2(char *data)
{
  int length = (int)strlen(data);
  if(data[length-1] == '.')
    {
      return true;
    }
  return false;
}


/**
 *this function read number
 * words based on the words
 *
 * to read from file and fill
 * the database in this words
 * it fill the markov chain in
 * the markov node
 * @param fp file to read from it
 * @param words_to_read num of words to read from the file
 * @param markov_chain it is the database of linked list that should fill
 */
int fill_database (FILE *fp,int  words_to_read,MarkovChain *markov_chain)
{char line[C1000];
  int counter = 1 ;
  while(fgets(line,C1000,fp) != NULL
        && (counter <= words_to_read || words_to_read == -1))
    {char *token = strtok (line, DELIMITERS);
      if(!token){continue;}
      counter +=1;
      Node *first = add_to_database (markov_chain, token);
      if(!first)
        {
          return EXIT_FAILURE;}
      Node *second = NULL;
      while (token != NULL &&
             (counter <= words_to_read || words_to_read == -1))
        {token = strtok (NULL, DELIMITERS);
          if(!token){break;}
          second = add_to_database(markov_chain, token);
          if(!second){
              return EXIT_FAILURE;}
          counter +=1;
          if(!last_char_dot_2 (first->data->data))
            {add_node_to_frequencies_list
              (first->data, second->data,markov_chain);}
          first = second;
        }
    }
  return 0;
}



/**
 * THIS IS THE MAIN FUNCTION
 */
int main (int argc, char *argv[]){
  srand (strtol (argv[1],NULL,C10));
  if(check_valid_argc (argc) == EXIT_FAILURE){return EXIT_FAILURE;}
  FILE *file_to_read = fopen (argv[C3], "r");
  if (file_to_read == NULL)
    {
      fprintf (stdout, MESSAGE2);
      return EXIT_FAILURE;
    }
  if (argc == C5)
    {
      args_is_5 ( argv, file_to_read);
    }
  else
    {
      argc_is_4 ( argv, file_to_read);
    }
    return 0;
}
