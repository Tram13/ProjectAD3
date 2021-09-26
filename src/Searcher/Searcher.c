#include "Searcher.h"
#include "../Helpers/ErrorHandler/ErrorHandler.h"
#include <string.h>
#include "../Helpers/Printer/Printer.h"
#include "../Helpers/UTF8/utf8.h"
#include "../Helpers/UTF8/accents.h"
#include <ctype.h>

unsigned int i_min(unsigned int x, unsigned int y) {
    return x * (x <= y) + y * (y < x);
}

// Deze functie controleert of het nog zinvol is om verder te gaan met het zoeken: indien alle kosten in de matrix
// groter zijn dan de toegelaten kost, mogen we stoppen met zoeken
// Vooral bij lange zoektermen zal dit een merkbaar positief effect hebben, omdat we dan niet de volledige string
// moeten overlopen
int cost_too_high(const unsigned int *nieuwe, unsigned int length) {
    int i = 0;
    while (i < length && nieuwe[i] > MAX_ALLOWED_EDIT_DISTANCE) {
        i++;
    }
    return i == length;
}

unsigned int editdistance_substring(char *search_term, char *text_orig) {
    // Code gebaseerd op cursus en oefening 68
    // d(a, b) == d(b, a), dus de volgorde van de parameters maakt eigenlijk niet uit
    unsigned int text_length = u8_strlen(text_orig);
    unsigned int search_term_length = u8_strlen(search_term);
    int swapped = 0;
    unsigned int vorige[text_length + 1];
    unsigned int nieuwe[text_length + 1];

    // Opgekuiste kopieën (accenten en lowercase)
    uint32_t *cleaned_search = clean_string(search_term);
    uint32_t *cleaned_text = clean_string(text_orig);

    for (int j = 0; j <= text_length; j++) {
        vorige[j] = j; // Beginwaarde van vorige instellen op [0, 1, 2 , 3, 4, 5, ...]
    }
    for (int i = 1; i <= search_term_length; i++) {
        nieuwe[0] = i;
        for (int j = 1; j <= text_length; j++) {
            if (cleaned_search[i - 1] == cleaned_text[j - 1]) {  // Zelfde teken
                nieuwe[j] = vorige[j - 1];
            } else {
                nieuwe[j] = min3(vorige[j - 1] + EDIT_COST, // Substitutie
                                 vorige[j] + EDIT_COST, // Toevoeging
                                 nieuwe[j - 1] + EDIT_COST); // Verwijdering
            }
            if ((!swapped && cleaned_search[i] == cleaned_text[j - 1]) &&
                (cleaned_search[i - 1] == cleaned_text[j]) &&
                (cleaned_search[i] != cleaned_search[i - 1])) { // Verwisselen
                nieuwe[j] -= EDIT_COST;
                swapped = 1;
            }
        }
        if (swapped == 1) { // we kunnen nooit met 3 letters 2x swappen, dus hierdoor voorkomen we dit
            swapped = 2;
        } else {
            swapped = 0;
        }
        copy_list(vorige, nieuwe, text_length + 1);
        if (i > MAX_ALLOWED_EDIT_DISTANCE && cost_too_high(nieuwe, text_length + 1)) {
            free(cleaned_text);
            free(cleaned_search);
            return MAX_ALLOWED_EDIT_DISTANCE + 1;
        }
    }
    free(cleaned_text);
    free(cleaned_search);
    return nieuwe[text_length];
}

void copy_list(unsigned int *destination, const unsigned int *source, unsigned int amount) {
    for (int i = 0; i <= amount; i++) {
        destination[i] = source[i];
    }
}

unsigned int min3(unsigned int a, unsigned int b, unsigned int c) {
    unsigned int min = a;
    if (min > b) {
        min = b;
    }
    if (min > c) {
        min = c;
    }
    return min;
}

// Zoekt in de databank
SearchResults *
do_search(OrderedDatabase *database, char *search_term, SearchResults **searchresults, int current_substring_count) {
    // Plaats in geheugen vrijmaken voor de zoekresultaten
    SearchResults *current_searchresults = init_current_searchresults(searchresults,
                                                                      current_substring_count);
    unsigned int search_term_length = u8_strlen(search_term);
    int current_searchresults_size = DEFAULT_SEARCH_RESULT_SIZE;
    // Grenzen opstellen van zoekopdracht
    // Zoekopdracht zodanig gelimiteerd dat we niet 3 of meer letters kunnen invoegen en/of verwijderen
    // We zoeken dus enkel in de databank naar locaties die ongeveer even lang zijn als de zoekterm, maar die niet buiten de grenzen van de databank vallen
    unsigned int minimal_length;
    unsigned int maximal_length;
    if (search_term_length <
        MAX_ALLOWED_EDIT_DISTANCE - 1) { // We kunnen in de databank natuurlijk niet zoeken op strings met lengte (-1)
        minimal_length = 0;
    } else {
        minimal_length = search_term_length + 1 - MAX_ALLOWED_EDIT_DISTANCE;
    }
    if (search_term_length + MAX_ALLOWED_EDIT_DISTANCE >
        database->size) { // Om ervoor te zorgen dat we niet buiten de databank lezen
        maximal_length = database->size;
    } else {
        maximal_length = search_term_length + MAX_ALLOWED_EDIT_DISTANCE;
    }

    for (unsigned int i = minimal_length; i < maximal_length; i++) {
        // Databank overlopen
        for (int k = 0; k < database->databases[i]->size; k++) {
            // Zoek in de databank
            unsigned int distance = editdistance_substring(search_term,
                                                           database->databases[i]->locations[k]->name);
            unsigned int limit = 1 + (strlen(search_term) / MAX_ALLOWED_EDIT_DISTANCE);
            // Indien de maximale editeerafstand van 3 niet overschreden is
            if (distance <= i_min(MAX_ALLOWED_EDIT_DISTANCE, limit)) {
                save_searchresult(database->databases[i]->locations[k],
                                  &current_searchresults_size, current_searchresults, distance);
            }
        }
    }
    return current_searchresults;
}

// Opslaan van een goed zoekresultaat op de nodige plaats, zodat we het later terugvinden
void save_searchresult(Location *location, int *current_searchresults_size, SearchResults *sr, unsigned int distance) {
    sr->database->locations[sr->database->size] = location;
    sr->distance[sr->size] = distance;
    sr->database->size++;
    sr->size++;
    if (*current_searchresults_size <= sr->database->size) {
        *current_searchresults_size *= 2; // Verdubbelen van resultatenlijst
        sr->database->locations = realloc(sr->database->locations,
                                          *current_searchresults_size * sizeof(Location *));
        assert_with_message(sr->database != NULL, MALLOC_SEARCH_RESULTS_FAIL);
        sr->distance = realloc(sr->distance, *current_searchresults_size * sizeof(unsigned int));
        assert_with_message(sr->distance != NULL, MALLOC_SEARCH_RESULTS_FAIL);
    }
}

SearchResults *init_current_searchresults(SearchResults **searchresults, int current_substring_count) {
    searchresults[current_substring_count] = malloc(sizeof(SearchResults));
    SearchResults *current_searchresults = searchresults[current_substring_count];
    assert_with_message(current_searchresults != NULL, MALLOC_SEARCH_RESULTS_FAIL);

    current_searchresults->distance = malloc(DEFAULT_SEARCH_RESULT_SIZE * sizeof(unsigned int));
    assert_with_message(current_searchresults->distance != NULL, MALLOC_SEARCH_RESULTS_FAIL);

    current_searchresults->database = malloc(sizeof(Database));
    assert_with_message(current_searchresults->database != NULL, MALLOC_SEARCH_RESULTS_FAIL);
    current_searchresults->database->size = 0;

    current_searchresults->database->locations = malloc(DEFAULT_SEARCH_RESULT_SIZE * sizeof(Location *));
    assert_with_message(current_searchresults->database->locations != NULL, MALLOC_SEARCH_RESULTS_FAIL);

    current_searchresults->size = 0;

    return current_searchresults;
}

void free_searchresults(SearchResults **searchresults, int size) {
    for (int i = 0; i < size; i++) {
        free(searchresults[i]->database->locations);
        free(searchresults[i]->database);
        free(searchresults[i]->distance);
        free(searchresults[i]);
    }
}

// Dit genereert alle totale matches, en zoekt dan welke 5 er de beste zijn
void
get_best_matches(SearchResults **searchresults, StringList *stringlist, BestMatches *best_matches, Arguments *args) {
    // Controleer of iedere substring minstens één match heeft, anders hoeven we deze hele functie niet uit te voeren
    int size = stringlist->size;
    unsigned int total_count = 1;
    for (int i = 0; i < size; i++) {
        if (searchresults[i]->size == 0) {
            return;
        }
        total_count *= searchresults[i]->size;
    }

    // Houdt per array bij aan welk element we zaten, we starten natuurlijk op index 0
    unsigned int indices[size];
    memset(&indices, 0, size * sizeof(unsigned int));

    // Houdt bij welke array momenteel de grootste is, vanaf rechts, want dat is makkelijker om te resetten
    int index_biggest = size - 1;

    // Houdt bij op welk element we zitten
    int index_counter = 0;
    while (index_biggest >= 0) { // Zolang er een lijst is die groter is dan huidige lijst


        // Verzamelen van een totale match
        Location *current_match[size];
        unsigned int distance[size];

        for (int i = 0; i < size; i++) {
            current_match[i] = searchresults[i]->database->locations[indices[i]];
            distance[i] = searchresults[i]->distance[indices[i]];
        }

        // Score opslaan
        double score = get_score(current_match, distance, stringlist, args);
        int new_best_match_index = index_new_best_match(best_matches, score);
        if (new_best_match_index != -1) {
            save_score(current_match, size, best_matches, new_best_match_index, score, distance);
        }

        // Zoekt nieuwe grootste lijst, -1 indien niet gevonden ( -> einde while)
        index_biggest = size - 1;
        while (index_biggest >= 0 && (indices[index_biggest] + 1 >= searchresults[index_biggest]->size)) {
            index_biggest--;
        }

        if (index_biggest >= 0) {
            indices[index_biggest]++;

            // Alles aan de rechterkant moet nu terug 0 zijn, want die hebben we volledig overlopen
            memset(&indices[index_biggest + 1], 0, (size - index_biggest - 1) * sizeof(unsigned int));
        }
        index_counter++;
    }
}

// Loopt lineair alle huidige beste matches af om te kijken of er een nieuwe beste match is
// Geeft -1 terug als de nieuwe match slechter is dan alle huidige beste matches
int index_new_best_match(BestMatches *best_matches, double score) {
    double worst_score = score;
    int worst_index = -1;
    for (int i = AMOUNT_BEST_MATCHES - 1; i >= 0; i--) {
        if (best_matches->scores[i] < worst_score) {
            worst_score = best_matches->scores[i];
            worst_index = i;
        }
    }
    return worst_index;
}

BestMatches *init_best_matches() {
    BestMatches *best_matches = malloc(sizeof(BestMatches));
    assert_with_message(best_matches != NULL, MALLOC_SEARCH_RESULTS_FAIL);

    best_matches->all_matches = malloc(AMOUNT_BEST_MATCHES * sizeof(SearchResults *));
    assert_with_message(best_matches->all_matches != NULL, MALLOC_SEARCH_RESULTS_FAIL);

    best_matches->scores = malloc(AMOUNT_BEST_MATCHES * sizeof(double));
    assert_with_message(best_matches->scores != NULL, MALLOC_SEARCH_RESULTS_FAIL);

    for (int i = 0; i < AMOUNT_BEST_MATCHES; i++) {
        best_matches->all_matches[i] = malloc(sizeof(SearchResults));
        assert_with_message(best_matches->all_matches[i] != NULL, MALLOC_SEARCH_RESULTS_FAIL);

        best_matches->all_matches[i]->database = malloc(sizeof(Database));
        assert_with_message(best_matches->all_matches[i]->database != NULL, MALLOC_SEARCH_RESULTS_FAIL);
        best_matches->scores[i] = -1;
    }

    best_matches->size = AMOUNT_BEST_MATCHES;
    return best_matches;
}

// Slaat de score op op de juiste plaats, zodat we die later kunnen gebruiken bij het vergelijken van andere matches
void save_score(Location **total_match, int size, BestMatches *best_matches, int index,
                double score, const unsigned int *distance) {
    // Vorige database-entries vrijgeven, indien die bestaat
    if (best_matches->scores[index] != -1) {
        free(best_matches->all_matches[index]->database->locations);
        free(best_matches->all_matches[index]->distance);
    }
    // Kopie maken van score die op de heap wordt geplaatst
    // We doen dit niet zoalng we niet zeker zijn dat de huidige match een topscore heeft, omdat de stack sneller werkt.
    // Enkel heap als het een nieuwe beste match is.
    // Na testen leek dit in veel gevallen de snellere manier:
    // De for-lus is relatief klein, in de meeste gevallen praktisch constant, dus de kopie maken duurt niet zo lang.
    // En niet steeds moeten malloc-free doen in de while-lus die de totale matches genereert, bespaart dus meer tijd, zeker als de databank groter wordt.
    unsigned int *distance_copy = malloc(size * sizeof(unsigned int));
    assert_with_message(distance != NULL, MALLOC_SEARCH_RESULTS_FAIL);
    // Idem voor total_match
    Location **total_match_copy = malloc(size * sizeof(Location *));
    assert_with_message(total_match_copy != NULL, MALLOC_SEARCH_RESULTS_FAIL);
    for (int i = 0; i < size; i++) {
        distance_copy[i] = distance[i];
        total_match_copy[i] = total_match[i];
    }

    // Nieuwe score opslaan
    best_matches->scores[index] = score;
    // Nieuwe size van de totale match instellen
    best_matches->all_matches[index]->size = size;
    // Nieuw databank-entries opslaan
    best_matches->all_matches[index]->database->locations = total_match_copy;
    best_matches->all_matches[index]->database->size = size;
    best_matches->all_matches[index]->distance = distance_copy;
}

void free_best_matches(BestMatches *best_matches) {
    for (int i = 0; i < AMOUNT_BEST_MATCHES; i++) {
        if (best_matches->scores[i] != -1) {
            free(best_matches->all_matches[i]->distance);
            free(best_matches->all_matches[i]->database->locations);
        }
        free(best_matches->all_matches[i]->database);
        free(best_matches->all_matches[i]);
    }
    free(best_matches->scores);
    free(best_matches->all_matches);
    free(best_matches);
}

// Van een totale match wordt gevraagd enkel de databank-lijn met de hoogte rang te printen
// Deze functie zoekt de Location met de hoogte rang
Location *get_highest_rank_location(Location **locations, unsigned int size) {
    int index_max = -1;
    double max_rank = -1;
    for (int i = 0; i < size; i++) {
        if (max_rank < locations[i]->rank) {
            max_rank = locations[i]->rank;
            index_max = i;
        }
    }
    return locations[index_max];
}

// Vervangt hoofdletters en accenten
uint32_t *clean_string(char *string) {
    // Variabelen initialiseren
    unsigned int u8_len = u8_strlen(string);
    uint32_t *cleaned = malloc((u8_len + 1) * sizeof(uint32_t));
    assert_with_message(cleaned != NULL, MALLOC_SEARCH_RESULTS_FAIL);
    int j = 0;
    for (int i = 0; i <= u8_len; i++) {
        cleaned[i] = remove_accent(u8_nextchar(string, &j));
        if (cleaned[i] < 128) { // Indien de waarde kleiner is dan 128, gaat het om een ASCII-teken
            cleaned[i] = tolower(cleaned[i]);
        }
    }
    return cleaned;
}

// n² sorteeralgoritme, met n = 5
void sort_best_matches(BestMatches *best_matches) {
    // We sorteren 5 elementen éénmalig, hoeft niet zo efficient te zijn
    // Sorteren met qsort zou ook mogelijk zijn, maar de huidige structuur laat dat momenteel niet toe
    for (int i = 0; i < AMOUNT_BEST_MATCHES; i++) {
        for (int j = 0; j < AMOUNT_BEST_MATCHES; j++) {
            if (best_matches->scores[i] > best_matches->scores[j]) {
                double temp_score = best_matches->scores[i];
                best_matches->scores[i] = best_matches->scores[j];
                best_matches->scores[j] = temp_score;
                SearchResults *temp_results = best_matches->all_matches[i];
                best_matches->all_matches[i] = best_matches->all_matches[j];
                best_matches->all_matches[j] = temp_results;
            }
        }
    }
}