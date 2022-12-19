import time
import sys

def main() :

    with open("../input.txt", "r") as inputFile:

        def decodeBlueprint(kLine) :
            kTokens = kLine.split(" Each ")
            assert(len(kTokens) == 5)
            nID       = int(kTokens[0].replace(":", "").split(" ")[-1])
            kOre      = int(kTokens[1].replace(".", "").split(" ")[-2])
            kClay     = int(kTokens[2].replace(".", "").split(" ")[-2])
            kObsidian = [int(kToken) for i,kToken in enumerate(kTokens[3].split(" ")) if i in [3,6]]
            kGeode    = [int(kToken) for i,kToken in enumerate(kTokens[4].split(" ")) if i in [3,6]]
            return nID, {"ore" : kOre, "clay" : kClay, "obsidian" : kObsidian, "geode" : kGeode}
        #end

        def getTriangleNumber(x) :
            return (x*(x+1)) // 2
        #end

        def calculateGeodes(kBlueprint, nMaxTime=24) :

            ORE_BOT_COST      = kBlueprint["ore"]
            CLAY_BOT_COST     = kBlueprint["clay"]
            OBSIDIAN_BOT_COST = kBlueprint["obsidian"]
            GEODE_BOT_COST    = kBlueprint["geode"]
            MAX_ORE_COST      = max(ORE_BOT_COST, CLAY_BOT_COST, OBSIDIAN_BOT_COST[0], GEODE_BOT_COST[0])

            # Start the Queue with 1 bot and no minutes on the clock.
            kQueue     = [[0,0,0,0,0,0,0,0,1]]
            kSeen      = {}
            nMaxGeodes = 0

            # Process the Queue
            while len(kQueue) > 0 :

                kEntry         = kQueue.pop()
                nGeodeCount    = kEntry[0]
                nGeodeBots     = kEntry[1]
                nTime          = kEntry[2]
                nObsidianCount = kEntry[3]
                nObsidianBots  = kEntry[4]
                nClayCount     = kEntry[5]
                nClayBots      = kEntry[6]
                nOreCount      = kEntry[7]
                nOreBots       = kEntry[8]

                # Determine if this is the best outcome
                if nGeodeCount > nMaxGeodes :
                    nMaxGeodes = nGeodeCount
                #end

                # Do nothing else if we've used all of our allotted time.
                if nTime == nMaxTime :
                    continue
                #end

                # Tries to crudely look ahead assuming that if we built a new bot each cycle,
                # could we ever beat the best geode count found so-far.
                if nMaxGeodes > 0 :
                    nBestGeodes  = nGeodeCount + ((nMaxTime - nTime) * nGeodeBots)
                    nBestGeodes += getTriangleNumber(nMaxTime - nTime)
                    if nBestGeodes < nMaxGeodes :
                        continue
                    #end
                #end

                # Cull Seen/Worse States
                kSeenKey       = tuple([nGeodeCount,
                                        nGeodeBots,
                                        nObsidianCount,
                                        nObsidianBots,
                                        nClayCount,
                                        nClayBots,
                                        nOreCount,
                                        nOreBots])
                if kSeenKey not in kSeen :
                    kSeen[kSeenKey] = nTime
                elif nTime < kSeen[kSeenKey] :
                    kSeen[kSeenKey] = nTime
                else :
                    continue
                #end

                # The only reason to build this bot is to get more geodes, and the earliest
                # it'll have an impact is in 2 minutes time.
                # Minute 1: Time required to Build Geode Bot
                # Minute 2: Collect extra Geode
                if (nMaxTime - nTime) >= 2 :
                    # If we can build a Geode Bot this time, do so, it will never not be the best move,
                    # so skip all others.
                    if (nOreCount >= GEODE_BOT_COST[0]) and (nObsidianCount >= GEODE_BOT_COST[1]) :
                        kQueue.append([nGeodeCount    + nGeodeBots,
                                       nGeodeBots     + 1,
                                       nTime          + 1,
                                       nObsidianCount + nObsidianBots - GEODE_BOT_COST[1],
                                       nObsidianBots,
                                       nClayCount     + nClayBots,
                                       nClayBots,
                                       nOreCount      + nOreBots  - GEODE_BOT_COST[0],
                                       nOreBots])
                        continue
                    #end
                #end

                # The only reason to build this bot is to get more geodes, and the earliest
                # it'll have an impact is in 4 minutes time.
                # Minute 1: Time required to Build Obsidian Bot
                # Minute 2: Collect extra Obsidian
                # Minute 3: Time required to Build Geode Bot
                # Minute 4: Collect extra Geode
                if (nMaxTime - nTime) >= 4 :
                    if (nOreCount >= OBSIDIAN_BOT_COST[0]) and (nClayCount >= OBSIDIAN_BOT_COST[1]) :
                        # There's no point building more bots than the Geode Bot costs (Obsidian)

                        # If we can build an Obsidian Bot at this time, it appears to be the best move,
                        # so skip all others.
                        # Note 100% sure of this, but seems to work
                        if nObsidianBots < GEODE_BOT_COST[1] :
                            kQueue.append([nGeodeCount    + nGeodeBots,
                                           nGeodeBots,
                                           nTime          + 1,
                                           nObsidianCount + nObsidianBots,
                                           nObsidianBots  + 1,
                                           nClayCount     + nClayBots - OBSIDIAN_BOT_COST[1],
                                           nClayBots,
                                           nOreCount      + nOreBots  - OBSIDIAN_BOT_COST[0],
                                           nOreBots])
                            continue
                        #end
                    #end
                #end

                # The only reason to build this bot is to get more geodes, and the earliest
                # it'll have an impact is in 6 minutes time.
                # Minute 1: Time required to Build Clay Bot
                # Minute 2: Collect extra Clay
                # Minute 3: Time required to Build Obsidian Bot
                # Minute 4: Collect extra Obsidian
                # Minute 5: Time required to Build Geode Bot
                # Minute 6: Collect extra Geode
                bClay = False
                if (nMaxTime - nTime) >= 6 :
                    if nOreCount >= CLAY_BOT_COST :
                        # There's no point building more bots than the Obsiadian Bot costs (Clay)
                        if nClayBots < OBSIDIAN_BOT_COST[1] :
                            kQueue.append([nGeodeCount    + nGeodeBots,
                                           nGeodeBots,
                                           nTime          + 1,
                                           nObsidianCount + nObsidianBots,
                                           nObsidianBots,
                                           nClayCount     + nClayBots,
                                           nClayBots      + 1,
                                           nOreCount      + nOreBots - CLAY_BOT_COST,
                                           nOreBots])
                        #end
                        bClay = True
                    #end
                #end

                # The only reason to build this bot is to get more geodes, and the earliest
                # it'll have an impact is in 8 minutes time.
                # Minute 1: Time required to Build Ore Bot
                # Minute 2: Collect extra Ore
                # Minute 3: Time required to Build Clay Bot
                # Minute 4: Collect extra Clay
                # Minute 5: Time required to Build Obsidian Bot
                # Minute 6: Collect extra Obsidian
                # Minute 7: Time required to Build Geode Bot
                # Minute 8: Collect extra Geode
                bOre = False
                if (nMaxTime - nTime) >= 8 :
                    if nOreCount >= ORE_BOT_COST :
                        # There's no point building more bots than the Max of:
                        # - Ore Bot Cost (Ore)
                        # - Clay Bot Cost (Ore)
                        # - Obsdian Bot Cost (Ore)
                        # - Geode Bot Cost (Ore)
                        if nOreBots < MAX_ORE_COST :
                            kQueue.append([nGeodeCount    + nGeodeBots,
                                        nGeodeBots,
                                        nTime          + 1,
                                        nObsidianCount + nObsidianBots,
                                        nObsidianBots,
                                        nClayCount     + nClayBots,
                                        nClayBots,
                                        nOreCount      + nOreBots  - ORE_BOT_COST,
                                        nOreBots       + 1])
                        #end
                        bOre = True
                    #end
                #end

                # Only do nothing if we didn't build either an ore or a clay bot
                if False == bClay or False == bOre :
                    kQueue.append([nGeodeCount    + nGeodeBots,
                                   nGeodeBots,
                                   nTime          + 1,
                                   nObsidianCount + nObsidianBots,
                                   nObsidianBots,
                                   nClayCount     + nClayBots,
                                   nClayBots,
                                   nOreCount      + nOreBots,
                                   nOreBots])
                #end

            #end

            return nMaxGeodes

        #end

        # Input Data
        kBluePrints = {k:v for k,v in [decodeBlueprint(kLine.strip()) for kLine in inputFile.readlines()]}

        # Part 1 (24 Minutes all Blueprints)
        kMaxGeodes  = [k*calculateGeodes(v) for k,v in kBluePrints.items()]
        print(f"Part 1: {sum(kMaxGeodes)}")

        # Part 2 (32 Minutes, first 3 Blueprints Only)
        kBluePrintsOfInterest = [kBluePrints[1], kBluePrints[2], kBluePrints[3]]
        kMaxGeodes = [calculateGeodes(v, 32) for v in kBluePrintsOfInterest]
        kProduct   = 1
        for kMaxGeode in kMaxGeodes : kProduct *= kMaxGeode
        print(f"Part 2: {kProduct}")

    #end

#end

if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end
