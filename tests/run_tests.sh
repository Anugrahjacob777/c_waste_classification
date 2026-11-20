#!/bin/bash
# Runs every test file through the program and checks the counts.
# Usage (from the project folder):  bash tests/run_tests.sh

gcc -Wall src/waste_classifier.c -o /tmp/waste_classifier_test || exit 1
pass=0
fail=0

# check <file> <expected total> <expected unknown> <class line that must appear>
check() {
    out=$(printf "2\n3\n%s\n1\n0\n0\n" "$1" | TERM=xterm /tmp/waste_classifier_test 2>/dev/null)
    t=$(echo "$out" | grep "Total items classified" | head -1 | awk '{print $NF}')
    u=$(echo "$out" | grep "Unknown items" | head -1 | awk '{print $NF}')
    c=$(echo "$out" | grep -c -E "^  $4\$")
    if [ "$t" = "$2" ] && [ "$u" = "$3" ] && [ "$c" -ge 1 ]; then
        echo "PASS  $1  (total=$t unknown=$u)"
        pass=$((pass + 1))
    else
        echo "FAIL  $1  (total=$t unknown=$u, expected $2/$3, line \"$4\")"
        fail=$((fail + 1))
    fi
}

check tests/test_paper.txt      10 0 "Paper      : 10"
check tests/test_plastic.txt    10 0 "Plastic    : 10"
check tests/test_glass.txt      10 0 "Glass      : 10"
check tests/test_metal.txt      10 0 "Metal      : 10"
check tests/test_organic.txt    10 0 "Organic    : 10"
check tests/test_ewaste.txt     10 0 "E-waste    : 10"
check tests/test_edge_cases.txt  5 5 "Plastic    : 1"
check data/all_items.txt        98 3 "Plastic    : 18"

echo "Passed: $pass   Failed: $fail"
[ "$fail" -eq 0 ]
