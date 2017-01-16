# word-search-solver
A command-line program that analyzes a grid of letters in the user specified file, and solves it according to regular word-search rules, and any stipulations the user adds.

To start, launch the terminal, then change the directory to "FULL DIRECTORY/word-search-solver/bin/debug". Then to run the program you have a few possibilities.


### Possible command formats:

Search for individual words/phrases in the word search,
```analyzewordsearch "path/file_name.extension"```

Find all words in the word search,
```analyzewordsearch "path/file_name.extension" all```

List all words containing at least X characters,
```analyzewordsearch "path/file_name.extension" all from X```

List all words containing at most Y characters,
```analyzewordsearch "path/file_name.extension" all to Y```

List all words containing between X and Y characters, inclusive.
```analyzewordsearch "path/file_name.extension" all from X to Y```
