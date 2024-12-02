#!/usr/bin/awk -f

function check_levels(arr, n) {
    order = 0
    for (l = 1; l < n; l++) {
        dist = arr[l+1] - arr[l]
        if (!order)
            order = dist
        if (order > 0 && (dist < 1 || dist > 3)) 
            return 0
        if (order <= 0 && (dist < -3 || dist > -1)) 
            return 0
    }
    return 1
}

function check_damped_levels(arr, n) {
    for (i = 1; i <= n; i++) {
        k = 1
        for (j = 1; j <= n; j++) {
            if (j != i) arr2[k++] = arr[j]
        }
        if (check_levels(arr2, n - 1)) {
            return 1
        }
    }
    return 0
}

BEGIN { task1 = 0; task2 = 0 }
{
    split($0, levels, / /)
    n = length(levels)
    if (check_levels(levels, n)) {
        task1++
        task2++
    } else {
        if (check_damped_levels(levels, n))
            task2++
    }
}
END { print task1, task2 }
