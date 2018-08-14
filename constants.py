BYTEORDER = 'big'

MSG_LINK = 1
MSG_DV = 2
MSG_DST = 3

UPDATE_INTERVAL = 5.0
PORT = "/dev/ttyS8"
BAUD = 9600

INF = 255

NORTH = 0
EAST = 1
SOUTH = 2
WEST = 3

ADDRESS = ["5678", "90AB", "CDEF", "1011", "1213"]
ADDRESS_CAR = "F0F1"

GRAPH1 = {
    0: [[8, 8, NORTH], [1, 10, EAST], [2, 8, SOUTH], [9, 24, WEST]],
    1: [[7, 2, NORTH], [6, 15, EAST], [3, 52, SOUTH], [0, 10, WEST]],
    2: [[0, 8, NORTH], [3, 22, EAST], [4, 60, SOUTH], [10, 17, WEST]],
    3: [[1, 52, NORTH], [5, 7, EAST], [4, 13, SOUTH], [2, 22, WEST]],
    4: [[2, 60, NORTH], [3, 13, EAST], [12, 11, SOUTH], [11, 23, WEST]],
    5: [[3, 7, WEST]],
    6: [[1, 15, WEST]],
    7: [[1, 2, SOUTH]],
    8: [[0, 8, SOUTH]],
    9: [[0, 24, EAST]],
    10: [[2, 17, EAST]],
    11: [[4, 23, EAST]],
    12: [[4, 11, NORTH]]
}

GRAPH2 = {
    0: [[7, 8, NORTH], [1, 24, EAST], [3, 4, SOUTH]],
    1: [[6, 6, NORTH], [5, 7, EAST], [2, 55, SOUTH], [0, 24, WEST]],
    2: [[1, 55, NORTH], [4, 8, EAST], [3, 4, WEST]],
    3: [[0, 4, NORTH], [2, 4, EAST]],
    4: [[2, 8, WEST]],
    5: [[1, 7, WEST]],
    6: [[1, 6, SOUTH]],
    7: [[0, 8, SOUTH]]
}

GRAPH3 = {
    0: [[12, 2, NORTH], [1, 55, EAST], [3, 12, SOUTH], [4, 6, WEST]],
    1: [[11, 9, NORTH], [10, 2, EAST], [2, 18, SOUTH], [0, 55, WEST]],
    2: [[1, 18, NORTH], [9, 4, EAST], [3, 15, WEST]],
    3: [[0, 12, NORTH], [2, 15, EAST], [8, 1, WEST]],
    4: [[5, 9, NORTH], [0, 6, EAST], [7, 7, SOUTH], [6, 9, WEST]],
    5: [[4, 9, SOUTH]],
    6: [[4, 9, EAST]],
    7: [[4, 7, NORTH]],
    8: [[3, 1, EAST]],
    9: [[2, 4, WEST]],
    10: [[1, 2, WEST]],
    11: [[1, 9, SOUTH]],
    12: [[0, 2, SOUTH]]
}
