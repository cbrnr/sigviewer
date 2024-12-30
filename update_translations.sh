#!/bin/bash

cd "$(git rev-parse --show-toplevel)"

lupdate-qt5 $(git ls-files | egrep '\.cpp$|\.h$|\.ui$') -ts $(git ls-files | egrep '\.ts$') "${@}"
