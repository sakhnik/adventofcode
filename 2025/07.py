#!/usr/bin/env python3

res1 = 0
res2 = 0

with open('07.txt') as f:
    counts = {}
    for line in f:
        next_counts = {}
        line = line.strip()
        width = len(line)

        if not counts:
            col = line.index('S')
            counts[col] = 1
            continue

        for col, count in counts.items():
            if line[col] == '^':
                res1 += 1

                left = col - 1
                right = col + 1

                next_counts[left] = next_counts.get(left, 0) + count
                next_counts[right] = next_counts.get(right, 0) + count

            else:
                next_counts[col] = next_counts.get(col, 0) + count

        counts = next_counts

    res2 = sum(counts.values())

print(res1)
print(res2)
