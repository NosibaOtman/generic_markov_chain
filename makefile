tweets: linked_list.o markov_chain.o tweets_generator.o
	gcc linked_list.o markov_chain.o tweets_generator.o -o tweets_generator

snake: linked_list.o markov_chain.o snakes_and_ladders.o
	gcc  linked_list.o markov_chain.o snakes_and_ladders.o -o snake

linked_list.o: linked_list.h linked_list.c
	gcc -c linked_list.c

markov_chain.o: markov_chain.c markov_chain.h
	gcc -c markov_chain.c

tweets_generator.o: tweets_generator.c linked_list.h markov_chain.h
	gcc -c tweets_generator.c

snakes_and_ladders.o: snakes_and_ladders.c linked_list.h markov_chain.h
	gcc -c snakes_and_ladders.c


