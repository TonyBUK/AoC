import time
import sys

def main() :

    kHailStones = []
    with open("../input.txt", "r") as inputFile:
        kHailStones = [[[int(kToken) for kToken in kTokens[0].split(",")], 
                        [int(kToken) for kToken in kTokens[1].split(",")]]
                            for kLine in inputFile for kTokens in [kLine.replace("\n", "").replace(" ", "").split("@")]]
    #end

    def testIntersection(xMin, xMax, yMin, yMax, kHailStoneA, kHailStoneB) :

        def calculateIntersectPosition(xMin, xMax, yMin, yMax, kHailStoneA, kHailStoneB) :

            def lineIntersection(kLineAStart, kLineAEnd, kLineBStart, kLineBEnd):

                # This first part will compute the intersection of the lines projected to infinity
                xDiff = (kLineAStart[0] - kLineAEnd[0], kLineBStart[0] - kLineBEnd[0])
                yDiff = (kLineAStart[1] - kLineAEnd[1], kLineBStart[1] - kLineBEnd[1])

                def det(a, b):
                    return a[0] * b[1] - a[1] * b[0]

                div = det(xDiff, yDiff)
                if div == 0:
                    return None, None
                #end

                d = (det(kLineAStart, kLineAEnd), det(kLineBStart, kLineBEnd))
                x = det(d, xDiff) / div
                y = det(d, yDiff) / div

                # This second part will test whether the intersection point is within the bounds of the lines
                if ((kLineAStart[0] <= x <= kLineAEnd[0] or kLineAEnd[0] <= x <= kLineAStart[0]) and
                    (kLineBStart[0] <= x <= kLineBEnd[0] or kLineBEnd[0] <= x <= kLineBStart[0]) and
                    (kLineAStart[1] <= y <= kLineAEnd[1] or kLineAEnd[1] <= y <= kLineAStart[1]) and
                    (kLineBStart[1] <= y <= kLineBEnd[1] or kLineBEnd[1] <= y <= kLineBStart[1])) :
                    return x, y
                else:
                    return None, None
                #end
            #end

            # We know the Start Position, so what we need to do for each line is calculate a
            # sensible end point, we can do that by diffing the deltas.
            # Note: This assumes that the Hailstone movement is always at least 1.
            xDeltaFromXMin = max(abs(xMin - kHailStoneA[0][0]), abs(xMin - kHailStoneB[0][0]))
            xDeltaFromXMax = max(abs(xMax - kHailStoneA[0][0]), abs(xMax - kHailStoneB[0][0]))
            yDeltaFromYMin = max(abs(yMin - kHailStoneA[0][1]), abs(yMin - kHailStoneB[0][1]))
            yDeltaFromYMax = max(abs(yMax - kHailStoneA[0][1]), abs(yMax - kHailStoneB[0][1]))
            nFutureCycles  = max(xDeltaFromXMin, xDeltaFromXMax, yDeltaFromYMin, yDeltaFromYMax)

            kHailStoneAEndPoint = (kHailStoneA[0][0] + kHailStoneA[1][0] * nFutureCycles, kHailStoneA[0][1] + kHailStoneA[1][1] * nFutureCycles)
            kHailStoneBEndPoint = (kHailStoneB[0][0] + kHailStoneB[1][0] * nFutureCycles, kHailStoneB[0][1] + kHailStoneB[1][1] * nFutureCycles)

            # Compute the Point at which the Two Lines will intersect
            X, Y = lineIntersection(kHailStoneA[0], kHailStoneAEndPoint, kHailStoneB[0], kHailStoneBEndPoint)

            return (X, Y)

        #end

        kIntersection = calculateIntersectPosition(xMin, xMax, yMin, yMax, kHailStoneA, kHailStoneB)
        if (None, None) == kIntersection :
            return False
        elif (xMin <= kIntersection[0] <= xMax) and (yMin <= kIntersection[1] <= yMax) :
            return True
        else :
            return False
        #end
    #end

    nIntersections = 0
    for i,kHailStoneA in enumerate(kHailStones) :
        for kHailStoneB in kHailStones[i + 1:] :
            if testIntersection(200000000000000, 400000000000000, 200000000000000, 400000000000000, kHailStoneA, kHailStoneB) :
                nIntersections += 1
            #end
        #end
    #end
    print(f"Part 1: {nIntersections}")

    # Part 2
    #
    # I claim zero credit for this... code by google-fu.
    # Basically this: https://github.com/LuciusA1/Direct-and-Iterative-Solver-of-Linear-Systems-Master/blob/a420358d6b0df07d96cb975c0bd15e4559d77db1/Matrix_Solver_Methods.py#L23
    def gaussianElimination(A, b):

        n = len(A)
        M = A

        i = 0
        for x in M:
            x.append(b[i])
            i += 1
        #end

        for k in range(n):
            for i in range(k, n):
                if abs(M[i][k]) > abs(M[k][k]):
                    M[k], M[i] = M[i], M[k]
                #end
            #end

            for j in range(k+1, n):
                q = float(M[j][k]) / M[k][k]
                for m in range(k, n+1):
                    M[j][m] -= q * M[k][m]
                #end
            #end
        #end

        x = [0 for i in range(n)]

        x[n-1] = float(M[n-1][n]) / M[n-1][n-1]
        for i in range(n-1, -1, -1):
            z = 0
            for j in range(i+1, n):
                z = z + float(M[i][j]) * x[j]
            #end
            x[i] = float(M[i][n] - z) / M[i][i]
        #end
        return x
    #end

    def computeMatrixElement(v1, v2, p1, p2, indices):
        element = [0 for _ in indices]
        element[indices[0]] = -(v1[indices[1]] - v2[indices[1]])
        element[indices[1]] =   v1[indices[0]] - v2[indices[0]]
        element[indices[3]] =   p1[indices[1]] - p2[indices[1]]
        element[indices[4]] = -(p1[indices[0]] - p2[indices[0]])
        return element
    #end

    def computeSolutionElement(p1, v1, p2, v2, indices):
        return (p1[indices[1]] * v1[indices[0]] - p2[indices[1]] * v2[indices[0]]) - (p1[indices[0]] * v1[indices[1]] - p2[indices[0]] * v2[indices[1]])
    #end

    assert(len(kHailStones) > 3)

    # Compute Our Initial Matrix
    A = [
        #                    V1                 V2                 P1                 P2                 Indecies
        computeMatrixElement(kHailStones[0][1], kHailStones[1][1], kHailStones[0][0], kHailStones[1][0], [0, 1, 2, 3, 4, 5]),
        computeMatrixElement(kHailStones[0][1], kHailStones[2][1], kHailStones[0][0], kHailStones[2][0], [0, 1, 2, 3, 4, 5]),
        computeMatrixElement(kHailStones[0][1], kHailStones[1][1], kHailStones[0][0], kHailStones[1][0], [1, 2, 0, 4, 5, 3]),
        computeMatrixElement(kHailStones[0][1], kHailStones[2][1], kHailStones[0][0], kHailStones[2][0], [1, 2, 0, 4, 5, 3]),
        computeMatrixElement(kHailStones[0][1], kHailStones[1][1], kHailStones[0][0], kHailStones[1][0], [2, 0, 1, 5, 3, 4]),
        computeMatrixElement(kHailStones[0][1], kHailStones[2][1], kHailStones[0][0], kHailStones[2][0], [2, 0, 1, 5, 3, 4])
    ]

    # Compute our Solutions Matrix
    b = [
        #                      P1                 V1                 P2                 V2                 Indecies
        computeSolutionElement(kHailStones[0][0], kHailStones[0][1], kHailStones[1][0], kHailStones[1][1], [0, 1]),
        computeSolutionElement(kHailStones[0][0], kHailStones[0][1], kHailStones[2][0], kHailStones[2][1], [0, 1]),
        computeSolutionElement(kHailStones[0][0], kHailStones[0][1], kHailStones[1][0], kHailStones[1][1], [1, 2]),
        computeSolutionElement(kHailStones[0][0], kHailStones[0][1], kHailStones[2][0], kHailStones[2][1], [1, 2]),
        computeSolutionElement(kHailStones[0][0], kHailStones[0][1], kHailStones[1][0], kHailStones[1][1], [2, 0]),
        computeSolutionElement(kHailStones[0][0], kHailStones[0][1], kHailStones[2][0], kHailStones[2][1], [2, 0])
    ]

    print(f"Part 2: {round(sum(gaussianElimination(A, b)[:3]))}")

#end
 
if __name__ == "__main__" :
    startTime = time.perf_counter()
    main()
    print(f"{(time.perf_counter() - startTime)}s", file=sys.stderr)
#end