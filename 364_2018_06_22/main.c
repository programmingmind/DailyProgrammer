// The main idea behind this algorithm is that all you need to do at every step
// is make sure that the number of open blocks is divisible by 5. Since you can
// make any shape with just 5 squares, it doesn't matter to actually check for
// shapes just the actual area. So just do a DFS search on each square to make
// sure that any area you sub divide stays divisble by 5

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_DIM 32
#define ARR_LEN ((MAX_DIM*MAX_DIM)/32)

#define boardNdx(x, y) (((y) * MAX_DIM + (x)) / 32)
#define boardMask(x, y) (1 << (((y) * MAX_DIM + (x)) % 32))
#define checkNdx(b, x, y) ((b)[boardNdx((x), (y))] &   boardMask((x), (y)))
#define setNdx(b, x, y)   ((b)[boardNdx((x), (y))] |=  boardMask((x), (y)))
#define clearNdx(b, x, y) ((b)[boardNdx((x), (y))] &= ~boardMask((x), (y)))

void copyBoard(uint32_t* src, uint32_t* dest) {
    for (int i = 0; i < ARR_LEN; i++) {
        dest[i] = src[i];
    }
}

int countEmptyAndSetDFS(uint32_t* workBoard, int x, int y) {
    if (x < 0 || y < 0 || x >= MAX_DIM || y >= MAX_DIM) {
        return 0;
    }
    if (checkNdx(workBoard, x, y)) {
        return 0;
    }
    setNdx(workBoard, x, y);
    return 1 +
        countEmptyAndSetDFS(workBoard, x-1, y  ) +
        countEmptyAndSetDFS(workBoard, x+1, y  ) +
        countEmptyAndSetDFS(workBoard, x  , y-1) +
        countEmptyAndSetDFS(workBoard, x  , y+1);
}

int checkValid(uint32_t* b, int depth) {
    // Every empty space should be divisible by 5, except for maybe 1 space
    // which is divisible by 5 when depth is added
    uint32_t workBoard[ARR_LEN];
    copyBoard(b, workBoard);

    // Go through the board and count the number of empty spaces that are
    // connected. We just go through the board looking for empty spaces and then
    // do a DFS search around it for adjacent squares
    int partialSet = 0;
    for (int y = 0; y < MAX_DIM; y++) {
        for (int x = 0; x < MAX_DIM; x++) {
            const int count = countEmptyAndSetDFS(workBoard, x, y);
            if (count % 5) {
                if ((count + depth) % 5) {
                    return 0;
                } else if (partialSet) {
                    return 0;
                } else {
                    partialSet = 1;
                }
            }
        }
    }
    return 1;
}

int findNextDFS(uint32_t* b, int *indices, int x, int y, int depth) {
    if (x < 0 || y < 0 || x >= MAX_DIM || y >= MAX_DIM) {
        return 0;
    }
    if (checkNdx(b, x, y)) {
        return 0;
    }
    setNdx(b, x, y);
    if (checkValid(b, depth+1)) {
        indices[depth*2  ] = x;
        indices[depth*2+1] = y;
        if (++depth == 5) {
            return 1;
        }
        if (findNextDFS(b, indices, x-1, y  , depth) ||
            findNextDFS(b, indices, x+1, y  , depth) ||
            findNextDFS(b, indices, x  , y-1, depth) ||
            findNextDFS(b, indices, x  , y+1, depth)) {
            return 1;
        }
    }
    clearNdx(b, x, y);
    return 0;
}

int findNextMove(uint32_t* b, int *indices) {
    for (int y = 0; y <= MAX_DIM; y++) {
        for (int x = 0; x <= MAX_DIM; x++) {
            if (findNextDFS(b, indices, x, y, 0)) {
                return 1;
            }
        }
    }
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s dimX dimY [skipX skipY]*\n", argv[0]);
        return 1;
    }

    // read in dimension
    const int dimX = atoi(argv[1]);
    const int dimY = atoi(argv[2]);

    uint32_t input[ARR_LEN];
    for (int y = 0; y < MAX_DIM; y++) {
        for (int x = 0; x < MAX_DIM; x++) {
            if (x < dimX && y < dimY) {
                clearNdx(input, x, y);
            } else {
                setNdx(input, x, y);
            }
        }
    }

    char *solution = (char*)calloc(dimY * (dimX+1) + 1, 1);
    for (int y = 0; y < dimY; y++) {
        for (int x = 0; x <= dimX; x++) {
            solution[y*(dimX+1) + x] = x < dimX ? '?' : '\n';
        }
    }

    for (int i = 3; i+1 < argc; i += 2) {
        const int x = atoi(argv[i]);
        const int y = atoi(argv[i+1]);
        setNdx(input, x, y);
        solution[y*(dimX+1) + x] = '*';
    }

    char letter = 'a';
    int indices[10];
    while (findNextMove(input, indices)) {
        for (int i = 0; i < 5; i++) {
            int x = indices[i*2];
            int y = indices[i*2+1];
            solution[y * (dimX+1) + x] = letter;
        }
        letter++;
    }

    if (checkValid(input, 0)) {
        for (int i = 0; i < ARR_LEN; i++) {
            if (input[i] != 0xFFFFFFFF) {
                printf("Incomplete!\n");
                break;
            }
        }
    } else {
        printf("NOT POSSIBLE!\n");
    }
    printf("%s", solution);
    return 0;
}