#!/usr/bin/awk -f

BEGIN {
    left_count = 0
    right_count = 0
}

{
    left[++left_count] = $1
    right[++right_count] = $2
    right_counts[$2]++
}

END {
    asort(left)
    asort(right)

    task1 = 0
    task2 = 0
    for (i = 1; i <= left_count; i++) {
        task1 += (right[i] > left[i] ? right[i] - left[i] : left[i] - right[i])
        task2 += left[i] * right_counts[left[i]]
    }

    print task1, task2
}
