import time
import sys

def main() :

    VALUE  = 0
    GIVE   = 1

    def decodeCommand(kTokens) :

        assert(len(kTokens) >= 1)

        kCommand = {}

        if "value" == kTokens[0] :
            assert(len(kTokens) == 6)
            kCommand["command"] = VALUE
            kCommand["params"]  = {"value" : int(kTokens[1]),
                                   "bot"   : int(kTokens[5])}
        elif "bot" == kTokens[0] :
            assert(len(kTokens) == 12)
            kCommand["command"] = GIVE
            kCommand["params"]  = {"bot"    : int(kTokens[1]),
                                   "low"    : {"type" : kTokens[5],
                                               "id"   : int(kTokens[6])},
                                   "high"   : {"type" : kTokens[10],
                                               "id"   : int(kTokens[11])}}
        else :
            assert(False)
        #end

        return kCommand

    #end

    # Read and Compressed Text
    kCommands = []
    with open("../input.txt", "r") as inputFile:
        kCommands = [decodeCommand(line.strip().split(" ")) for line in inputFile.readlines()]
    #end

    # Create the Bots / Output Bins in Advance
    kBots    = {}
    kOutputs = {}
    kQueue   = []

    def createBot(kBots, nBotId) :
        if not nBotId in kBots :
            kBots[nBotId]            = {}
            kBots[nBotId]["chips"]   = []
            kBots[nBotId]["command"] = {}
        #end
    #end

    def fetchOrCreateBot(kBots, nBotId) :
        createBot(kBots, nBotId)
        return kBots[nBotId]
    #end

    def createOutput(kOutputs, nOutputId) :
        if not nOutputId in kOutputs :
            kOutputs[nOutputId]      = None
        #end
    #end

    def allocateCommand(kBots, kOutputs, kCommand) :

        if "bot" == kCommand["type"] :
            createBot(kBots, kCommand["id"])
        elif "output" == kCommand["type"] :
            createOutput(kOutputs, kCommand["id"])
        #end

        return kCommand

    #end

    # 1. Create and Allocate all the Bots/Output Bins
    # 2. Initialise the Chips stored in each Bot
    # 3. Add any Bots holding two chips to the Queue
    for kCommand in kCommands :
        
        if VALUE == kCommand["command"] :

            kBot = fetchOrCreateBot(kBots, kCommand["params"]["bot"])
            assert(len(kBot["chips"]) < 2)
            kBot["chips"].append(kCommand["params"]["value"])
            kBot["chips"].sort()

            if 2 == len(kBot["chips"]) :
                kQueue.append([kCommand["params"]["bot"], kBot])
            #end

        else :

            kBotSource = fetchOrCreateBot(kBots, kCommand["params"]["bot"])
            assert(len(kBotSource["command"]) == 0)
            kBotSource["command"]["low"]  = allocateCommand(kBots, kOutputs, kCommand["params"]["low"])
            kBotSource["command"]["high"] = allocateCommand(kBots, kOutputs, kCommand["params"]["high"])

        #end

    #end

    def processCommand(kBots, kOutputs, kQueue, kCommand, kChips, nIndex) :

        # IF the Target is a Bot
        if "bot" == kCommand["type"] :

            # Move the Chip to the Target Bot

            kTarget = kBots[kCommand["id"]]
            assert(len(kTarget["chips"]) < 2)

            kTarget["chips"].append(kChips[nIndex])
            kTarget["chips"].sort()

            # Determine if Target Bot has enough chips to need adding to the queue

            if 2 == len(kTarget["chips"]) :
                kQueue.append([kCommand["id"], kTarget])
            #end

        # Otherwise... Output Bin

        elif "output" == kCommand["type"] :

            # Move the Chip to the Target Output Bin

            assert(kOutputs[kCommand["id"]] == None)
            kOutputs[kCommand["id"]] = kChips[nIndex]

        else :

            assert(False)

        #end

        # Remove the Moved Chip from the Current Bot

        kChips.pop(nIndex)

    #end

    kChipComparison = [17,61]
    nPart1BotId     = None

    # Keep going until there are no Bots that can give any chips
    while len(kQueue) > 0 :

        # Process the Current Bot in the Queue
        # Note: Should be pop(0) to get the oldest, but the order itself is unimportant, and pop() is quicker
        #       as it doesn't have to shift any data around.
        nBotId, kBot = kQueue.pop()

        # Test for the Part 1 Solution
        if kBot["chips"] == kChipComparison :
            assert(None == nPart1BotId)
            nPart1BotId = nBotId
        #end

        # Process the High/Low Commands for each Bot
        # Note: This function will populate more entries in the Queue if applicable
        # Note: This is processed High/Low in that order to make pop'ing more efficient in processCommand
        #       for removing the Chip.
        processCommand(kBots, kOutputs, kQueue, kBot["command"]["high"], kBot["chips"], 1)
        processCommand(kBots, kOutputs, kQueue, kBot["command"]["low"],  kBot["chips"], 0)

    #end

    print(f"Part 1: {nPart1BotId}")
    print(f"Part 2: {kOutputs[0]*kOutputs[1]*kOutputs[2]}")

#end

if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)*1000}ms", file=sys.stderr)
#end