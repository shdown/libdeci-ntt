#!/usr/bin/env bash

case "$*" in
'v')
    PREFIX=( valgrind -q --leak-check=full --error-exitcode=42 )
    NMUL=200
    NSQR=200
    ;;
'')
    PREFIX=()
    NMUL=8192
    NSQR=2048
    ;;
*)
    echo >&2 "USAGE: $0 [v]"
    ;;
esac

run_driver() {
     "${PREFIX[@]}" ./driver "$@"
}

gen_for_n() {
    local n=$1
    if (( n == 0 )); then
        echo 0
        return
    fi
    local q=$(( n / 9 ))
    local r=$(( n % 9 ))
    local i
    for (( i = 0; i < q; ++i )); do
        echo -n 912345678
    done
    for (( i = 0; i < r; ++i )); do
        echo -n "$(( i == 0 ? 9 : i ))"
    done
    echo
}

test_mul() {
    local n x y
    for (( n = 0; n <= NMUL; ++n )); do
        echo >&2 "[mul] n=$n..."
        x=$(gen_for_n "$n")
        y=$({ echo "$x"; echo 1; } | run_driver mul)
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
    for (( n = 0; n <= NSQR; ++n )); do
        echo >&2 "[sqr] n=$n..."
        x=$(gen_for_n "$n")
        x2=$(python3 -c 'a=int(input()); print(a*a)' <<< "$x")
        y=$(run_driver sqr <<< "$x")
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
