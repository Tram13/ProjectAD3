#!/bin/bash

# Enkele variabelen en functies definieren
GREEN="\e[32m"          # Om groen te printen
RED="\e[31m"            # Om rood te printen
DEFAULT_COLOUR="\e[39m" # Om de standaardkleur te printen

function clean_up() { # Om alle gegenereerde bestanden weer te verwijderen
    cd ../..
    echo "Cleaning up tests"
    rm -r build-tests
}

if ! command -v dos2unix &>/dev/null; then
    echo -e "$RED\c"
    echo "dos2unix not installed. While the script might run correctly without dos2unix, it is recommended to install it."
    echo -e "$DEFAULT_COLOUR\c"
else
    # Mijn computer heeft de onweerstaanbare drang om die bestanden op te slaan in Windows-formaat
    # Daarom voor de zekerheid toch nog even garanderen dat de oplossingen in Linux-formaat staan
    dos2unix TestSolutions/test1 &>/dev/null
    dos2unix TestSolutions/test2 &>/dev/null
    dos2unix TestSolutions/test3 &>/dev/null
    dos2unix TestSolutions/test4 &>/dev/null
    dos2unix TestSolutions/test5 &>/dev/null
    dos2unix TestSolutions/test6 &>/dev/null
    dos2unix generateData &>/dev/null
fi

echo "Starting to run tests"

# Om enkele aparte functies van openseekmap te testen, zoals bijvoorbeeld de zoekfunctie en het verwerken van argumenten
# Het idee is vergelijkbaar met "JUnit"
echo "Compiling function tests"
cmake FunctionTests/CMakeLists.txt -B ../build-tests/tests &>/dev/null
cd ../build-tests/tests || exit 1
make &>/dev/null
if [ -f tests ]; then
    echo -e "$GREEN\c"
    echo "Compilation successful"
    echo -e "$DEFAULT_COLOUR\c"
else
    echo -e "$RED\c"
    echo "Failed to build function tests, aborting"
    echo -e "$DEFAULT_COLOUR\c"
    clean_up
    exit 1
fi

echo "Running function tests"
if ./tests database.data 51.0355237 3.7107158; then
    echo -e "$GREEN\c"
    echo "All function tests successfully executed"
    echo -e "$DEFAULT_COLOUR\c"
else
    echo -e "$RED\c"
    echo "Function tests failed, aborting"
    echo -e "$DEFAULT_COLOUR\c"
    clean_up
    exit 2
fi

# Om het effectieve programma "openseekmap" te testen, een beetje vergelijkbaar met "integratietesten"
echo "Compiling openseekmap"
cd ../../src || exit 3
cmake CMakeLists.txt -B ../build-tests/openseekmap &>/dev/null
cd ../build-tests/openseekmap || exit 3
make &>/dev/null
if [ -f openseekmap ]; then
    echo -e "$GREEN\c"
    echo "Compilation successful"
    echo -e "$DEFAULT_COLOUR\c"
else
    echo -e "$RED\c"
    echo "Failed to build openseekmap, aborting"
    echo -e "$DEFAULT_COLOUR\c"
    clean_up
    exit 3
fi

echo "Generating database"
mkdir "data"
if [ -f ../../data/belgie_downloaded.data ]; then
    echo "Database already downloaded"
    cp ../../data/belgie_downloaded.data data/belgie.data
else
    echo "Downloading database"
    ../../tests/generateData.sh
    mkdir ../../data/ &>/dev/null
    cp belgie.data ../../data/belgie_downloaded.data
    mv belgie.data data/belgie.data
fi
echo "Running full tests"
counter=0 # Telt hoeveel fouten we maken

# Test 1, databank met maar 1 lijn, we zoeken de exacte lijn
echo "Les Tris" >data/input
echo -e "3804621676\tLes Tris\t20\t4.2748774\t50.4511813" >data/data1
./openseekmap data/data1 <data/input >data/output
if ! diff data/output ../../tests/TestSolutions/test1; then
    echo -e "$RED\c"
    echo "Failed test"
    echo -e "$DEFAULT_COLOUR\c"
    ((counter++))
fi

# Test 2, databank met maar 1 lijn, we zoeken een compleet andere lijn
echo "De Sterre Gent" >data/input
echo -e "3804621676\tLes Tris\t20\t4.2748774\t50.4511813" >data/data1
./openseekmap data/data1 <data/input >data/output
if ! diff data/output ../../tests/TestSolutions/test2; then
    echo -e "$RED\c"
    echo "Failed test"
    echo -e "$DEFAULT_COLOUR\c"
    ((counter++))
fi

# Test 3, databank van belgie, we zoeken "De Sterre Gent"
echo "De Sterre Gent" >data/input
./openseekmap data/belgie.data <data/input >data/output
head -n 8 <data/output >data/output_first2results # We verwachten dat "De Sterre" in Gent op de 1ste en 2de plaats staat (want het zit 2x in de databank)
if ! diff data/output_first2results ../../tests/TestSolutions/test3; then
    echo -e "$RED\c"
    echo "Failed test"
    echo -e "$DEFAULT_COLOUR\c"
    ((counter++))
fi

# Test 4, databank van belgie, we zoeken "Korenmarkt Gent"
echo "Korenmarkt Gent" >data/input
./openseekmap data/belgie.data <data/input >data/output
head -n 16 <data/output >data/output_first4results
# Korenmarkt komt 2x voor in de databank, en Gent ook. Dus we verwachten dat de eerste 4 resultaten Korenmarkt & Gent zullen matchen
if ! diff data/output_first4results ../../tests/TestSolutions/test4; then
    echo -e "$RED\c"
    echo "Failed test"
    echo -e "$DEFAULT_COLOUR\c"
    ((counter++))
fi

# Test 5, databank van belgie, we zoeken "Krugovaya Ulitsa Fornosovo" (plaats in Rusland)
echo "Krugovaya Ulitsa Fornosovo" >data/input
./openseekmap data/belgie.data <data/input >data/output
# Komt geen enkele keer voor in de databank
if ! diff data/output ../../tests/TestSolutions/test5; then
    echo -e "$RED\c"
    echo "Failed test"
    echo -e "$DEFAULT_COLOUR\c"
    ((counter++))
fi

# Test 6, databank van belgie, we zoeken "De Stere Genk" (2 typfouten)
echo "De Stere Genk" >data/input
./openseekmap data/belgie.data <data/input >data/output
head -n 4 <data/output >data/output_firstresult
# We verwachten dat het eerste resultaat nog steeds "De Sterre Gent" zal zijn
if ! diff data/output_firstresult ../../tests/TestSolutions/test6; then
    echo -e "$RED\c"
    echo "Failed test"
    echo -e "$DEFAULT_COLOUR\c"
    ((counter++))
fi

# Test 7, databank van belgie, we zoeken "De Sterre Gent" en "Korenmarkt Gent" (2 zoekopdrachten) (combinatie van test 3 en 5)
echo -e "De Sterre Gent\nKorenmarkt Gent" >data/input
./openseekmap data/belgie.data <data/input >data/output
head -n 8 <data/output >data/output1_first2results # We verwachten dat "De Sterre" in Gent op de 1ste en 2de plaats staat (want het zit 2x in de databank)
if ! diff data/output1_first2results ../../tests/TestSolutions/test3; then
    echo -e "$RED\c"
    echo "Failed test"
    echo -e "$DEFAULT_COLOUR\c"
    ((counter++))
fi
tail -n 21 <data/output >data/output2_results
head -n 16 <data/output2_results >data/output2_output_first4results
# Korenmarkt komt 2x voor in de databank, en Gent ook. Dus we verwachten dat de eerste 4 resultaten Korenmarkt & Gent zullen matchen
if ! diff data/output_first4results ../../tests/TestSolutions/test4; then
    echo -e "$RED\c"
    echo "Failed test"
    echo -e "$DEFAULT_COLOUR\c"
    ((counter++))
fi

# Resultaten bekijken van full tests
if [ "$counter" -eq "0" ]; then
    echo -e "$GREEN\c"
    echo "All full tests successfully executed"
    echo -e "$DEFAULT_COLOUR\c"
else
    echo -e "$RED\c"
    echo "$counter full tests failed"
    echo -e "$DEFAULT_COLOUR\c"
    clean_up
    exit 4
fi

clean_up

echo -e "\n$GREEN\c"
echo "***********************************"
echo -e "* All tests successfully executed *"
echo "***********************************"
echo -e "$DEFAULT_COLOUR\c"

exit 0
