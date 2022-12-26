import time
import sys

def main() :
    print("Part1: XXXXXXXXX")
    print("Part2: XXXXXXXXX")
#end

if __name__ == "__main__" :
    startTime      = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end
