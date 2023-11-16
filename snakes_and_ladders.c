#include <string.h> // For strlen(), strcmp(), strcpy()
#include "markov_chain.h"

#define MAX(X, Y) (((X) < (Y)) ? (Y) : (X))

#define EMPTY (-1)
#define BOARD_SIZE 100

#define DICE_MAX 6
#define NUM_OF_TRANSITIONS 20
#define C1 1
#define C2 2
#define C3 3
#define C60 60
#define BASE 10
#define MESSAGE3 "Usage: please check the argument number\n"
/**
 * represents the transitions by ladders and snakes in the game
 * each tuple (x,y) represents a ladder from x to if x<y or a snake otherwise
 */
const int transitions[][2] = {{13, 4},
                              {85, 17},
                              {95, 67},
                              {97, 58},
                              {66, 89},
                              {87, 31},
                              {57, 83},
                              {91, 25},
                              {28, 50},
                              {35, 11},
                              {8,  30},
                              {41, 62},
                              {81, 43},
                              {69, 32},
                              {20, 39},
                              {33, 70},
                              {79, 99},
                              {23, 76},
                              {15, 47},
                              {61, 14}};

/**
 * struct represents a Cell in the game board
 */
typedef struct Cell {
    int number;
    int ladder_to;
    int snake_to;
} Cell;

/** Error handler **/
static int handle_error(char *error_msg, MarkovChain **database)
{
    printf("%s", error_msg);
    if (database != NULL)
    {
        free_database(database);
    }
    return EXIT_FAILURE;
}



static bool is_last_snake(void *pointer);

/**
 * this is the print function of the snake and ladders
 * this function print the step of the game
 * in every random walk
 * @param pointer to change to cell and print the
 * number of the specific cell
 */
static void print_func_snake(void *pointer)
{
  Cell *cell = (Cell *)pointer;
  if(cell->number == 1){
      printf("[%d] ->",cell->number);
      return;
  }
  else if(is_last_snake(cell)){
    printf("[%d]",cell->number);
  }
  else if(cell->number != BOARD_SIZE && cell->snake_to ==
  EMPTY && cell->ladder_to == EMPTY){
     printf("[%d] ->",cell->number);
  }
  else if(cell->snake_to != EMPTY){
    printf("[%d]-snake to %d ->",cell->number,cell->snake_to);
  }
  else if(cell->ladder_to != EMPTY){
    printf("[%d]-ladder to %d ->",cell->number,cell->ladder_to);
  }
}


/**
 * this is the copy function of the snake and ladders
 * it is copy the pointer_cell to new allocated cell
 * for it
 * @param pointer this is a cell to copy
 * @return
 */
static void* copy_func_snake(void *pointer)
{
    Cell *original = (Cell *)pointer;
    Cell *copy = malloc(sizeof(Cell));
    if (copy != NULL) {
        copy->number = original->number;
        copy->ladder_to = original->ladder_to;
        copy->snake_to = original->snake_to;

    } else {
        fprintf(stderr,ALLOCATION_ERROR_MASSAGE);
        exit(EXIT_FAILURE);
    }
    return copy;
}

/**
 *  * this is the free function of the snake and ladders
 *  it is free the allocated memory
 * @param pointer this is the cell to free
 */
static void free_data_snake(void *pointer)
{
    Cell *str = (Cell *)pointer;
    free(str);
    str = NULL;
}

/**
 *  * this is the compare function of the snake and ladders
 * @param pointer1 cell1 to be compare
 * @param pointer2 cell2 to be compare
 * @return 1,if(cell1 is bigger,
 * -1(if cell2 is bigger) , 0 otherwise
 */

static int comp_func_snake(void*pointer1,void* pointer2)
{
    Cell *cell1 = (Cell *)pointer1;
    Cell *cell2 = (Cell *)pointer2;

    if (cell1->number > cell2->number) {
        return 1;  // cell1 is bigger
    } else if (cell1->number < cell2->number) {
        return -1; // cell2 is bigger
    } else if (cell1->number == cell2->number){
        return 0;  // equal
    }
  return 0;
}


/**
 * this is the function that check if we reach to
 * the last step in the game
 * @param pointer cell to check if we are in the last
 * cell
 * @return true if it id the last state,false otherwise
 */
static bool is_last_snake(void *pointer)
 {
    Cell *cell = (Cell *)pointer;
    if (cell->number == BOARD_SIZE) {
        return true;  // It is the last state
    }
   return false;
}

/**
 * this function create the board
 * @param cells
 * @return
 */
static int create_board(Cell *cells[BOARD_SIZE])
{
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        cells[i] = malloc(sizeof(Cell));
        if (cells[i] == NULL)
        {
            for (int j = 0; j < i; j++) {
                free(cells[j]);
            }
            handle_error(ALLOCATION_ERROR_MASSAGE,NULL);
            return EXIT_FAILURE;
        }
        *(cells[i]) = (Cell) {i + 1, EMPTY, EMPTY};
    }

    for (int i = 0; i < NUM_OF_TRANSITIONS; i++)
    {
        int from = transitions[i][0];
        int to = transitions[i][1];
        if (from < to)
        {
            cells[from - 1]->ladder_to = to;
        }
        else
        {
            cells[from - 1]->snake_to = to;
        }
    }
    return EXIT_SUCCESS;
}

/**
 * fills database
 * @param markov_chain
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */
static int fill_database(MarkovChain *markov_chain)
{
    Cell* cells[BOARD_SIZE];
    if(create_board(cells) == EXIT_FAILURE)
    {
        return EXIT_FAILURE;
    }
    MarkovNode *from_node = NULL, *to_node = NULL;
    size_t index_to;
    for (size_t i = 0; i < BOARD_SIZE; i++)
    {
        add_to_database(markov_chain, cells[i]);
    }

    for (size_t i = 0; i < BOARD_SIZE; i++)
    {
        from_node = get_node_from_database(markov_chain,cells[i])->data;

        if (cells[i]->snake_to != EMPTY || cells[i]->ladder_to != EMPTY)
        {
            index_to = MAX(cells[i]->snake_to,cells[i]->ladder_to) - 1;
            to_node = get_node_from_database(markov_chain, cells[index_to])
                    ->data;
            add_node_to_frequencies_list (from_node, to_node, markov_chain);
        }
        else
        {
            for (int j = 1; j <= DICE_MAX; j++)
            {
                index_to = ((Cell*) (from_node->data))->number + j - 1;
                if (index_to >= BOARD_SIZE)
                {
                    break;
                }
                to_node = get_node_from_database(markov_chain, cells[index_to])
                        ->data;
                add_node_to_frequencies_list (from_node, to_node,
                                              markov_chain);
            }
        }
    }
    // free temp arr
    for (size_t i = 0; i < BOARD_SIZE; i++)
    {
        free(cells[i]);
    }
    return EXIT_SUCCESS;
}




/**
 * this function check the validate of argc number
 * @param argc
 * @return EXIT_SUCCESS if the number of agrc is valid
 * ,EXIT_FAILURE otherwise
 */
int check_valid_argc_snake(int argc){
  if(argc != C3){
      fprintf (stdout,MESSAGE3);
      return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}


/**
 * this function fill the markovchain and generate sequence
 * for the snake
 * @param argv the argv that the main get
 * @param markov_chain an allocated memory of markovchain
 * to fill it
 */
void fill_and_generate_sequence_snake
(char *const *argv, MarkovChain *markov_chain)
{
  LinkedList *linked_list = malloc (sizeof (LinkedList));
  if(!linked_list){
      fprintf (stdout,ALLOCATION_ERROR_MASSAGE);
      return ;}
  markov_chain->database = linked_list;
  markov_chain->database->first=NULL;
  markov_chain->database->last=NULL;
  markov_chain->database->size=0;
  markov_chain->print_func = print_func_snake;
  markov_chain->comp_func = comp_func_snake;
  markov_chain->copy_func = copy_func_snake;
  markov_chain->free_data = free_data_snake;
  markov_chain->is_last = is_last_snake;

  fill_database (markov_chain);
  int round_num = strtol (argv[C2], NULL, BASE);
  for (int i=0;i<round_num ;i++)
    {
      fprintf (stdout,"Random Walk %d: ",i+1);
      generate_tweet (markov_chain,
                      markov_chain->database->first->data,C60-C1); // to do
    }
}



/**
 * @param argc num of arguments
 * @param argv 1) Seed
 *             2) Number of sentences to generate
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */
int main(int argc, char *argv[])
{
  srand (strtol (argv[1],NULL,BASE));
  if(check_valid_argc_snake (argc) == EXIT_FAILURE){
    return EXIT_FAILURE;
  }
  MarkovChain *markov_chain = malloc (sizeof (MarkovChain));
  if(markov_chain==NULL)
    {
      fprintf (stdout,ALLOCATION_ERROR_MASSAGE);
    }
  fill_and_generate_sequence_snake(argv, markov_chain);
  free_database (&markov_chain);
  return 0;
}
