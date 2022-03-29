import os
import subprocess
import time
import sys
import math

ITERATIONS = 10

def main() :

    kCommands  = {"solver.c"   : {"description" : "C",
                                  "build"       : [["cmake", "."], ["make"]],
                                  "execute"     : ["./solver"],
                                  "clean"       : [["git", "clean", "-d", "-f", "-x"]]},
                  "solver.cpp" : {"description" : "C++",
                                  "build"       : [["cmake", "."], ["make"]],
                                  "execute"     : ["./solver"],
                                  "clean"       : [["git", "clean", "-d", "-f", "-x"]]},
                  "solver.py"  : {"description" : "Python",
                                  "build"       : [],
                                  "execute"     : ["python3", "-OO", "solver.py"],
                                  "clean"       : []}}

    def profileCommand(kCommand, kExpected) :

        for kBuildCommand in kCommand["build"] :
            subprocess.run(kBuildCommand, stdout=subprocess.PIPE)
        #end

        kTotalTime = []
        for _ in range(ITERATIONS) :

            print(".", end="", file=sys.stderr)
            startTime = time.perf_counter()
            kResult = subprocess.run(kCommand["execute"], stdout=subprocess.PIPE, stderr=subprocess.DEVNULL)
            nTime = (time.perf_counter() - startTime)
            kTotalTime.append(nTime)
            if kResult.stdout != kExpected :
                print("", file=sys.stderr)
                return False, 0
            #end
            sys.stderr.flush()
            time.sleep(0)

        #end

        nTime = min(kTotalTime)

        for kCleanCommand in kCommand["clean"] :
            subprocess.run(kCleanCommand, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        #end
        print("", file=sys.stderr)

        return True, nTime

    #end

    kTimes = {}

    for kRoot, _, kFiles in os.walk(".") :
        for kFile in kFiles :
            if kFile in kCommands :

                kCurrentDir = os.getcwd()
                os.chdir(kRoot)

                kParentRoot = os.path.abspath("../")

                print("Profiling " + kParentRoot + " : " + kFile, end="", file=sys.stderr)

                if not kParentRoot in kTimes :
                    kTimes[kParentRoot] = {}
                    kTimes[kParentRoot]["min"] = math.inf
                #end
                kTimes[kParentRoot][kCommands[kFile]["description"]] = 0

                kFileHandle = os.open("../result.txt", os.O_RDONLY)
                kExpected = os.read(kFileHandle, os.path.getsize("../result.txt"))
                os.close(kFileHandle)

                bValid, kTimes[kParentRoot][kCommands[kFile]["description"]] = profileCommand(kCommands[kFile], kExpected)
                if kTimes[kParentRoot][kCommands[kFile]["description"]] < kTimes[kParentRoot]["min"] :
                    kTimes[kParentRoot]["min"] = kTimes[kParentRoot][kCommands[kFile]["description"]]
                #end

                if bValid is False :
                    print("*** Warning... wrong result returned ***", file=sys.stderr)
                #end

                os.chdir(kCurrentDir)

            #end
        #end
    #end

    kTimeKeysSorted = sorted(kTimes.keys())

    for kCurrent in kTimeKeysSorted :
        for kCommand in kCommands.values() :
            if kCommand["description"] in kTimes[kCurrent] :
                kDescription = kCommand["description"]
                nTime        = kTimes[kCurrent][kDescription]
                nMinTime     = kTimes[kCurrent]["min"]
                print(f"{kCurrent},{kDescription},{nTime * 1000},{(nTime / nMinTime) * 100}")
            #end
        #end
    #end

#end


if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end