#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Helpers/ErrorHandler/ErrorHandler.h"
#include "DatabaseReader/DatabaseReader.h"
#include "SubSetGenerator/SubSetGenerator.h"
#include "InputReader/InputReader.h"
#include "Searcher/Searcher.h"
#include "Helpers/Printer/Printer.h"

int main(int argc, char **argv) {
    // Argumenten inlezen
    Arguments *args = parse_arguments(argc, argv);
    // Creatie databank
    OrderedDatabase *ordered_database = read_database(args->database);
    // Klaar voor input van de gebruiker
    printf("?");
    // Lees lijn van gebruiker in
    char input_line[MAX_INPUT_LENGTH];
    while (read_input(input_line) != NULL) {
        remove_trailing_newline(input_line);
        // De 'input-zin' splitsen in woorden
        int wordcounter = 0;
        char *splitted[MAX_INPUT_LENGTH]; // Lijst van alle woorden, gesplitst op spatie -> bvb. ["De", "Sterre", "Gent"]
        split_input(splitted, input_line, &wordcounter);

        // Genereer boomstructuur voor alle mogelijke subsetten voor het zoekprobleem
        SubSets subsets = {NULL, 0};
        Node root = {0, NULL, malloc(sizeof(Node *) * wordcounter), 0};
        generate_subsets(&subsets, wordcounter, &root);
        // Bitvectoren die bijhouden welke substrings er al gezocht zijn
        // Bij input: "De Sterre Gent", kan er bijvoorbeeld gesplitst worden op ("De", "Sterre", "Gent") en ("De", "Sterre Gent")
        // Als we dan beide gesplitste versies van de input opzoeken, zullen we "De" 2x opzoeken
        // Door in de bitvectoren bij te houden welke (combinaties van) woorden we al hebben opgezocht, kunnen we veel
        // opzoekwerk besparen, en dus sneller zoeken (bij grote inputs).
        unsigned long long int processed_substrings[wordcounter]; // Dit limiteert het aantal woorden op 64
        memset(&processed_substrings, 0, wordcounter * sizeof(unsigned long long int));
        assert_with_message(wordcounter <= sizeof(unsigned long long int) * 8, TOO_MANY_WORDS_ERROR);

        // Structuur die alle gevonden matches in de databank zal bijhouden, bijgehouden per verschillende substring
        int substrings_count = get_possible_substrings_count(wordcounter);
        int current_substring_count = 0;
        SubstringPlacement *substring_placements[substrings_count];
        SearchResults *all_searchresults[substrings_count];

        // Om de zoekresultaten te matchen met ALLE subsets
        // aantal paden * max aantal nodes op één pad
        int sorted_searchresults[get_possibilities(wordcounter) * wordcounter];
        int sorted_index = 0;

        // Structuur die beste volledige matches bijhoudt met hun score
        BestMatches *best_matches = init_best_matches();

        for (int i = 0; i < subsets.possibilities; i++) { // Voor alle mogelijke subsets van de inputstring
            NodeList *path = subsets.paths[i];

            // Genereer de effectieve string
            char *list[path->size];
            StringList stringlist = {list, 0};
            get_stringlist_from_path(path, &stringlist, splitted);

            // Lijst van zoekresultaten, voor de huidige volledige zoekterm (bvb. lijst van resultaten voor "De Sterre" en "Gent")
            SearchResults *full_string_searchresults[stringlist.size];
            // Voor iedere substring -> bvb. bij subsets ("De Sterre",  "Gent") -> substring "De Sterre" en substring "Gent"
            for (int j = 0; j < stringlist.size; j++) {
                // Check of deze substring al geprocessed is
                unsigned int current_substring_length = path->nodes[j]->value; // Aantal woorden in huidige substring
                unsigned int index_first_word_of_substring = 0;
                for (int k = 0; k < j; k++) {
                    index_first_word_of_substring += path->nodes[k]->value;
                }
                if ((processed_substrings[current_substring_length - 1] & (1u << index_first_word_of_substring)) == 0) {
                    // Deze substring is nog niet gezocht in de databank
                    // Aanduiden zodat deze volgende keer niet meer gezocht wordt
                    processed_substrings[current_substring_length - 1] |= (1u << index_first_word_of_substring);
                    // Volgorde bijhouden
                    save_substring_order(substring_placements, current_substring_count, current_substring_length,
                                         index_first_word_of_substring);

                    // Zoeken in databank
                    SearchResults *current_searchresults = do_search(ordered_database, stringlist.list[j],
                                                                     all_searchresults,
                                                                     current_substring_count);
                    // Opslaan van zoekresultaat
                    all_searchresults[current_substring_count] = current_searchresults;
                    current_substring_count++;
                }
                // Aangezien we niet strict iedere substring steeds opzoeken in de databank, moeten we nu de juiste zoekresultaten uit de lijst van opgeslagen resultaten halen
                int search_index = get_correct_substring_placement(substring_placements, current_substring_length,
                                                                   index_first_word_of_substring);
                sorted_searchresults[sorted_index] = search_index;
                full_string_searchresults[j] = all_searchresults[sorted_searchresults[sorted_index]];
                sorted_index++;
            }
            get_best_matches(full_string_searchresults, &stringlist, best_matches, args);
            free_list_of_strings(stringlist.list, stringlist.size);
        }

        sort_best_matches(best_matches);
        print_best_matches(best_matches);

        // Geheugen vrijmaken
        free_best_matches(best_matches);
        free_substring_placements(substring_placements, substrings_count);
        free_searchresults(all_searchresults, substrings_count);
        free_list_of_strings(splitted, wordcounter);
        free_node_children_recursive(&root);
        free_nodelist_list(subsets.paths, get_possibilities(wordcounter));
        printf("?");
    }

    free_ordered_database(ordered_database);
    free_arguments(args);
    return 0;
}
