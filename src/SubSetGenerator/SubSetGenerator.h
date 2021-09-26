#ifndef PROJECTAD3_SUBSETGENERATOR_H
#define PROJECTAD3_SUBSETGENERATOR_H

#define SUBSET_MALLOC_FAILED "Could not process input."

typedef struct StringList {
    char **list;
    int size;
} StringList;

typedef struct Node {
    int value;
    struct Node *parent;
    struct Node **children;
    int children_count;
} Node;

typedef struct NodeList {
    struct Node **nodes;
    int size;
} NodeList;

typedef struct SubSets {
    NodeList **paths; // Lijst van lijsten van strings
    int possibilities;
} SubSets;

typedef struct SubstringPlacement {
    unsigned int wordcount;
    unsigned int start_index;
} SubstringPlacement;

void generate_subsets(SubSets *destination, int wordcounter, Node *root);

int get_possibilities(int word_count); // Geeft aantal mogelijkheden terug voor een gegeven aantal woorden

void get_first_words(int x, char **string, int wordcounter,
                     char *destination); // Zet de eerste x woorden van de inputstring in destination

void split(int x, Node *parent, int wordcounter);

void get_leaves(Node *node, NodeList *leaves);

void get_path(Node *leave, NodeList *destination);

void get_stringlist_from_path(NodeList *path, StringList *destination, char **splitted);

void free_list_of_strings(char **list, int elementcounter);

void free_node_children_recursive(Node *node);

void free_nodelist_nodes(NodeList *nodelist);

void free_nodelist_list(NodeList **nodelistlist, int elementcounter);

int get_possible_substrings_count(int wordcounter);

void save_substring_order(SubstringPlacement **substring_placements, int current, unsigned int length, unsigned int start_index);

void free_substring_placements(SubstringPlacement **substring_placements, int size);

int current_string_is_match(SubstringPlacement *substring_placement, unsigned int length, unsigned int start_index);

int get_correct_substring_placement(SubstringPlacement **substring_placement, unsigned int length, unsigned int start_index);

#endif //PROJECTAD3_SUBSETGENERATOR_H
