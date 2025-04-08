#!/bin/bash

# === CONFIGURATION ===
# Your MUnit folder and build path
MUNIT_DIR="munit"
MUNIT_BUILD="$MUNIT_DIR/build"
MUNIT_INCLUDE="$MUNIT_DIR"
C_FILE="$1"
OUTPUT="${C_FILE%.c}" # output binary name from filename
CFLAGS="-I$MUNIT_INCLUDE"
USE_VALGRIND=false

# === CHECK INPUT ===
if [[ -z "$C_FILE" ]]; then
  echo "Usage: ./build_and_run.sh path/to/file.c"
  exit 1
fi

if [[ ! -f "$C_FILE" ]]; then
  echo "Error: '$C_FILE' does not exist."
  exit 1
fi


for arg in "$@"; do
  if [[ "$arg" == "--valgrind" ]]; then
    USE_VALGRIND=true
    shift
  fi
done


# === CHECK FOR STATIC LIBRARY ===
if [[ -f "$MUNIT_BUILD/libmunit.a" ]]; then
  echo "🔧 Building with static MUnit..."
  clang "$C_FILE" "$MUNIT_BUILD/libmunit.a" $CFLAGS -o "$OUTPUT"
elif [[ -f "$MUNIT_BUILD/libmunit.so" ]]; then
  echo "🔧 Building with shared MUnit..."
  clang "$C_FILE" -L"$MUNIT_BUILD" -lmunit $CFLAGS -o "$OUTPUT"
else
  echo "❌ Could not find libmunit.a or libmunit.so in $MUNIT_BUILD"
  echo "Please make sure you built MUnit with Meson + Ninja."
  exit 1
fi

# === RUN THE PROGRAM ===
if [[ "$USE_VALGRIND" == true ]]; then
  echo "🧠 Running with Valgrind..."
  LD_LIBRARY_PATH="$MUNIT_BUILD" valgrind --leak-check=full --show-leak-kinds=all ./"$OUTPUT"
else
  echo "🚀 Running $OUTPUT..."
  LD_LIBRARY_PATH="$MUNIT_BUILD" ./"$OUTPUT"
fi

rm ./$OUTPUT