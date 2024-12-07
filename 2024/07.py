#!/usr/bin/env python

import sys


def test(target: int, i: int, nums: [int]):
    if i == 0:
        return target == nums[0]

    if target % nums[i] == 0 and test(int(target / nums[i]), i - 1, nums):
        return True
    if target >= nums[i] and test(target - nums[i], i - 1, nums):
        return True
    return False


def test2(target: int, i: int, nums: [int]):
    if i == 0:
        return target == nums[i]

    if target % nums[i] == 0 and test2(int(target / nums[i]), i - 1, nums):
        return True
    if target >= nums[i] and test2(target - nums[i], i - 1, nums):
        return True
    try:
        sn = str(nums[i])
        st = str(target)
        if st.endswith(sn) and test2(int(str(target)[:-len(sn)]), i - 1, nums):
            return True
    except Exception:
        pass
    return False


task1 = 0
task2 = 0
for line in sys.stdin.readlines():
    vals = line.split()
    target = int(vals[0][:-1])
    nums = [int(i) for i in vals[1:]]
    if test(target, len(nums) - 1, nums):
        task1 += target
    if test2(target, len(nums) - 1, nums):
        task2 += target

print(task1)
print(task2)
