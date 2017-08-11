# Concordance
Simple library and example app for calculating concordance in UTF-8 text.

Can find the concordance in an alphabetical order or by frequency.
Accepts a list of separator characters.
## Possible improvements:
Allow separator strings of more than one character.
Templated counters.
Potentially, Performance.
Tests.
# Usage
Compile with `make`.
run `conc [-F] [filename]`

Command line options:
- `-F`: Sort by frequency.
- `filename`: name of filename from which to read the text. If none is specified, stdin is used (reads until EOF).
