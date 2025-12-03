#!/usr/bin/env python3

def best(seq, count):
    stack = []
    n = len(seq)

    for i, ch in enumerate(seq):
        while stack and stack[-1] < ch and len(stack) + n - i > count:
            stack.pop()
        if len(stack) < count:
            stack.append(ch)

    return ''.join(stack)


res1 = 0
res2 = 0

with open('03.txt', 'r') as f:
    for line in f:
        s = line.strip()
        res1 += int(best(s, 2))
        res2 += int(best(s, 12))

print(res1)
print(res2)
