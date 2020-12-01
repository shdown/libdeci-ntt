#!/usr/bin/env bash

gen_for_n() {
    local n=$1
    local q=$(( n / 9 ))
    local r=$(( n % 9 ))
    local i
    for (( i = 0; i < q; ++i )); do
        echo -n 123456789
    done
    for (( i = 1; i <= r; ++i )); do
        echo -n "$i"
    done
    echo
}

test_mul() {
    local n x y
    for (( n = 1; n <= 8192; ++n )); do
        echo >&2 "[mul] n=$n..."
        x=$(gen_for_n "$n")
        y=$({ echo "$x"; echo 1; } | ./driver mul)
        if [[ $x != $y ]]; then
            echo >&2 "FAIL"
            echo >&2 "EXPECTED: $x"
            echo >&2 "FOUND: $y"
            exit 1
        fi
    done
}

test_sqr() {
    local n x x2 y
    for (( n = 1; n <= 2048; ++n )); do
        echo >&2 "[sqr] n=$n..."
        x=$(gen_for_n "$n")
        x2=$(python3 -c 'a=int(input()); print(a*a)' <<< "$x")
        y=$(./driver sqr <<< "$x")
        if [[ $x2 != $y ]]; then
            echo >&2 "FAIL"
            echo >&2 "EXPECTED: $x2"
            echo >&2 "FOUND: $y"
            exit 1
        fi
    done
}

test_mul
test_sqr

echo >&2 "PASSED"
