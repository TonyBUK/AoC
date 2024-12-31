import time
import sys

def main() :

   kTowels   = set()
   kPatterns = set()

   with open("../input.txt", "r") as inputFile:
       kTokens   = inputFile.read().split("\n\n")
       kTowels   = set(kTokens[0].split(", "))
       kPatterns = set(kTokens[1].split("\n"))
   #end

   def possibleCombinations(kPattern : str, kTowels : set[str], nMaxLength : int, kSeen : dict[str, int], nPermutations = 0) -> int :

       nStartPermutations = nPermutations
       nMaxOffset         = min(nMaxLength, len(kPattern))

       # If the Pattern has been Seen Before... return the known delta
       if kPattern in kSeen :
           return nPermutations + kSeen[kPattern]
       #end

       # If the Pattern has been Exhausted, return the incremented permutations
       if 0 == len(kPattern) :
           return nPermutations + 1
       #end

       # Recurse through all possible sub-patterns from longest to shortest
       for nLength in range(nMaxOffset, 0, -1) :
           if kPattern[:nLength] in kTowels :
               nPermutations = possibleCombinations(kPattern=kPattern[nLength:], kTowels=kTowels, nMaxLength=nMaxLength, kSeen=kSeen, nPermutations=nPermutations)
           #end
       #end

       # Add this to the Seen List
       kSeen[kPattern] = nPermutations - nStartPermutations

       # Return the Known Permutation Count
       return nPermutations

   #end

   # As a Minor Optimisation, as we want to start with the longest patterns and work backwards...
   # what is the max length pattern?
   nMaxLength = max([len(kTowel) for kTowel in kTowels])


   # Both Parts :
   #
   # Part One : How many patterns had at least one combination?
   # Part Two : How many combinations were there?

   kSeen                 = {}
   nPossibleCount        = 0
   nPossibleCombinations = 0

   # Iterate through each of the Patterns counting how many *can* be made, and how many ways there were of making them
   for kPattern in kPatterns :
       nCombinationCount = possibleCombinations(kPattern=kPattern, kTowels=kTowels, nMaxLength=nMaxLength, kSeen=kSeen)
       if nCombinationCount > 0 :
           nPossibleCount        += 1
           nPossibleCombinations += nCombinationCount
       #end
   #end

   print(f"Part 1: {nPossibleCount}")
   print(f"Part 2: {nPossibleCombinations}")

#end

if __name__ == "__main__" :
   startTime      = time.perf_counter()
   main()
   print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end