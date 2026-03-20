#!/bin/bash

cd "$(git rev-parse --show-toplevel)"

lupdate $(git ls-files | grep -E '\.cpp$|\.h$|\.ui$') -ts $(git ls-files | grep -E '\.ts$') "${@}"
