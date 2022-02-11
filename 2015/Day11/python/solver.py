import time

def main() :

    # Read/Clean the Input File
    passwords = []
    with open("../input.txt", "r") as inputFile:
        passwords = [line.strip() for line in inputFile.readlines()]
    #end

    def solvePassword(startingPassword) :

        # Full Digits including illegal charactersx
        FULL_DIGITS    = "abcdefghijklmnopqrstuvwxyz"

        # Allowed Digits
        ALLOWED_DIGITS = "abcdefghjkmnpqrstuvwxyz"
        #                 00000000001111111111222
        #                 01234567890123456789012

        # The password itself is really just a base 23 number in that each
        # position has 23 possible values based upon the rules that it's
        # a lower case letter and excludes i, o and l.
        # "aaaa" would be 0,0,0,0
        # "abcd" would be 0,1,2,3
        # "wxyz" would be 19,20,21,22
        # So step 1 is to convert the string into an array of digits.
        password       = [ALLOWED_DIGITS.index(C) for C in startingPassword]
        passwordrange  = list(reversed(range(len(password)))) # Cache this so it doesn't get constantly re-assessed
        passwordBase   = len(ALLOWED_DIGITS)

        # Mapping Array to convert from Base 23 to Base 26 (Full Alphabet)
        BASE26_MAPPINGS = [FULL_DIGITS.index(C) for C in ALLOWED_DIGITS]

        def incrementPassword(password) :
            # Note: This could be a bit cleverer if we didn't just increment by 1 but instead
            #       calculated for each failed rule what the first valid value that meets the
            #       rule is.

            # Incrementing the password is now add and carry, i.e. the way
            # you probably used to add numbers as a child before getting a
            # calculator or memorising.
            #
            # i.e. 0,1,2,3 + 22 is:
            #
            # 3 + 22 = 25, but 25 > 23 so subtract 23 to leave 2, and carry the 1 (23/23 = 1)
            # leaving 0,1,2,2 + 1,0, giving a final answer of 0,1,3,2
            for x in passwordrange :
                password[x] += 1
                if password[x] < passwordBase : break
                else                          : password[x] = 0
            #end
        #end

        # Rule 1: Passwords must include one increasing straight of at least three letters,
        #         like abc, bcd, cde, and so on, up to xyz. They cannot skip letters; abd
        #         doesn't count.
        #
        #         But there's a big gotcha here... because we've stripped i, o and l, that
        #         means technically hjk now passes this rule, which doesn't seem right,
        #         so I've re-indexed the values back into base 26 first before doing the
        #         comparison, that way hjk would now no longer be consecutive.
        def rule1(password) :
            for a,b,c in zip(password[0:], password[1:], password[2:]) :
                a = BASE26_MAPPINGS[a]
                b = BASE26_MAPPINGS[b]
                c = BASE26_MAPPINGS[c]
                if (a == b-1) and (b == c-1) :
                    return True
                #end
            #end
            return False
        #end

        # Rule 3 : Passwords must contain at least two different, non-overlapping pairs of
        #          letters, like aa, bb, or zz.
        #
        # Note: Because of the "different" part, it's impossible to overlap, i.e. aa can't
        #       overlap bb.  If the different part wasn't a rule, then we'd need to consdier
        #       aaa and ensure it's not detected as two pairs, but since it is, we don't.
        def rule3(password) :
            uniquePairs = set()
            for a,b in zip(password[0:], password[1:]) :
                if a == b :
                    uniquePairs.add(a)
                    if len(uniquePairs) == 2:
                        return True
                    #end
                #end
            #end
            return False
        #end

        # Force a single increment to get things moving...
        incrementPassword(password)

        # The core principle here is to keep incrementing the password until
        # we pass a given rule, then move onto the next rule and repeat.  We've
        # found our password when all the rules passed without incrementing the
        # password.
        while True :

            anyIncrements = False

            # Validate the Current Password

            # Rule 1: See above
            while False == rule1(password) :
                incrementPassword(password)
                anyIncrements = True
            #end

            # Rule 2: Passwords may not contain the letters i, o, or l, as these letters can be
            #         mistaken for other characters and are therefore confusing.

            # This is impossible to achieve because of how we've already stripped out these letters
            # when moving to Base 23, meaning we always pass this rule

            # Rule 3 : See above
            while False == rule3(password) :
                incrementPassword(password)
                anyIncrements = True
            #end

            if False == anyIncrements : break

        #end

        # Return the Found Password as a String
        return  "".join([ALLOWED_DIGITS[x] for x in password])

    #end

    # Process each Password
    for startingPassword in passwords :
        solvedPassword = solvePassword(startingPassword)
        print(f"Part 1: {solvedPassword}")
        print(f"Part 2: {solvePassword(solvedPassword)}")
    #end

#end

if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s")
#end
