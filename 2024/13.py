#!/usr/bin/env python

import re
import sys
from sympy import symbols, Eq, solve


button_pattern = r"Button (\w): X\+(\d+), Y\+(\d+)"
prize_pattern = r"Prize: X=(\d+), Y=(\d+)"

task1 = 0
task2 = 0


def solve_le(dx_a, dy_a, dx_b, dy_b, X, Y, shift):
    a, b = symbols('a b', integer=True)

    eq1 = Eq(dx_a * a + dx_b * b, X + shift)
    eq2 = Eq(dy_a * a + dy_b * b, Y + shift)

    try:
        solutions = solve((eq1, eq2), (a, b))
        return 3 * solutions[a] + solutions[b]

    except Exception:
        ...
    return 0


sections = sys.stdin.read().strip().split('\n\n')
for section in sections:
    buttons = {}
    it = re.finditer(button_pattern, section)
    m = next(it)
    dx_a, dy_a = int(m.group(2)), int(m.group(3))
    m = next(it)
    dx_b, dy_b = int(m.group(2)), int(m.group(3))
    prize_match = re.search(prize_pattern, section)
    X, Y = int(prize_match.group(1)), int(prize_match.group(2))
    task1 += solve_le(dx_a, dy_a, dx_b, dy_b, X, Y, 0)
    task2 += solve_le(dx_a, dy_a, dx_b, dy_b, X, Y, 10000000000000)


print(task1)
print(task2)
