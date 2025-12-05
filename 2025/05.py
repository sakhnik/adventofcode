#!/usr/bin/env python3

def count_merge(ranges):
    ranges = sorted((a, b) for a, b in ranges)

    total = 0
    cur_begin, cur_end = ranges[0]

    for begin, end in ranges[1:]:
        if begin <= cur_end + 1:
            cur_end = max(cur_end, end)
        else:
            total += cur_end - cur_begin + 1
            cur_begin, cur_end = begin, end

    total += cur_end - cur_begin + 1
    return total


res1 = 0
res2 = 0
ranges = []

with open('05.txt', 'r') as f:
    for line in f:
        line = line.strip()
        if not line:
            break
        b, e = map(int, line.split('-'))
        ranges.append((b, e))

    def is_fresh(id):
        for b, e in ranges:
            if b <= id <= e:
                return True
        return False

    for line in f:
        id = int(line.strip())
        if is_fresh(id):
            res1 += 1

print(res1)
print(count_merge(ranges))
