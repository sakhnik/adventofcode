#!/usr/bin/env python3


res1 = 0
with open('04.txt', 'r') as f:
    m = [[ch for ch in line] for line in f.readlines()]
    height = len(m)
    width = len(m[0])

    def has_roll(r, c):
        return r >= 0 and c >= 0 and r < height and c < width and m[r][c] == '@'

    def remove():
        res = 0
        m2 = [[ch for ch in line] for line in m]

        for row in range(height):
            for col in range(width):
                if not has_roll(row, col):
                    continue
                count = -1
                for dr in range(-1, 2):
                    for dc in range(-1, 2):
                        count += 1 if has_roll(row + dr, col + dc) else 0
                if count < 4:
                    res += 1
                    m2[row][col] = ' '
        return res, m2

    counts = []
    while True:
        res, m = remove()
        counts.append(res)
        if not res:
            break

print(counts[0])
print(sum(counts))
