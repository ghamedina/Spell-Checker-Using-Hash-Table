/*
* CS 261 Data Structures
* Assignment 6
* Name: Derrick Li
* Date: Jun. 4, 2017
* Description: Extra credit: Spell Checker program - spellChecker.c file
*/

#include "hashMap.h"
#include <assert.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
* Calculates the Levenshtein Distance between two input strings. Copied from source.
* Source Citation: https://en.wikibooks.org/wiki/Algorithm_Implementation/Strings/Levenshtein_distance#C
* (Macro MIN3 also copied from source.)
* @param s1 - string
* @param s2 - string
* @return integer representing levenshtein distance between string s1 and string s2.
*/
#define MIN3(a, b, c) ((a) < (b) ? ((a) < (c) ? (a) : (c)) : ((b) < (c) ? (b) : (c)))
int levenshtein(char *s1, char *s2) {
	unsigned int s1len, s2len, x, y, lastdiag, olddiag;
	s1len = strlen(s1);
	s2len = strlen(s2);
	unsigned int column[s1len + 1];
	for (y = 1; y <= s1len; y++)
		column[y] = y;
	for (x = 1; x <= s2len; x++) {
		column[0] = x;
		for (y = 1, lastdiag = x - 1; y <= s1len; y++) {
			olddiag = column[y];
			column[y] = MIN3(column[y] + 1, column[y - 1] + 1, lastdiag + (s1[y - 1] == s2[x - 1] ? 0 : 1));
			lastdiag = olddiag;
		}
	}
	return(column[s1len]);
}

/**
 * Allocates a string for the next word in the file and returns it. This string
 * is null terminated. Returns NULL after reaching the end of the file.
 * @param file
 * @return Allocated string or NULL.
 */
char* nextWord(FILE* file)
{
    int maxLength = 16;
    int length = 0;
    char* word = malloc(sizeof(char) * maxLength);
    while (1)
    {
        char c = fgetc(file);
        if ((c >= '0' && c <= '9') ||
            (c >= 'A' && c <= 'Z') ||
            (c >= 'a' && c <= 'z') ||
            c == '\'')
        {
            if (length + 1 >= maxLength)
            {
                maxLength *= 2;
                word = realloc(word, maxLength);
            }
            word[length] = c;
            length++;
        }
        else if (length > 0 || c == EOF)
        {
            break;
        }
    }
    if (length == 0)
    {
        free(word);
        return NULL;
    }
    word[length] = '\0';
    return word;
}

/**
 * Loads the contents of the file into the hash map.
 * @param file
 * @param map
 */
void loadDictionary(FILE* file, HashMap* map)
{
    // FIXME: implement
	char* word = nextWord(file);

	while (word != NULL)
	{
		hashMapPut(map, word, 0);

		free(word);
		word = nextWord(file);
	}
}

/**
 * Prints the concordance of the given file and performance information. Uses
 * the file input1.txt by default or a file name specified as a command line
 * argument.
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, const char** argv)
{
    // FIXME: implement
    HashMap* map = hashMapNew(1000);
    
    FILE* file = fopen("dictionary.txt", "r");
    clock_t timer = clock();
    loadDictionary(file, map);
    timer = clock() - timer;
    printf("Dictionary loaded in %f seconds\n", (float)timer / (float)CLOCKS_PER_SEC);
    fclose(file);
    
    char inputBuffer[256];
    int quit = 0;
	while (!quit)
	{
		printf("Enter a word or \"quit\" to quit: ");
		scanf("%s", inputBuffer);

		// Implement the spell checker code here..
		while (getchar() != '\n');   //get rid of newline characters

		if (strcmp(inputBuffer, "quit") == 0)
		{
			printf("\nGoodbye.\n\n");
		}
		else if (hashMapContainsKey(map, inputBuffer))
		{
			printf("\nThe word \"%s\" is spelled correctly.\n\n", inputBuffer);
		}
		else
		{
			/* Initialize suggestion array */
			const int NUM_OF_SUGGESTIONS = 6;
			HashLink **suggestion = malloc(NUM_OF_SUGGESTIONS * sizeof(HashLink *));

			for (int i = 0; i < NUM_OF_SUGGESTIONS; i++)
			{
				suggestion[i] = NULL;
			}

			/* Goes through the entire dictionary map, one word at a time */
			for (int i = 0; i < map->capacity; i++)
			{
				HashLink *tempLink = map->table[i];

				while (tempLink != NULL)
				{
					/* Calculates levenshtein distance between the mispelled word and the dictionary word */
					int levenDist = levenshtein(inputBuffer, tempLink->key);

					/* Go through each element of suggestion array */
					for (int i = 0; i < NUM_OF_SUGGESTIONS; i++)
					{
						/* If suggestion array has empty elements then create links and store in array */
						if (suggestion[i] == NULL)
						{
							suggestion[i] = malloc(sizeof(HashLink));

							suggestion[i]->key = malloc(sizeof(char) * 256);   //max size of word 255 char + '/0'
							strcpy(suggestion[i]->key, tempLink->key);
							suggestion[i]->value = levenDist;
							suggestion[i]->next = NULL;

							break;
						}
						/* Else, go through each element and see if levenDist is less than the current stored levenDist.
							Store new word and levenDist if it is less than current levenDist. */
						else if (levenDist < suggestion[i]->value)
						{
							strcpy(suggestion[i]->key, tempLink->key);
							suggestion[i]->value = levenDist;

							break;
						}
					}

					tempLink = tempLink->next;
				}

			}

			/* Prints the suggestions */
			printf("\nThe word \"%s\" could not be found in the dictionary.\n", inputBuffer);
			printf("Perhaps you meant one of the following words:\n");

			for (int i = 0; i < NUM_OF_SUGGESTIONS; i++)
			{
				printf("%s\n", suggestion[i]->key);
			}
			printf("\n");

			/* For each link, frees the key in it and the link itself. Then frees the suggestion array. */
			for (int i = 0; i < NUM_OF_SUGGESTIONS; i++)
			{
				free(suggestion[i]->key);
				suggestion[i]->key = NULL;

				free(suggestion[i]);
				suggestion[i] = NULL;
			}

			free(suggestion);
			suggestion = NULL;
		}

		// End spell checker code
        
        if (strcmp(inputBuffer, "quit") == 0)
        {
            quit = 1;
        }
    }
    
    hashMapDelete(map);
    return 0;
}