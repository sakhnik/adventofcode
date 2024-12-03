#!/usr/bin/awk -f

BEGIN {
    task1 = 0; task2 = 0;
    enabled = 1;  # true
    pattern = ""
}

{
    while (match($0, /mul\(([0-9]+),([0-9]+)\)|do\(\)|don't\(\)/, matches)) {
        if (matches[0] == "do()") {
            enabled = 1;  # true
        } else if (matches[0] == "don't()") {
            enabled = 0;  # false
        } else if (matches[1] != "" && matches[2] != "") {
            mul = matches[1] * matches[2];
            task1 += mul;
            if (enabled) {
                task2 += mul;
            }
        }
        # Move to the rest of the line after the match
        $0 = substr($0, RSTART + RLENGTH);
    }
}

END {
    print task1, task2;
}
