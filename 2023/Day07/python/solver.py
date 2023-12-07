import time
import sys
 
def main() :

    def rankHands(kHands, bAllowJoker = False) :

        kRankedHands = []

        # This converts a hand into a number of base 13, ie. the number of cards in a suit
        def calculateStrength(kHand, bAllowJoker) :

            CARDS     = "J23456789TQKA" if bAllowJoker else "23456789TJQKA"
            BASE      = len(CARDS)
            nStrength = 0

            for kCard in kHand :
                nStrength += (nStrength * BASE) + CARDS.index(kCard)
            #end

            return nStrength

        #end

        # We keep separate lists for each hand type, and then sort them by strength
        kHigh         = []
        kOnePair      = []
        kTwoPair      = []
        kThreeOfAKind = []
        kFullHouse    = []
        kFourOfAKind  = []
        kFiveOfAKind  = []

        # Create the Unsorted Hand Lists
        for kHand in kHands :

            # Part Two introduces the Joker, so we may need to distinguish
            # between the hand played and the hand used to calculate strength
            kPlayedHand = kHand[0]

            if bAllowJoker :

                if "J" in kHand[0] :

                    # Given Strength is based on the original hand, and we don't worry about
                    # arbitrating wins any other way, it should just be a matter of replacing the
                    # Joker with the card of the histest frequency
                    kUniqueCardsForPermutations = {kHand[0].count(k) : k for k in set(list(kHand[0])) if k != "J"}
                    if len(kUniqueCardsForPermutations) > 0 :
                        kPlayedHand = (kHand[0].replace("J", kUniqueCardsForPermutations[max(kUniqueCardsForPermutations.keys())]))
                    #end
                #end
            #end

            # Calculate the Hand Strength based on the unaltered hand
            nStrength = calculateStrength(kHand[0], bAllowJoker)

            # Determine the Hand Type
            kUniqueCards = {k : kPlayedHand.count(k) for k in set(list(kPlayedHand))}
            nUniqueCards = len(kUniqueCards)

            # If all 5 Cards are unique, by definition, we must be a high card
            if nUniqueCards == 5 :

                kHigh.append([nStrength, kHand[1]])

            # If there are 4 unique cards, then we must have a pair
            elif nUniqueCards == 4 :

                kOnePair.append([nStrength, kHand[1]])

            # If there are 1 unique cards, then we must have a five of a kind
            elif nUniqueCards == 1 :
                    
                kFiveOfAKind.append([nStrength, kHand[1]])

            # Either 4 of a Kind or Full House
            elif nUniqueCards == 2 :
                    
                nHighestCardCount = max(kPlayedHand.count(k) for k in kUniqueCards)

                if nHighestCardCount == 4 :
                        
                    kFourOfAKind.append([nStrength, kHand[1]])

                else :

                    assert(3 in kUniqueCards.values() and 2 in kUniqueCards.values())

                    kFullHouse.append([nStrength, kHand[1]])
                
                #end

            # Either 3 of a Kind or 2 Pair
            else :

                nHighestCardCount = max(kUniqueCards.values())

                if nHighestCardCount == 3 :
                        
                    kThreeOfAKind.append([nStrength, kHand[1]])
                
                elif nHighestCardCount == 2 :   

                    kTwoPair.append([nStrength, kHand[1]])

                else :

                    assert(False)

                #end

            #end

        #end

        # Create the Ranked List
        kRankedHands = (sorted(kHigh) +
                        sorted(kOnePair) +
                        sorted(kTwoPair) +
                        sorted(kThreeOfAKind) +
                        sorted(kFullHouse) +
                        sorted(kFourOfAKind) +
                        sorted(kFiveOfAKind))

        return kRankedHands

    #end

    kHands = []
    with open("../input.txt", "r") as inputFile:

        # Create a list of Hands/Bets
        kHands = [(k, int(v)) for line in inputFile.readlines() for k, v in [line.split()]]

        print(f"Part 1 : {sum([(i+1) * k[1] for i,k in enumerate(rankHands(kHands, False))])}")
        print(f"Part 2 : {sum([(i+1) * k[1] for i,k in enumerate(rankHands(kHands, True))])}")

#end
 
if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end