#!/usr/bin/env bash

if git rev-parse --verify HEAD >/dev/null 2>&1; then
    against=HEAD
else
    # The initial commit
    against=e8021046ec587f32f3f8b619d7759c12f3df8492
fi

# Disallow non-ASCII filenames
if test $(git diff --cached --name-only --diff-filter=A -z $against | LC_ALL=C tr -d '[ -~]\0' | wc -c) != 0; then
    echo "Error: Attempt to add a non-ASCII file name."
    exit 1
fi

# Check formatting
files=$(git diff --name-only --cached '*.h' '*.cpp')

if ! diff -u <(cat $files) <(clang-format --style=file $files) > /dev/null; then
    echo "Error: format of staged .h/.cpp files isn't conform."
    exit 1
fi
