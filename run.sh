#!/bin/bash

LISP_DIR="$(dirname $0)" 
PROJECT_DIR="$1"

"${LISP_DIR}/a.out" "$LISP_DIR" "$PROJECT_DIR"

