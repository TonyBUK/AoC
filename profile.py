import os
import subprocess
import time
import sys
import math

GROUPING_COUNT = 5

def main(kOutputFormat) :

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

        kTotalTime      = []
        nAcceptedTime   = -1

        while True :

            print(".", end="", file=sys.stderr)
            sys.stderr.flush()

            startTime = time.perf_counter()
            kResult = subprocess.run(kCommand["execute"], stdout=subprocess.PIPE, stderr=subprocess.DEVNULL)
            nTime = (time.perf_counter() - startTime)

            kTotalTime.append(nTime)
            if kResult.stdout != kExpected :
                print("", file=sys.stderr)
                return False, 0
            #end
            print(".", end="", file=sys.stderr)
            sys.stderr.flush()

            # Keep Going until we have at least the grouping count
            if len(kTotalTime) >= GROUPING_COUNT :
                kTotalTime  = sorted(kTotalTime)
                nMinTime    = min(kTotalTime)

                nComparisonTest      = 0.0005
                nComparisonMagnitude = 1

                while nMinTime > nComparisonMagnitude :
                    nComparisonTest      *= 10
                    nComparisonMagnitude *= 10
                #end

                # Look for a range of times that are within a sensible range
                # of each other for the magnitude
                for t1, t2 in zip(kTotalTime, kTotalTime[GROUPING_COUNT:]) :
                    if abs(t2 - t1) <= nComparisonTest :
                        nAcceptedTime = ((t2-t1)/2)+t1
                        break
                    #end
                #end
            #end

            if -1 != nAcceptedTime : break

        #end

        for kCleanCommand in kCommand["clean"] :
            subprocess.run(kCleanCommand, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        #end
        print("", file=sys.stderr)

        return True, nAcceptedTime

    #end

    kTimes = {}

    for kRoot, _, kFiles in os.walk(".") :
        for kFile in kFiles :
            if kFile in kCommands :

                kCurrentDir = os.getcwd()
                os.chdir(kRoot)

                kParentRoot  = os.path.abspath("../")
                kPathSplit   = os.path.split(kParentRoot)
                kDay         = kPathSplit[1]
                kYear        = os.path.split(kPathSplit[0])[1]
                kParentTuple = tuple([kYear, kDay])

                print("Profiling " + kParentRoot + " : " + kFile, end="", file=sys.stderr)

                if not kParentTuple in kTimes :
                    kTimes[kParentTuple] = {}
                    kTimes[kParentTuple]["min"]  = math.inf
                    kTimes[kParentTuple]["path"] = kParentRoot
                #end
                kTimes[kParentTuple][kCommands[kFile]["description"]] = 0

                kFileHandle = os.open("../result.txt", os.O_RDONLY)
                kExpected = os.read(kFileHandle, os.path.getsize("../result.txt"))
                os.close(kFileHandle)

                bValid, kTimes[kParentTuple][kCommands[kFile]["description"]] = profileCommand(kCommands[kFile], kExpected)
                if kTimes[kParentTuple][kCommands[kFile]["description"]] < kTimes[kParentTuple]["min"] :
                    kTimes[kParentTuple]["min"] = kTimes[kParentTuple][kCommands[kFile]["description"]]
                #end

                if bValid is False :
                    print("*** Warning... wrong result returned ***", file=sys.stderr)
                #end

                os.chdir(kCurrentDir)

            #end
        #end
    #end

    def outputHeader(kOutputFormat, kDescription, kFileHandle) :
        if "csv" == kOutputFormat :
            pass
        else :
            print("Language : Time      : Percentage of Best Time", file=kFileHandle)
            print("=========:===========:========================", file=kFileHandle)
        #end
    #end

    def output(kOutputFormat, kDescription, kLanguage, nTime, nMinTime, kFileHandle) :
        if "csv" == kOutputFormat :
            print(f"{kDescription},{kLanguage},{nTime * 1000},{(nTime / nMinTime) * 100}", file=kFileHandle)
        else :
            strTime = ""
            if nMinTime < 1 :
                nMinTime = round(nMinTime * 1000) / 1000
            elif nMinTime < 10 :
                nMinTime = round(nMinTime * 100) / 100
            elif nMinTime < 60 :
                nMinTime = round(nMinTime * 10) / 10
            else :
                nMinTime = round(nMinTime)
            #end

            if nTime < 1 :
                nTime = round(nTime * 1000) / 1000
                strTime = "{0:.0f}".format(nTime*1000) + "ms"
            elif nTime < 10 :
                nTime = round(nTime * 100) / 100
                strTime = "{0:.2f}".format(nTime) + "s"
            elif nTime < 60 :
                nTime = round(nTime * 10) / 10
                strTime = "{0:.1f}".format(nTime) + "s"
            else :
                nTime = round(nTime)
                strTime = "{0:.1f}".format(nTime) + "s"
            #end
            percentTime = "{0:.2f}".format(nTime/nMinTime*100) + "%"
            print(f"{str.ljust(kLanguage,8)} : {str.ljust(strTime,9)} : {percentTime}", file=kFileHandle)
        #end
    #end

    kTimeKeysSorted = sorted(kTimes.keys())
    kYears          = {}

    for kCurrent in kTimeKeysSorted :

        if kCurrent[0] not in kYears :
            kYears[kCurrent[0]] = {}
            kYears[kCurrent[0]]["min"]   = math.inf
            kYears[kCurrent[0]]["count"] = 0
            kYears[kCurrent[0]]["path"]  = os.path.split(kTimes[kCurrent]["path"])[0]
        #end

        if kOutputFormat == "csv" :
            kFileHandle = sys.stdout
        else :
            kFileHandle = open(kTimes[kCurrent]["path"] + "/" + "times.txt", "w")
        #end
        outputHeader(kOutputFormat, kCurrent, kFileHandle)

        for kCommand in kCommands.values() :

            if kCommand["description"] in kTimes[kCurrent] :

                if kCommand["description"] not in kYears[kCurrent[0]] :
                    kYears[kCurrent[0]][kCommand["description"]] = 0
                #end

                kDescription = kCommand["description"]
                nTime        = kTimes[kCurrent][kDescription]
                nMinTime     = kTimes[kCurrent]["min"]
                output(kOutputFormat, kCurrent[0] + '/' + kCurrent[1], kCommand["description"], nTime, nMinTime, kFileHandle)
                kYears[kCurrent[0]][kCommand["description"]] += nTime
                kYears[kCurrent[0]]["count"]                 += 1
            #end
        #end

        if kOutputFormat != "csv" :
            kFileHandle.close()
        #end
    #end

    for kYear,kTimes in kYears.items() :

        if kTimes["count"] != (25 * 3) :

            for kKey,nTime in kTimes.items() :
                
                if kKey == "min"   : continue
                if kKey == "count" : continue
                if kKey == "path"  : continue

                if nTime < kTimes["min"] :
                    kTimes["min"] = nTime
                #end
            #end

            if kOutputFormat == "csv" :
                kFileHandle = sys.stdout
            else :
                kFileHandle = open(kTimes["path"] + "/" + "times.txt", "w")
            #end

            nMinTime = kTimes["min"]
            outputHeader(kOutputFormat, kYear, kFileHandle)
            for kKey, nTime in kTimes.items() :
                if kKey == "min"   : continue
                if kKey == "count" : continue
                if kKey == "path"  : continue

                output(kOutputFormat, kYear, kKey, nTime, nMinTime, kFileHandle)
            #end

            if kOutputFormat != "csv" :
                kFileHandle.close()
            #end

        #end

    #end

#end

if __name__ == "__main__" :
    startTime      = time.perf_counter()
    if len(sys.argv) <= 1 :
        main("csv")
    else :
        main(sys.argv[1])
    #end
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end