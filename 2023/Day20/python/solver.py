import time
import sys
import dataclasses
import enum
import math
import copy
 
class EModuleType(enum.IntEnum) :
    BROADCASTER = 0
    FLIPFLOP    = 1
    CONJUNCTION = 2
#end
 
@dataclasses.dataclass
class ModuleDataData :
    eModuleType        : EModuleType
    kBroadcastModules  : list
    kConjunctionStates : dict
    bState             : False
#end
 
TYPES = {
    "%" : EModuleType.FLIPFLOP,
    "&" : EModuleType.CONJUNCTION
}
 
def gcd(a, b) :
    # Euclidean algorithm
    while (b != 0) :
        temp = b
        b = a % b
        a = temp
    #end
    return a
#end
 
def lcm(a, b) :
    return (a * b // gcd(a, b))
#end
 
def lcmm(kIterable) :
 
    # Recursively iterate through pairs of arguments
    if len(kIterable) == 2 :
        return lcm(kIterable[0], kIterable[1])
    #end
 
    return lcm(kIterable[0], lcmm(kIterable[1:]))
 
#end
 
def main() :
 
    kModuleConfiguration = {}
    with open("../input.txt", "r") as inputFile:
       
        for kLine in inputFile :
 
            kLine   = kLine.replace("\n", "")
            kTokens = [k.replace(" ", "") for k in kLine.split("->")]
 
            if "broadcaster" == kTokens[0] :
                kModuleName = kTokens[0]
                eModuleType = EModuleType.BROADCASTER
            else :
                kModuleName = kTokens[0][1:]
                eModuleType = TYPES[kTokens[0][0]]
            #end
 
            assert(kModuleName not in kModuleConfiguration)
            kModuleConfiguration[kModuleName] = ModuleDataData(eModuleType, kTokens[1].split(","), {}, False)
 
        #end
 
    #end
 
    # Initialise all the States a Conjunction Node can receive
    for kModule, kConfiguration in kModuleConfiguration.items() :
 
        for kBroadcastModule in kConfiguration.kBroadcastModules :
 
            if kBroadcastModule in kModuleConfiguration :
 
                if EModuleType.CONJUNCTION == kModuleConfiguration[kBroadcastModule].eModuleType :
 
                    assert(kModule not in kModuleConfiguration[kBroadcastModule].kConjunctionStates)
                    kModuleConfiguration[kBroadcastModule].kConjunctionStates[kModule] = False
 
                #end
 
            #end
 
        #end
 
    #end
 
    # Push the button Frank!
    def pushButton(kModuleConfiguration, nIterations, kFilter = None, kKeyOfInterest = None, bDebug = False) :
 
        nLowPulses   = 0
        nHighPulses  = 0
        n            = 1
 
        while n <= nIterations :
 
            if bDebug:
                print("New Queue...")
            #end
 
            kModuleQueue = [["button", "broadcaster", False]]
 
            while (len(kModuleQueue) > 0) :
 
                kNextPulse = kModuleQueue.pop(0)
 
                if bDebug:
                    print(f"{kNextPulse[0]} {'-high' if kNextPulse[2] else '-low'}-> {kNextPulse[1]}")
                #end
 
                if kNextPulse[2] :
                    nHighPulses += 1
                else :
                    nLowPulses += 1
                #end
 
                # Part Two
                if kKeyOfInterest is not None :
                    if kNextPulse[1] == kKeyOfInterest[0] :
                        if kNextPulse[2] == kKeyOfInterest[1] :
                            return n
                        #end
                    #end
                #end
 
                # Skip Outputs
                if kNextPulse[1] not in kModuleConfiguration : continue
                kModule    = kModuleConfiguration[kNextPulse[1]]
 
                if EModuleType.BROADCASTER == kModule.eModuleType :
 
                    # Broadcasters don't do anything special...
                    for kNextModule in kModule.kBroadcastModules :
                        kModuleQueue.append([kNextPulse[1], kNextModule, kNextPulse[2]])
                    #end
 
                elif EModuleType.FLIPFLOP == kModule.eModuleType :
 
                    # Only Process Low Pulses
                    if False == kNextPulse[2] :
 
                        # Invert the State
                        kModule.bState = not kModule.bState
 
                        # Broadcast the State to the appropriate Modules
                        for kNextModule in kModule.kBroadcastModules :
                            kModuleQueue.append([kNextPulse[1], kNextModule, kModule.bState])
                        #end
 
                    #end
 
                elif EModuleType.CONJUNCTION == kModule.eModuleType :
 
                    # Update the Internal State
                    assert(kNextPulse[0] in kModule.kConjunctionStates)
                    kModule.kConjunctionStates[kNextPulse[0]] = kNextPulse[2]
 
                    # Force Filtered Items to Good
                    if None != kFilter :
                        if kNextPulse[1] in kFilter.keys() :
                            for k in kFilter[kNextPulse[1]] :
                                kModule.kConjunctionStates[k] = True
                            #end
                        #end
                    #end
 
                    # Combine them to a Single Inverted State
                    kModule.bState = not all(kModule.kConjunctionStates.values())
 
                    # Broadcast the State to the appropriate Modules
                    for kNextModule in kModule.kBroadcastModules :
                        kModuleQueue.append([kNextPulse[1], kNextModule, kModule.bState])
                    #end
 
                else :
 
                    assert(0)
 
                #end
               
            #end
                   
            n += 1
 
        #end
                   
        return nLowPulses * nHighPulses
 
    #end
 
    print(f"Part 1: {pushButton(copy.deepcopy(kModuleConfiguration), 1000)}")
 
    # Part 2 is interested in frequencies
 
    # First find the Source Module
    kSourceModule = None
    for kModule, kConfiguration in kModuleConfiguration.items() :
        if "rx" in kConfiguration.kBroadcastModules :
            kSourceModule = kModule
            break
        #end
    #end
    assert(kSourceModule is not None)
 
    # Can't be bothered to solve this generically... it's going to be a Conjunction Module
    assert(EModuleType.CONJUNCTION == kModuleConfiguration[kSourceModule].eModuleType)
    kModuleKeys = list(kModuleConfiguration[kSourceModule].kConjunctionStates.keys())
 
    # We treat this as if it's 4 separate loops abusing the inputs
    kCycles    = []
    for kKey in kModuleKeys :
 
        kFilter = set()
        for kFilterKey in kModuleKeys :
            if kKey == kFilterKey : continue
            kFilter.add(kFilterKey)
        #end
        kCycles.append(pushButton(copy.deepcopy(kModuleConfiguration), math.inf, {kSourceModule : kFilter}, ["rx", False]))
 
    #end
 
    # Find the LCM
    print(f"Part 2: {lcmm(kCycles)}")
 
#end
 
if __name__ == "__main__" :
    startTime = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end