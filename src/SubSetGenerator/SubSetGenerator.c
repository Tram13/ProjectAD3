#include "SubSetGenerator.h"
#include <string.h>
#include <stdlib.h>
#include "../InputReader/InputReader.h"
#include "../Helpers/ErrorHandler/ErrorHandler.h"
#include "../Searcher/Searcher.h"
#include <math.h>

void generate_subsets(SubSets *destination, int wordcounter, Node *root) {
    /* We stellen een boomstructuur op. De bedoeling is dat het pad van ieder blad tot de wortel de verdeling van de woorden opgeeft,
     * waarbij de waarde van iedere top aanduidt hoeveel woorden er worden samengenomen.
     *
     * Stel als input: De Sterre Gent
     *                  ( 0 )
     *                /   |   \
     *            ( 1 ) ( 2 ) ( 3 )
     *           /   \     \
     *        ( 1 ) ( 2 ) ( 1 )
     *        /
     *     ( 1 )
     *
     * Dit geeft ons dan van links naar rechts: De Sterre Gent, DeSterre Gent, De SterreGent, DeSterreGent
     *
     */
    assert_with_message(root->children != NULL, SUBSET_MALLOC_FAILED);
    split(wordcounter, root, wordcounter);

    // Alle bladen verzamelen
    NodeList leaves = {malloc(get_possibilities(wordcounter) * sizeof(Node *)), 0};
    assert_with_message(leaves.nodes != NULL, SUBSET_MALLOC_FAILED);
    get_leaves(root, &leaves);

    // Opslaan van de paden van blad naar wortel
    NodeList **paths = malloc(leaves.size * sizeof(NodeList *));
    assert_with_message(paths != NULL, SUBSET_MALLOC_FAILED);

    for (int i = 0; i < leaves.size; i++) {
        paths[i] = malloc(sizeof(NodeList));
        assert_with_message(paths[i] != NULL, SUBSET_MALLOC_FAILED);
        paths[i]->nodes = malloc((wordcounter + 1) * sizeof(Node *));
        assert_with_message(paths[i]->nodes != NULL, SUBSET_MALLOC_FAILED);
        paths[i]->size = 0;
        get_path(leaves.nodes[i], paths[i]);
    }

    // De waarden in de boom omzetten naar strings en die opslaan
    destination->paths = paths;
    destination->possibilities = leaves.size;

    free_nodelist_nodes(&leaves);
}

int get_possibilities(int word_count) {
    return (int) pow(2, (word_count - 1));
}

// Geeft de eerste x woorden terug uit een string. Bvb: x=2, "De Sterre Gent" -> "De Sterre"
void get_first_words(int x, char **splitted, int wordcounter, char *destination) {
    strcpy(destination, ""); // String cleanup
    for (int i = 0; i < x; i++) {
        // Omdat de input nooit langer dan MAX_INPUT_LENGTH kan zijn, kan er ook geen buffer overflow optreden
        strcat(destination, splitted[i + wordcounter]);
        if (i != x - 1) { // verwijder spatie op het einde van de string
            strcat(destination, " ");
        }
    }
    unsigned int dest_len = strlen(destination);
    for (int i = 0; i < 4; i++) {
        destination[dest_len + i] = '\0';
    }
}

// Dit stelt de effectieve boomstructuur op
void split(int x, Node *parent, int wordcounter) {
    if (x == 0) {
        return;
    }
    for (int i = 1; i < x + 1; i++) {
        Node *new_node = malloc(sizeof(Node));
        assert_with_message(new_node != NULL, SUBSET_MALLOC_FAILED);
        new_node->value = i;
        new_node->parent = parent;
        new_node->children = malloc(sizeof(Node *) * wordcounter);
        assert_with_message(new_node->children != NULL, SUBSET_MALLOC_FAILED);
        new_node->children_count = 0;
        parent->children[parent->children_count] = new_node;
        parent->children_count++;
        split(x - i, new_node, wordcounter);
    }
}

// Slaat alle bladeren van de boom op in een NodeList
void get_leaves(Node *node, NodeList *leaves) {
    if (node->children_count != 0) {
        for (int i = 0; i < node->children_count; i++) {
            get_leaves(node->children[i], leaves);
        }
    } else {
        leaves->nodes[leaves->size] = node;
        leaves->size++;
    }
}

// Geeft een Nodelist terug van het pad van een blad tot de wortel
void get_path(Node *leave, NodeList *destination) {
    while (leave->parent != NULL) {
        destination->nodes[destination->size] = leave;
        destination->size++;
        leave = leave->parent;
    }
}

// Zet een pad van blad tot wortel om in de overeenkomstige string
void get_stringlist_from_path(NodeList *path, StringList *destination, char **splitted) {
    destination->size = path->size;

    int startvalue = 0;
    for (int j = 0; j < path->size; j++) { // Loopt over nodes in een pad
        destination->list[j] = malloc(MAX_INPUT_LENGTH * sizeof(char));
        assert_with_message(destination->list[j] != NULL, SUBSET_MALLOC_FAILED);

        get_first_words(path->nodes[j]->value, splitted, startvalue, destination->list[j]);
        startvalue += path->nodes[j]->value;
    }
}

void free_list_of_strings(char **list, int elementcounter) {
    for (int i = 0; i < elementcounter; i++) {
        free(list[i]);
    }
}

void free_node_children_recursive(Node *node) {
    for (int i = 0; i < node->children_count; i++) {
        free_node_children_recursive(node->children[i]); // Free kinderen en alles daarmee geassocieerd
        free(node->children[i]);
    }
    free(node->children); // Free de lijst zelf
}

void free_nodelist_nodes(NodeList *nodelist) {
    free(nodelist->nodes);
}

void free_nodelist_list(NodeList **nodelistlist, int elementcounter) {
    for (int i = 0; i < elementcounter; i++) {
        free_nodelist_nodes(nodelistlist[i]);
        free(nodelistlist[i]);
    }
    free(nodelistlist);
}

int get_possible_substrings_count(int wordcounter) {
    // Stel input: De Sterre Gent
    // Gesplist op 1 woord   -> 3 mogelijke substrings ("De", "Sterre", "Gent")
    // Gesplist op 2 woorden -> 2 mogelijke substrings ("De Sterre", "Sterre Gent")
    // Gesplist op 3 woorden -> 1 mogelijke substring  ("De Sterre Gent")
    // Dus totaal:              3 + 2 + 1 = 6 -> Dit is de som van een rekenkundige rij
    // Formule som van rekenkundige rij
    double count = (0.5) * (wordcounter) * (1 + wordcounter);
    return (int) count;
}

void save_substring_order(SubstringPlacement **substring_placements, int current, unsigned int length, unsigned int start_index) {
    substring_placements[current] = malloc(sizeof(SubstringPlacement));
    SubstringPlacement *substring_placement = substring_placements[current];
    assert_with_message(substring_placement != NULL, MALLOC_SEARCH_RESULTS_FAIL);
    substring_placement->wordcount = length;
    substring_placement->start_index = start_index;
}

void free_substring_placements(SubstringPlacement **substring_placements, int size) {
    for (int i = 0; i < size; i++) {
        free(substring_placements[i]);
    }
}

int current_string_is_match(SubstringPlacement *substring_placement, unsigned int length, unsigned int start_index) {
    return substring_placement->start_index == start_index && substring_placement->wordcount == length;
}

int get_correct_substring_placement(SubstringPlacement **substring_placement, unsigned int length, unsigned int start_index) {
    int i = 0;
    while (! current_string_is_match(substring_placement[i], length, start_index)) {
        i ++;
    }
    return i;
}
