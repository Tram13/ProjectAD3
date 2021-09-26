Opgave project DA3
==================

Dit jaar onderzoeken we in het project goede manieren om benaderend een
string te vinden in een databank. In dit specifieke geval gaat het om
het vinden van straten bij naam. Als iemand bijvoorbeeld
"vrijheid**s**traat brugge" opzoekt, willen we als resultaat
"Vrijheid**ss**traat Brugge" (met 2 s-en) krijgen en niet een van de 14
andere "Vrijheidstraat"en die niet in Brugge liggen. Om dit te doen
zullen we onder andere gebruik maken van het Shift-AND algoritme.

Functionele vereisten
---------------------

Schrijf een programma `openseekmap` dat zoekopdrachten kan uitvoeren op
een databank van plaatsen. Dit programma moet starten met het inlezen
van de databank. Eens die is ingelezen moet een vraagteken gevolgd door
een newline-teken (`?\n`) naar de standaard uitvoer worden geschreven.
Daarna worden query's verwerkt.

Het programma neemt als standaard invoer een UTF-8 query string. Uit
deze string mogen alle controlekarakters en leestekens vervangen worden
door spaties (behalve newline-tekens) en opeenvolgende spaties mogen
samengevat worden tot 1 spatie. Er kunnen meerdere zoekopdrachten na
elkaar gegeven worden gescheiden door het newline-teken.

    $ echo "De Sterre Gent" | openseekmap database.data

Als je een query hebt van $\bar n$ woorden (gescheiden door spaties),
kun je die op verschillende manieren in stukken verdelen. Voor "De
Sterre Gent" heb je bijvoorbeeld \["De", "Sterre", "Gent"\], \["De
Sterre","Gent"\], \["De", "Sterre Gent"\] en \["De Sterre Gent"\]. Zo
zijn er $2^{\bar n - 1}$ opsplitsingen. Voor elk van die opsplitsingen,
gaan we op zoek naar *totale matches* zoals hieronder beschreven. We
selecteren dan de beste *totale matches* uit alle gevonden *totale
matches* voor alle mogelijke opsplitsingen. Wat "beste" hier precies
betekent zien we later.

Voor een opgesplitste zoekopdracht $z=z_1... z_n$, zoek je elk stuk
($z_i$) afzonderlijk op. Voor elk stuk $z_i$ maak je een verzameling
$M_i$ van plaatsen in de databank met een editeerafstand van ten hoogste
$\min(3,1+|z_i|/3)$. Hiervoor gebruik je onder andere Shift-AND. Een
*totale match* V voor een opgesplitste zoekopdracht z met n
stukken is een opeenvolging $v_1... v_n$, zodat
$\forall i: v_i\in M_i$.

Je programma moet ook plaatsen kunnen zoeken rond een bepaalde punt.
Hiertoe moet je programma twee optionele argumenten aanvaarden. Deze
stellen samen de lengte- en breedtegraad geven waar rond gezocht moet
worden (er worden steeds 2 argumenten gegeven). Onderstaande oproep naar
je programma moet "De Sterre" zoeken rond het station Gent Sint-Pieters
([geo:51.0355237,
3.7107158](https://www.openstreetmap.org/node/1178257779)). De lengte-
en breedtegraad worden gegeven als een kommagetal (met punt als decimaal
scheidingsteken).

    $ echo "De Sterre" | openseekmap database.data 51.0355237 3.7107158

### Databank

Je programma gebruikt een databank waarvan het pad wordt meegegeven als
eerste argument. Dit bestand bevat verschillende plaatsen die door een
newline-karakter (`\n`) zijn gescheiden. Voor elk van de plaatsen zijn
er 5 velden gescheiden door een tab-teken (`\t`):

-   Een openstreetmap identifier van de plaats (numerieke id: `unit_64`)
-   Een naam (UTF-8 string)
-   Een rang (natuurlijk getal tussen 1 en 30)
-   Een lengteligging (numeriek: `double`)
-   Een breedteligging (numeriek: `double`)

Sommige plaatsen komen meerdere keren voor, omdat ze meerdere namen
hebben.

Van deze eigenschappen is de rang de enige die niet voor zichzelf
spreekt. De rang is een waarde tussen 1 en 30 die aangeeft hoe algemeen
de plaats is. Specifiekere plaatsen (zoals straten) krijgen een hoge
rang, algemenere regio's krijgen een lage rang. Onderstaande tabel geeft
voor elke rang een "`extent`":

   Rang   Voorbeelden                             `extent`
  ------- --------------------------------------- -----------
    1-3   Oceanen, Continenten                    ±10000 km
     4    Landen                                  ±1000 km
    5-9   Staten, Regio's, Provincies, Gewesten   ±250 km
   10-12  Provincies                              ±75 km
   13-16  Groot Steden                            ±7.5 km
   17-18  Steden, gemeenten                       ±4 km
    19    Dorpen, deelgemeenten                   ±2 km
    20    Buurten, kwartieren                     ±1 km
   21-25  Blokken, straten                        ±500m
   25-30  Gebouwen, punten                        ±50m

De `extent` geeft een benadering voor de radius waarbinnen je kunt
zeggen dat je "op die plaats bent". Deze waarde zullen we gebruiken in
onze sorteer-heuristiek. Exacte waarden die we zullen gebruiken elke
rang op vind je op het einde van dit document.

### Editeerafstand

De definitie van de editeerafstand die we voor dit project hanteren is
de volgende:

Fouten die genegeerd mogen worden:

-   Accenten (zie `accents.h` en `accents.c` in de `assignment` map)
-   Hoofdletters (enkel `A-Z` ≈ `a-z`)
-   Hoofdletters en accenten (vb: `ë` ≈ `É`)

Fouten met kost 1:

-   Letter invoegen (ook letters die te veel zijn op het einde): `ab`
    →`abc`
-   Letter verwijderen: `abc` → `ac`
-   Letter vervangen: `abc` → `adc`
-   Letters wisselen: `abc` →`acb`

Niet toegestaan (kost ∞):

-   3 of meer letters invoegen en/of verwijderen.

Je kunt een correctheid toekennen aan een match v voor een
zoekopdracht w door de lengte van w te delen door de editeerafstand
van v tot w. De correctheid $\mathsf{C}$ van een totale match wordt
als volgt bepaald:


\mathsf{C}(v_1v_2... v_n) = \frac{\sum_{i\in\{1,...,n\}}|z_i|-d(z_i,v_i)}{\sum_{i\in\{1,...,n\}}|z_i|}


Alleen matches met stukken die allemaal een editeerafstand kleiner of
gelijk aan $\min(3,1+|z_i|/3)$ komen in aanmerking.

### Synergie

Een zoekopdracht kan uit meerdere woorden bestaan, dan komt het er op
aan om de beste match te vinden die aan zo veel mogelijk woorden
tegelijk voldoet. Voor elke mogelijke opdeling van de zoekopdracht in
stukken zoek je benaderende matches voor elk deel. Stel dat de
zoekopdracht opgesplitst is als $w_1 w_2 ... w_n$, en dat de
benaderende matches voor stuk $w_i in M_i$ zitten dan kun je de
"synergie" $\mathsf{S}(v_1 v_2 ... v_n)$ van een combinatie matches
$v_1 v_2 ... v_n$ (met $\forall i: v_i \in M_i$) bepalen met:


\mathsf{S}_1(v_i,v_j) = \min\left[2,  \left(\frac{\max\left[extent(v_i),extent(v_j))\right]}{d_{geo}(v_i,v_j)}\right)^2 \right]
\qquad
\mathsf{S}(v_1 v_2 ... v_n) = \sum_{\substack{i<j\\i,j\in\{1,...,n\}}}\frac{ \mathsf{S}_1(v_i,v_j)}{n(n-1)}


Waarbij $d_{geo}$ de afstand in vogelvlucht tussen 2 punten is. Deze
afstand kun je met [haversine
formula](https://en.wikipedia.org/wiki/Haversine_formula) bepalen die op
het einde van de opgave in C staat geïmplementeerd (bron [Rosetta
Code](https://rosettacode.org/wiki/Haversine_formula#C)). Het resultaat
is in km.

### Sortering

We zoeken naar de 5 beste matches die een maximale waarde geven voor
volgende gewogen som:

-   20% correctheid
-   70% Synergie
-   10% voor $\max_{i}(rank(v_i))/30$

Als het programma een locatie meekrijgt (2de en 3de argument) dan gaat
het sorteren volgens de onderstaande metriek:

-   15% correctheid
-   55% Synergie
-   15% voor $\max_{i}(rank(v_i))/30$
-   15% voor
    $\min\left[1,\max\left[0,1-\log_{1000}\left(\frac{d_{geo}(v_s,POS)}{extent(v_s)}\right)\right]\right]$
    waarbij $d_{geo}(v_s,POS)$ de afstand in vogelvlucht is
    tussen het meest specifieke element van de match en de meegegeven
    positie.

Waarbij $v_s$ het meest specifieke element (hoogste rang) van de match.
Er kan op aanvraag afgeweken worden van de sortering in samenspraak met
de assistent van het vak.

### Uitvoer

Om een match uit te schrijven neem je het meest specifieke element uit
de match (het element met de hoogste `rang`). Hiervoor schrijf je dan
het volgende naar standaard uitvoer:

-   een newline (`\n`)
-   Naam van het element
-   spatie
-   haakje open "`(`"
-   de openstreetmap identifier
-   haakje sluiten "`)`"
-   een newline (`\n`)
-   Coördinaten van het element als een `geo` link:
    $geo:latitude,longitude$. Waarbij
    -   latitude de lengteligging van het element is
    -   longitude de breedteligging van het element is
-   een newline (`\n`)
-   het teken `~`
-   een newline (`\n`)

Na het uitschrijven van de 5 beste matches schrijf je een vraagteken
(`?`) naar standaard uitvoer op een nieuwe lijn. Is er nog een query
gegeven dan voer je die uit en herhaal je dit process.

Onderzoek
---------

Er zijn verschillende manieren waarop je je zoekmachine kunt
optimaliseren. Om te weten of je optimalisaties goed zijn is het
belangrijk om te benchmarken. Je kunt differentiëren tussen
verschillende algoritmen in je programma met compiler vlaggen waarmee je
dan kunt bepalen welke `c` file wordt ingeladen. Zorg ervoor dat als je
programma zonder specifieke vlaggen wordt gecompileerd, het beste
algoritme word gekozen.

*Belangrijk:* Heb je een experiment uitgevoerd en bleek het niet goed te
werken, vermeld dit dan ook in je verslag. Negatieve resultaten zijn ook
resultaten.

#### Algoritmes voor editeerafstand

Zijn er methoden die je kunt gebruiken om een preselectie te maken van
mogelijke matches voordat je effectief nagaat wat de editeerafstand is?
Wat is de snelste manier om de editeerafstand te bepalen? Heb je
bestaande algoritmen aangepast om ze sneller te maken voor deze context?

Vermeld in je verslag wat je hebt geprobeerd en wat het beste was.

#### Preprocessing

Een mogelijke manier om het zoeken te versnellen is door de databank te
preprocessen. Zo kun je de data eventueel
opsplitsen/sorteren/herschrijven/in een andere datastructuur steken/...
Dit kan een opvallende impact hebben op de uitvoeringstijd van je
programma.

Vermeld in je verslag wat je hebt geprobeerd en wat het beste was.

#### Bounding

Als je zoekt naar oplossingen kan het voorkomen dat je op voorhand al
weet dat dit resultaat niet in de top 5 zal zitten. Kijk of het mogelijk
is om het zoeken al voortijdig af te breken.

Vermeld in je verslag wat je hebt geprobeerd en wat het beste was.

#### UTF-8

De UTF-8 karakterset is in staat om 1,112,064 mogelijke karakters voor
te stellen. Als je Shift-AND implementeert, is het misschien niet zo
efficiënt om 1,112,064 karakteristieke vectoren (8.897 MB) te berekenen
en op te slaan als je daar waarschijnlijk minder dan 1% van zult
gebruiken. Vergelijk de performantie van het opslaan van de
karakteristieke vectoren en een andere aanpak die je zelf ontwerpt.
Beschrijf je resultaten in het verslag. In de assignment map kun je 2
files (`utf8.c` en `utf8.h`) vinden die het werken met UTF-8 strings
vereenvoudigen.

*Tip*: Het is misschien interessant om te beginnen met een ASCII variant
van de dataset. Deze is ook te vinden in de repository als `ascii.data`.
Eens je algoritme daarmee werkt pas je je code aan om te werken met
UTF-8.

Specificaties
-------------

### Programmeertaal

In de opleidingscommissie informatica (OCI) werd beslist dat, om meer
ervaring in het programmeren in C te verwerven, het project van
Algoritmen en Datastructuren 3 in C geïmplementeerd moet worden. Het is
met andere woorden de bedoeling je implementatie in C uit te voeren. Je
implementatie moet voldoen aan de ANSI-standaard. Je mag hiervoor dus
gebruikmaken van de laatste features in C18, voor zover die ondersteund
worden door `gcc` versie 10.2.0 of 9.3.0 op SubGIT.

Voor het project kun je de standaard libraries gebruiken; externe
libraries zijn echter niet toegelaten, behalve de UTF-8 library en
`remove_accents` code die je van ons krijgt (die kopieer je naar je src
map). Het spreekt voor zich dat je normale, procedurele C-code schrijft
en geen platformspecifieke APIs (zoals bv. de Win32 API) of features uit
C++ gebruikt. Op Windows bestaat van enkele functies zoals `qsort` een
"safe" versie (in dit geval `qsort_s`), maar om je programma te kunnen
compileren op een Unix-systeem kun je die versie dus niet gebruiken.

**Wat je ontwikkelingsplatform ook mag zijn, controleer geregeld de
output van SubGIT om te verifiëren dat je programma ook bij ons
compileert!**

### Input-Output en implementatiedetails

Voor de in- en uitvoer gebruiken we de standaard `stdin` en `stdout`
streams.

Je programma moet dus bijvoorbeeld als volgt gebruikt kunnen worden:

    $ cat input
    De Sterre Gent
    Vrijheidstraat Brugge
    $ cat input | ./openseekmap database.data
    ?
    De sterre (2659815473)
    geo:51.0262973,3.7110885
    ~
    ...

### Limieten

Voor de UTF-8 België
[belgie.data](https://ufora.ugent.be/d2l/common/dialogs/quickLink/quickLink.d2l?ou=234097&type=content&rcode=5B0748E6-E75C-4A93-8875-E034639B31CD-519535)
dataset die je kreeg moet een zoekopdracht van hoogstens 3 woorden
binnen 15 seconden voltooid worden en mag je programma niet meer dan
50MiB geheugen gebruiken. Deze limieten zijn zeer ruim gerekend, je
implementatie is waarschijnlijk niet goed als je er net onder zit.

Verslag
-------

Schrijf een bondig verslag over je werk. Vertel in je verslag welke
ontwerpbeslissingen je hebt genomen. Maak in je verslag ook een korte
analyse van de tijdscomplexiteit van jouw programma in functie van de
lengte van de zoekstring, het aantal woorden in de zoekstrings, het
aantal elementen in de databank (en eventueel andere metrieken die een
invloed hebben).

Maak je verslag niet onnodig lang.

Indienen
--------

### Directorystructuur

Je indiening moet volgende structuur hebben:

-   `src/` bevat alle broncode (inclusief je eigen Makefiles, *exclusief
    gecompileerde bestanden*).

-   `tests/` alle testcode (je moet testcode hebben, push geen grote
    datafiles naar git, plaats een scriptje die ze genereert).

-   `extra/verslag.pdf` bevat de elektronische versie van je verslag. In
    deze map kun je ook eventueel extra bijlagen plaatsen.

-   `assignment/` bevat de laatste versie van de opgave zoals je die
    binnen haalt van
    `git@SubGIT.UGent.be:2020-2021/DA3/project-assignment`. Je past best
    niets aan in deze map.

-   `gcc-version` een tekstbestand met daarin de tekst "9.3.0" of
    "10.2.0" afhankelijk van welke versie van GCC je gebruikt

-   `sources` is een tekstbestand dat de `.c` bronbestanden voor je
    project oplijst, 1 per lijn relatief t.o.v. de `src` directory.
    Bijvoorbeeld:

        main.c
        preprocessors/sort_preprocessor.c

Je directory structuur ziet er dus ongeveer zo uit:

    |
    |-- extra/
    |   `-- verslag.pdf
    |-- src/
    |   `-- je broncode
    |-- tests/
    |   `-- je testcode
    |-- assignment/
    |   `-- de opgave
    |-- sources
    `-- gcc-version

Al je bestanden moeten als UTF-8 opgeslagen zijn.

### Compileren

De code zal bij het indienen gecompileerd worden met (ruwweg)
onderstaande opdracht door SubGIT met GCC versie 9.3.0 of 10.2.0
(afhankelijk van de inhoud van `gcc-version`) in je `src` map.

    gcc -std=c18 -O3 -Wall -Werror -lm $(cat ../sources)

Op Ufora zal de Dockerfile en bijhorende bronbestanden staan die SubGIT
gebruikt om jouw code te compileren en minimale testen op uit te voeren.
Je kunt deze Docker ook onmiddellijk van Dockerhub halen met volgende
commando's:

    docker pull beardhatcode/da3-project-2020-2021:latest
    docker run -it --rm --mount type=bind,source={PAD},destination=/submission,readonly beardhatcode/da3-project-2020-2021:latest

Waarbij `{PAD}` vervangen dient te worden met het absolute pad naar de
hoofdmap (niet `src`) van je code.

### SubGIT

Het indienen gebeurt via het [SubGIT](https://SubGIT.ugent.be/)
platform. Indien je hier nog geen account op hebt, dien je deze aan te
maken.

#### Repository afhalen

    git clone git@SubGIT.ugent.be:2020-2021/DA3/project/{studentnr} projectDA3

#### Opgave als `upstream` instellen

Je kunt de opgave en boilerplate voor het project afhalen door de opgave
repository als upstream met volgende commando's in de `projectDA3` map:

    git remote add upstream git@SubGIT.UGent.be:2020-2021/DA3/project-assignment
    git pull upstream master

(Je kunt niet pushen naar de upstream, alleen de lesgevers kunnen dat.)

#### Feedback

Als je pusht naar SubGIT, zul je in je terminal te zien krijgen of je
code voldoet aan de minimumvereisten. In dat geval krijg je bij het
pushen de melding dat het pushen geslaagd is:

    remote: Acceptable submission

Je kunt geen code pushen naar de `master` branch als die niet compileert
of niet aan de minimale IO vereisten voldoet. Je kunt echter wel pushen
naar andere branches en daar zal je push wel aanvaard worden.

#### `master` branch

De `master` branch op SubGIT stelt jouw indiening voor. Je kunt voor de
deadline zoveel pushen als je wilt. Zorg ervoor dat je voor de deadline
zeker je finale versie naar de **`master`** branch hebt gepushed.

#### Controleren of je zeker goed hebt ingediend

Je kunt jouw indiening bekijken door jouw repository nog eens te klonen
in een andere map

    cd eenAndereMap
    git clone git@SubGIT.ugent.be:2020-2021/DA3/project/{studentnr} projectDA3Controle

### Deadlines en belangrijke data

Zorg ervoor dat je eerste tussentijdse versie hebt gepusht hebt naar een
branch[^1] op SubGIT voor **2020-11-28 om 13:37:42**. De versie die je
hier upload zal niet bekeken worden, we vragen je dit te doen om er
zeker van zijn dat iedereen kan indienen. Als je niets pusht voor deze
deadline verlies je al je punten voor het project.

De code die op **2020-12-02 om 13:37:42** op de `master` branch staat is
je finale code indiening. Enkel code die op dat moment de `master`
branch staat wordt verbeterd. **Geen code op de `master` branch is nul
op vier voor het project.** Je kunt na deze deadline nog steeds pushen
om wijzigingen aan te brengen aan het verslag.

Na **2019-12-09 om 13:37:42** kun je ook helemaal geen wijzigingen meer
aanbrengen aan je repo. Het verslag dat aanwezig is als PDF op de
`master` branch in `extra/verslag.pdf` is je finaal verslag.

Algemene richtlijnen
--------------------

-   Schrijf efficiënte code, maar ga niet over-optimaliseren: **geef de
    voorkeur aan elegante, goed leesbare code**. Kies zinvolle namen
    voor methoden en variabelen en voorzie voldoende commentaar.
-   Op Ufora staat een Dockerfile waarmee je een Linux container kunt
    bouwen die jou code compileert en minimale testen uitvoert. Als alle
    testen slagen zal dit programma `AANVAAARD` uitschrijven als laatste
    regel en stoppen met exit code 0. Code die hier niet aan voldoet,
    zal geweigerd worden door SubGIT en levert geen punten op (0/4 voor
    het project).
-   Het project wordt gequoteerd op **4** van de 20 te behalen punten
    voor dit vak, en deze punten worden ongewijzigd overgenomen naar de
    tweede examenperiode.
-   Projecten die ons niet (via de `master` branch op SubGIT) bereiken
    voor de deadline worden niet meer verbeterd: dit betekent het
    verlies van alle te behalen punten voor het project.
-   Dit is een individueel project en dient dus door jou persoonlijk
    gemaakt te worden. Het is uiteraard toegestaan om andere studenten
    te helpen of om ideeën uit te wisselen, maar **het is ten strengste
    verboden code uit te wisselen**, op welke manier dan ook. Het
    overnemen van code beschouwen we als fraude (van **beide** betrokken
    partijen) en zal in overeenstemming met het examenreglement
    behandeld worden. Op het internet zullen ongetwijfeld ook (delen
    van) implementaties te vinden zijn. Het overnemen of aanpassen van
    dergelijke code is echter **niet toegelaten** en wordt gezien als
    fraude. (Als je je code op GitHub/Bitbucket/sourcehut/Gitlab/...
    plaatst moet je die privaat houden tot de bekendmaking van de
    eerstezits examenresultaten van het vak)
-   Essentiële vragen worden **niet** meer beantwoord tijdens de laatste
    week voor de deadline.

Vragen
------

Op Ufora zal een lijst van veelgestelde vragen staan.

Als je vragen hebt over de opgave of problemen ondervindt, dan kun je je
vraag stellen tijdens het vragenuurtje op dinsdagvoormiddag. Alternatief
kun je ook terecht op het forum op Ufora. Contacteer je ons per mail,
stuur dan ook je studentennummer en de branch waarop de code met het
probleem zit mee. Zo kunnen we onmiddellijk je code pullen. Stuur geen
screenshots van code of foutboodschappen.

Appendix
========

Haversine formule
-----------------

Een numeriek stabiele implementatie van de [haversine
formula](https://en.wikipedia.org/wiki/Haversine_formula) vind je
hieronder (bron [Rosetta
Code](https://rosettacode.org/wiki/Haversine_formula#C)).

    #include <stdio.h>
    #include <stdlib.h>
    #include <math.h>

    // Straal van de aarde in km
    #define R 6371
    #define TO_RAD (3.1415926536 / 180)
    double d_geo(double lat1, double lon1, double lat2, double lon2)
    {
        double dx, dy, dz;
        lon1 = lon1 - lon2;
        lon1 *= TO_RAD, lat1 *= TO_RAD, lat2 *= TO_RAD;

        dz = sin(lat1) - sin(lat2);
        dx = cos(lon1) * cos(lat1) - cos(lat2);
        dy = sin(lon1) * cos(lat1);
        return asin(sqrt(dx * dx + dy * dy + dz * dz) / 2) * 2 * R; // km
    }

Voorbeeld inlezen UTF-8
-----------------------

    #include <stdio.h>      /* printf */
    #include "utf8.h"       /* u8_ functions */

    int main(){
        char str[]="Foo©bar𝌆baz☃qux\0";
        int i = 0,j = 0;
        unsigned int c;
        while(c = u8_nextchar(str,&i)){
            printf("%d is the charcode of the first UTF-8 character in '%s' (bytes %d-%d)\n", c, str+j, j, i);
            j = i;
        }
    }

output:

-   70 is the charcode of the first UTF-8 character in 'Foo©bar𝌆baz☃qux'
    (bytes 0-1)
-   111 is the charcode of the first UTF-8 character in 'oo©bar𝌆baz☃qux'
    (bytes 1-2)
-   111 is the charcode of the first UTF-8 character in 'o©bar𝌆baz☃qux'
    (bytes 2-3)
-   **169** is the charcode of the first UTF-8 character in
    '©bar𝌆baz☃qux' (bytes 3-5)
-   98 is the charcode of the first UTF-8 character in 'bar𝌆baz☃qux'
    (bytes 5-6)
-   97 is the charcode of the first UTF-8 character in 'ar𝌆baz☃qux'
    (bytes 6-7)
-   114 is the charcode of the first UTF-8 character in 'r𝌆baz☃qux'
    (bytes 7-8)
-   **119558** is the charcode of the first UTF-8 character in
    '𝌆baz☃qux' (bytes 8-12)
-   98 is the charcode of the first UTF-8 character in 'baz☃qux' (bytes
    12-13)
-   97 is the charcode of the first UTF-8 character in 'az☃qux' (bytes
    13-14)
-   122 is the charcode of the first UTF-8 character in 'z☃qux' (bytes
    14-15)
-   **9731** is the charcode of the first UTF-8 character in '☃qux'
    (bytes 15-18)
-   113 is the charcode of the first UTF-8 character in 'qux' (bytes
    18-19)
-   117 is the charcode of the first UTF-8 character in 'ux' (bytes
    19-20)
-   120 is the charcode of the first UTF-8 character in 'x' (bytes
    20-21)

Rang data
---------

Mapping rang → extend in km.

    1   100000
    2   50000
    3   10000
    4   1000
    5   750
    6   600
    7   500
    8   350
    9   200
    10  100
    11  75
    12  50
    13  10
    14  9
    15  7.5
    16  6
    17  4.5
    18  4
    19  2
    20  1
    21  0.8
    22  0.7
    23  0.6
    24  0.5
    25  0.4
    26  0.3
    27  0.2
    28  0.1
    29  0.05
    30  0.01

Data generatie
--------------

Met volgende shell code kun je zelf datasets maken.

    # get data
    wget https://github.com/OSMNames/OSMNames/releases/download/v2.1.1/planet-latest_geonames.tsv.gz

    # extract belgium
    zcat planet-latest_geonames.tsv.gz | \
        awk -F $'\t' 'BEGIN {OFS = FS} {if(NR!=1){if($16 =="be"){print}}else{print}}' | \
        gzip - > countryExtract.tsv.gz

    # split double names, add alt names (+ ignore errors in Brussel)
     zcat countryExtract.tsv.gz | \
        awk -F $'\t' 'BEGIN{OFS="\t"} {n=split($1,a,";");for(i=1;i<=n;i++){print($4,a[i],9,7,$8)}} $9 > 16 && $2 != "\"\"" && $2 ~ /^[A-Za-z ]*$/ {print(4,2,9,7,$8)}'

[^1]: Niet noodzakelijk `master`, moet zelfs niet compileren.
