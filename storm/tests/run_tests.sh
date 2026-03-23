#!/bin/bash
# storm regression test runner
# pipeline for each test: storm source → compiler → nasm → gcc → run → compare output

PROJECT_ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
TESTS_DIR="$(cd "$(dirname "$0")" && pwd)"
CASES_DIR="$TESTS_DIR/cases"
EXPECTED_DIR="$TESTS_DIR/expected"
COMPILER="$PROJECT_ROOT/s"

GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m'

pass=0
fail=0
skip=0


if [ ! -f "$COMPILER" ]; then
    echo -e "${RED}error: compiler binary not found at $COMPILER${NC}"
    echo "build the project first: cmake . && make"
    exit 1
fi

if ! command -v nasm &>/dev/null; then
    echo -e "${RED}error: nasm not installed. run: sudo apt install nasm${NC}"
    exit 1
fi

# -- backup storm-example.storm so we can restore it after the run --

ORIGINAL="$PROJECT_ROOT/storm-example.storm"
BACKUP="$PROJECT_ROOT/.storm-example.bak"
[ -f "$ORIGINAL" ] && cp "$ORIGINAL" "$BACKUP"

cleanup() {
    [ -f "$BACKUP" ] && mv "$BACKUP" "$ORIGINAL" || true
    rm -f "$PROJECT_ROOT/storm.o" "$PROJECT_ROOT/storm_run"
}
trap cleanup EXIT

# -- run each test case in alphabetical order --

echo ""
echo "storm regression tests"
echo "======================"
echo ""

for case_file in "$CASES_DIR"/*.storm; do
    [ -f "$case_file" ] || continue
    test_name=$(basename "$case_file" .storm)
    expected_file="$EXPECTED_DIR/$test_name.txt"

    if [ ! -f "$expected_file" ]; then
        echo -e "${YELLOW}SKIP${NC}  $test_name  (no expected output file)"
        ((skip++))
        continue
    fi

    # use this case as the compiler input
    cp "$case_file" "$ORIGINAL"

    # step 1: compile storm source → storm.asm
    # suppress all compiler output (debug cerr + file content cout)
    if ! "$COMPILER" >/dev/null 2>&1; then
        echo -e "${RED}FAIL${NC}  $test_name  → compiler crashed"
        ((fail++))
        continue
    fi

    if [ ! -f "$PROJECT_ROOT/storm.asm" ]; then
        echo -e "${RED}FAIL${NC}  $test_name  → storm.asm not generated"
        ((fail++))
        continue
    fi

    # step 2: assemble
    if ! nasm -f elf64 "$PROJECT_ROOT/storm.asm" -o "$PROJECT_ROOT/storm.o" 2>/dev/null; then
        echo -e "${RED}FAIL${NC}  $test_name  → nasm assembly failed"
        ((fail++))
        continue
    fi

    # step 3: link against libc for printf
    if ! gcc "$PROJECT_ROOT/storm.o" -o "$PROJECT_ROOT/storm_run" -no-pie 2>/dev/null; then
        echo -e "${RED}FAIL${NC}  $test_name  → gcc linking failed"
        ((fail++))
        continue
    fi

    # step 4: run binary and compare stdout against expected
    actual=$("$PROJECT_ROOT/storm_run" 2>/dev/null || true)
    expected=$(cat "$expected_file")

    if [ "$actual" = "$expected" ]; then
        echo -e "${GREEN}PASS${NC}  $test_name"
        ((pass++))
    else
        echo -e "${RED}FAIL${NC}  $test_name"
        echo "        expected: |$expected|"
        echo "        actual:   |$actual|"
        ((fail++))
    fi
done

echo ""
echo "======================"
printf "results: ${GREEN}%d passed${NC}, ${RED}%d failed${NC}, ${YELLOW}%d skipped${NC}\n" $pass $fail $skip
echo ""

[ $fail -eq 0 ]
