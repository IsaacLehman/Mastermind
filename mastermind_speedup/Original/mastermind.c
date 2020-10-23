
/** Isaac Lehman ~ COMP325  ~  Fall 2020
 * Credit: Code modified from Dr. Valentines' original
 * ******************************************************************
 * MasterMind is a game published by Pressman Toy Corp (pressmantoy.com)
 * It consists of pegs of six colors from which a CodeMaker builds a
 * 4-peg secretCode.  The CodeBreaker attempts to crack that code by 
 * posing a series of guessCodes.  Each guess is scored 1 Black for the
 * right color in the right location, and 1 White for matching colors
 * but different locations.
 *
 * We want to test a "Baby Knuth" algorithm against all possible codes.
 * 
 * This program will run all MasterMind Code permutations through a 
 * "Baby Knuth" algorithm.  That is, we have a computer-as-code-
 * breaker function (playGame) that recieves a Code and returns how
 * many guesses it took to crack it. Main will send all possible Codes to
 * that function, saving how many guesses were needed to break each
 * code.  We report (a) the worst case, (b) average number of guesses,
 * and (c) std Deviation of guesses needed.
 * 
 * We use unsigned chars as pegs: 256 possible 'colors' in 8 bits.  
 * The legal colors are [1,NUM_COLORS].
 * 
 * See Knuth, D. "The Computer as Master Mind", Journal of 
 *      Recreational Mathematics, Vol 9(1), 1976-7.
 * 
 * COMPILE & EXECUTE
 * gcc -Og ${file} -o ${fileDirname}/${fileBasenameNoExtension} -lm
 * ex. gcc -Og mastermind.c -o mm_exe -lm
 * time ./mm_exe    To run a timed execution
 * 
 * *****************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

//Our Global 'Constants'
#define MAX_PEGS 6              //number of pegs in a Code
#define NUM_COLORS 6            //number of colors for pegs
#define MAX_GUESSES 20          //worst case guess cnt to crack

//#define DEBUG     turn verbose reporting on/off

/* Declare our Code and CodePtr types
**  unsigned char gives 256 color values ~ should be big enough
**  array 'theCode' is the set of pegs 
**  Legal color values are [1,NUM_COLORS]
**  fields 'numWhite' & 'numBlack' are the SCORE this code made 
**      against some other code
**  Code references such a struct; CodePtr is a pointer to a struct
********************************************************************/
typedef struct CodeStruct {
    unsigned char theCode[ MAX_PEGS ];  //the colors
    unsigned char numWhite, numBlack;   //this code's score against other code
}Code, *CodePtr;

//Function Prototypes
void initCode (CodePtr newCode, unsigned char* ary);
void generateNextCode(unsigned char * origCode);
void scoreCodes( Code secretCode,  CodePtr guessCode);
char getCodeColor(unsigned char colorVal);
char* toString(Code aCode);
int notMaxCode(Code c);
int playGame(Code secretCode);



/********************************************************************
 ********************************************************************
 ************************************** M A I N *********************
 *******************************************************************/
int main(void) {
    //Var Dictionary for Main
    Code secretCode;                //our secret code
    unsigned char pegAry[MAX_PEGS]; //a local array for just the "pegs" 
    int maxGames = round(exp(MAX_PEGS * log(NUM_COLORS))); //#permutations
    unsigned char *guessAry = malloc(maxGames); //save #guesses for each code
    int guessCounts[MAX_GUESSES]; //show the distribution of guesses/code
    int gameNum = 0;    //how many games have we played so far?

    //our game stats
    int numGuesses;         //how many guesses did computer make on one code
    int maxGuesses = 0;     //worst case of num guesses needed
    Code maxGuessCode;      //a code that caused the worst case
    int sumGuesses=0;       //used for avg guesses
    double avgGuess;        //average number of guesses to solve all permutations
    double stdDeviation;    //std Deviation of guesses
    int i;                  //loop control var

    /*****************************************************/
    /***** 1.0 INITIALIZATION ****************************/
    //REQUIRED headers
    printf("Isaac Lehman ~ COMP325 ~ Initial Mastermind Game\n");
    printf("  This program plays a 'baby Knuth' algorithm against ");
    printf("all possible Codes in a %d-color, %d-peg MasterMind Game.\n",
            NUM_COLORS, MAX_PEGS);
    printf("  It will report the worst case, avg case & std deviation\n\n");

    //init our guess distribution array
    //This is an "extra" feature the professor uses to error check results
    for (i=0; i<MAX_GUESSES; i++) guessCounts[i]=0;

    //Initialize our code to the first code: all NUM_COLORS 
    for (i=0; i<MAX_PEGS; i++) pegAry[i]=NUM_COLORS;    //set all to NUM_COLORS

    //"priming the pump" on our loop: 
    //      start @6666 then genNextCode "rolls it over" to 1111

    initCode(&secretCode, pegAry);  //plug array into Code struct

    /*****************************************************/
    /***** 2.0 Crack all possible Codes ******************/
    do {
        generateNextCode(secretCode.theCode);  //generate our next code

        numGuesses = playGame(secretCode);  //how many guesses to crack?

        if (numGuesses > maxGuesses) {    //get worst case of guesses
            maxGuesses = numGuesses;
            maxGuessCode = secretCode;  //save that code, too
        }

        sumGuesses += numGuesses;       //add 'em all up for AVG
        guessAry[gameNum] = numGuesses; //save each guess for stdDev
        gameNum++;                      //bump counter- just a check
    }while (notMaxCode(secretCode));    //stop when each peg is max value


    /*****************************************************/
    /***** 3.0 Calculate Stats & Display Results *********/
    if (gameNum != maxGames) //be sure we ran ALL the codes through
        printf("\n *** We ran %d games, but we expected to have %d games ***", gameNum, maxGames);

    avgGuess = sumGuesses / (double)maxGames;
    stdDeviation = 0.0;

    //Loop through all returned guess counts: sum for stdDev & 
    //count for our "guess distribution" results
    for (i=0; i<maxGames; i++) { //sum up all diff's squared
        stdDeviation += (avgGuess-guessAry[i])*(avgGuess-guessAry[i]);
        guessCounts[guessAry[i]]++;    //count the 2-guess, 3-guess, etc
    }
    stdDeviation = sqrt(stdDeviation / (maxGames-1.0)); //1.0 avoids int div

    printf("\n\nTournament Results\n");
    printf("This game had %d pegs of %d colors (%d permutations)\n", 
                MAX_PEGS, NUM_COLORS, maxGames);
    printf("\tMax Guesses: %d  (%s for example) \n", 
                    maxGuesses, toString(maxGuessCode));
    printf("\tAvg guesses used: %.3f\n", avgGuess);
    printf("\tThe standard deviation is %.3f\n", stdDeviation);

    //Show the distribution of guesses
    //NB: this is NOT part of the normal assignment: I use it for error ck
    printf("\nDistribution of Guesses over all Codes\n");

    i=MAX_GUESSES-1;    //index to LAST elem in guessCounts array
    while ((i>0) && (guessCounts[i] == 0)) 
        i--;    //find first non-zero from high end of guess counts

    //now show our distribution of guesses
    gameNum = 0;    //count total number of games run/codes cracked
    for (; i>=0; i--) {//already init'd i 
        printf("# of %d guesses: %d\n", i, guessCounts[i]);
        gameNum += guessCounts[i];  //tally up all codes cracked
    }
    printf("\nWe cracked %d codes total\n\n", gameNum);

    /*****************************************************/
    /***** 4.0 Finish Up *********************************/
    free(guessAry); //garbage collection

    printf("\n\n\t<< * normal termination * >> \n\n\n");
    return 0;
}//main

/** initCode
 * Receives a pointer to a Code, and an array of values
 * It will copy those values into the "theCode" array of Code.  The
 * User is responsible to ensure the ary is the right size.
 * It will also set the Black/White scores to zero.
 * *****************************************************************/
void initCode (CodePtr newCode, unsigned char* ary) {
    newCode->numWhite = newCode->numBlack = 0;  //clear score counters
    memcpy(newCode->theCode, ary, MAX_PEGS);    //copy the pegs
}

/** generateNextCode
 * Receives an array representing the pegs array of a Code
 * Given a current code, we will change it to be the next sequntial code
 * Think of the old car odometers: one column rolls over into the next
 *      as it reaches 9 and rolls over to 0.  We use the same approach.
 * Like that odometer, a code of all NUM_COLORS will roll over to  a
 * code of all 1's.  eg 4 peg/6 color:  6666 -> 1111  -or- 1266 -> 1311
 * That is, each peg cycles from 1..6 and back to 1.
 *******************************************************************/
void generateNextCode(unsigned char * origCode) {
    int index = MAX_PEGS - 1;    //right most peg of origCode
    while (index >= 0) {
        if (origCode[index] < NUM_COLORS){ //need update ONLY this column
            origCode[index]++;  //move to next color
            index = -1;         //stop while loop - we're done
        } else {//we must rollover into next column
            origCode[index] = 1;  //move this column back to first value
            index--;            //and incr next column (carry in -ish)
        }
    }
}//generateNextCode


/*ScoreCodes
**  This is HEART of our game: score 2 codes against one another
**  Receives a VALUE secretCode and a REFERENCE guessCode
**  Returns guessCode with updated scores in numBlack & numWhite
**
**  Marks off pegs as they as used for scoring (no dbl dipping)
**      secretCode pegs changed to BOGUS_SECRET
**      guessCode pegs changed to BOGUS_GUESS (marring the COPY in tempCode)
********************************************************************/
void scoreCodes( Code secretCode,  CodePtr guessCode) {
    const unsigned char BOGUS_SECRET = 0;     //'BOGUS' values to mark off pegs used
    const unsigned char BOGUS_GUESS = NUM_COLORS+1;   // already in score (no dbl-dip)
    Code tempCode = (*guessCode);    //make a local copy of guess
    int i,j;                        //loop control var's

    guessCode->numWhite = guessCode->numBlack = 0;  //clear scores

    //Step 1: score Blacks
    for (i=0; i<MAX_PEGS; i++) {
        if (secretCode.theCode[i] == tempCode.theCode[i]) {//right color/right col
            guessCode->numBlack+=1;             //count one black score
            tempCode.theCode[i] = BOGUS_GUESS;   //mark pegs so we 
            secretCode.theCode[i] = BOGUS_SECRET; // don't double dip
        } 
    }//score blacks

    //Phase 2: score Whites
    for (i=0; i<MAX_PEGS; i++) {
        for (j=0; j<MAX_PEGS; j++) {
            //test for a match
            //if i==j we cannot have match ('black' loop would catch it)
            if (secretCode.theCode[i] == tempCode.theCode[j]) {
                guessCode->numWhite+=1;     //score one white
                tempCode.theCode[j] = BOGUS_GUESS;  //prevent double dip
                secretCode.theCode[i] = BOGUS_SECRET;
                break; //done with THIS i&j combo (no double dip)
            }
        }//for j
    }//for i

}//scoreCodes


/* getCodeColor
**  receives a value in [1,NUM_COLORS]
**  returns a letter for the '6 color' represented:
**      Red, Green, Blue, Yellow, White, blacK for 6 colors
**
**  User must update the colorList for more than 6 colors
********************************************************************/
char getCodeColor(unsigned char colorVal) {
    char val;

    //The color "letters": x & z are bogus (should never occur)
    static char colorList[] = {'x', 'R', 'G', 'B', 'Y', 'W', 'K', 'z'};
    
    if ( (1<=colorVal) && ( colorVal <=NUM_COLORS))  //range ck
        val = colorList[colorVal];
    else {  //User sent a bad color number: outside [1,NUM_COLORS]
        printf("\nError in getCodeColor: %d is a bad color number - using X\n", 
            colorVal);
        val='X';
    }

    return val;
}//getCodeColor


/* toString
**  Receives a Code struct
**  Returns a char string with letters representing the peg colors
**  
**  WARNING: the returned pointer is to a STATIC array here: so you
**      will have only one "string" live at a time.  Make a copy if
**      you need two Code-strings or more at the same time.
********************************************************************/
char* toString(Code aCode) {
    //STATIC so the mem space is preserved from call to call
    static char theCodeString[ MAX_PEGS + 1 ]; //extra char for '\0'
    int i;  //loop control

    for (i=0; i<MAX_PEGS; i++)  //process each peg
        theCodeString[i] = getCodeColor(aCode.theCode[i]);

    theCodeString[MAX_PEGS] = '\0';    //null terminator for cStr
    return theCodeString;
}//toString


/** notMaxCode: recieves a code and tests for MAX value
 * We need a test for the generateNextCode function.
 * This test asks "is the odometer at its MAX capacity?"
 * Or is each peg at the max (NUM_COLORS) value?
 * Returns an int (treated as a boolean)
 * *****************************************************************/
int notMaxCode(Code c){
    int isNotAllMax = 0; //assume FALSE (is all max values)
    int i=0;            //loop control

    //test each peg against max color value
    while ((i<MAX_PEGS) && !isNotAllMax) {
        if (c.theCode[i] != NUM_COLORS)
            isNotAllMax = 1;   //set true
        else
            i++;
    }

    return isNotAllMax;
}


/* playGame: the Computer as Code Breaker
** Receives a secretCode  
** Returns numGuesses used to crack it
**
** Cracks that code using a "baby Knuth" algorithm
** That is, make a '1122' form guess
** Keep all codes that score the same against that guess as
**     that guess scored against the secretCode.
**
** Then repeat until you get a score of all Blacks:
** (a) select 1st candidate from pool as new guess
** (b) score guess against secret
** (c) keep all candidates that score the same against guess
**********************************************************/
int playGame(Code secretCode) {
    Code guessCode;     //the guess we are making
    Code trialCode;     //internal 'guess' we use to pool candidates
    int maxCodes = 
        round(exp (MAX_PEGS*log(NUM_COLORS)));   //total permutations of Codes
    Code *codeList = 
        malloc(maxCodes*sizeof(Code)); //our candidate pool of Codes
    int codesRemaining = 0;     //size of our candidate pool
    unsigned char pegAry[MAX_PEGS]; //the low level 'pegs' array that is part of Code
    int numBlack, numWhite; //the score
    int codeCount;          //num codes retained in our search (after culling out Codes)
    int guessCnt = 1;       //guesses used to crack code: must have at least one
    int i;                  //loop control

    //Phase 1: use a hard-coded first guess (Knuth's 1122)
    // or for 6 pegs use 112233
    //score this against the secretCode 
    if(MAX_PEGS == 4) {
        pegAry[0] = pegAry[1] = 1;
        pegAry[2] = pegAry[3] = 2;  //1122 from Knuth for 4peg/6color
    }
    else if(MAX_PEGS == 6) {
        pegAry[0] = pegAry[1] = pegAry[2] = 1;
        pegAry[3] = pegAry[4] = 2; 
        pegAry[5] = 3; //112233 from Knuth for 6peg/6color
    }
    
    initCode(&guessCode, pegAry);       //make our first guess
    scoreCodes(secretCode, &guessCode); //score guess against secret
    numBlack = guessCode.numBlack;
    numWhite = guessCode.numWhite;  //save the scores

    //Now score 1122 against ALL permutations of codes
    //inital code is all NUM_COLORS (e.g. 6666)
    for (i=0; i<MAX_PEGS; i++)
        pegAry[i] = NUM_COLORS;  
    initCode(&trialCode, pegAry);       //make our trial code

    //Priming the pump: start @6666 (for example) then the 
    //  genNextCode function roles it over to 1111
    //  This ensures we do all permutations in our loop

    //generate all permutations of mm codes
    do {
        generateNextCode(trialCode.theCode);    //gen Next Code        
        scoreCodes(guessCode, &trialCode);  //score our trial against guess    
        if ((numBlack == trialCode.numBlack) && 
                (numWhite == trialCode.numWhite)){  //we have a keeper
            codeList[codesRemaining] = trialCode;   //this is a keeper
            codesRemaining++;                       //bump counter
        }

    } while (notMaxCode(trialCode)); //stop @ 6666 for example

#ifdef DEBUG
    printf("Cracking code %s : ", toString(secretCode));
#endif

    /*  Phase 2: So we have our initial pool of candidates.  
    Select 1st candidate & score it against the secretCode
    Then score it against all remaining candidates. Keep
    ONLY those codes yielding that same score */
    while (numBlack != MAX_PEGS) {//haven't guessed it yet
 
        #ifdef DEBUG
        printf("%5d", codesRemaining);
        #endif

        guessCode = codeList[0];    //guess the 1st remaining code
        scoreCodes(secretCode, &guessCode); //score it
        guessCnt++;     //bump guess counter

        numBlack = guessCode.numBlack;  //save our target scores
        numWhite = guessCode.numWhite;

        codeCount = 0;  //count codes we retain from codeList

        //Now process the new guess through the remaining candidates
        //We use just one array, and simply copy the keepers into
        //  new (lower) locations in codeList.

        // codesRemaining: number of candidates we have now
        // codeCount: number of those candidates we are "keeping"
        for (i=0; i<codesRemaining; i++) {//test each code
            trialCode = codeList[i];
            scoreCodes(guessCode, &trialCode);
            if ((numBlack == trialCode.numBlack) && (numWhite == trialCode.numWhite)){//keeper
                codeList[codeCount] = trialCode;    //move to top of codeList
                codeCount++;    //#of keepers so far
            }
        }//for all remaining codes

        //A simple error check: no candidates remaining
        if (codeCount <= 0){
            printf("We have a problem: no candidates remaining for %s\n\n", toString(secretCode));
            guessCnt = -99999;  //bogus value: we blew up!
            break;  //exit our whileLoop
        }
        codesRemaining = codeCount; //we have a smaller set of candidates for next pass
    }//while not solved

    #ifdef DEBUG
    printf("\n");
    #endif

    free(codeList); //handle dynamic memory

    return guessCnt;    //tell user the number of guesses we used
}//playGame: Computer as Code Breaker