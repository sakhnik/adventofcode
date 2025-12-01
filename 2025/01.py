#!/usr/bin/env python3

count1 = 0
count2 = 0
pos = 50

with open('01.txt', 'r') as f:
    for line in f:
        sign = 1 if line[0] == 'R' else -1
        clicks = int(line[1:].strip())

        next_pos = pos + (clicks * sign)
        start = min(pos, next_pos)
        finish = max(pos, next_pos)
        count2 += finish // 100 - ((start - 1) // 100) - (pos == 0)

        pos = ((next_pos % 100) + 100) % 100
        if pos == 0:
            count1 += 1

print(count1)
print(count2)
