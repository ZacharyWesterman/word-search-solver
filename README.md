This program takes an image of a word search and solves it.

# Dependencies:
* [OpenCV](https://opencv.org) 2.4: Check their website for installation instructions.
* [tesseract-ocr](https://github.com/tesseract-ocr/tesseract): Your distro's package manager should have something like `tesseract-ocr` or `libtesseract-ocr`. If not, you can always build from source.
* [cereal](https://github.com/USCiLab/cereal): For dictionary and word search data serialization.
* [libzed](https://github.com/ZacharyWesterman/libzed): Mostly just for text manipulation. See the readme for installation instructions.
* Lastly, Make and g++ are required.

Once the above are satisfied, you should be able to compile just by running `make`.
This should result in an executable "wssolve". To solve a word search do something like
```
./wssolve /path/to/image
```

Note word searches currently require a border around the letters, and no other borders (the word bank is assumed to be outside of the border). This program will **NOT** work with oddly-shaped word searches (e.g. triangular).

For best results, the letters in the word search should be well-aligned, the letters should not have anything between them, there should be no illustrations, there should be nothing obscuring the letters or word bank, and all words in the word bank should be on the same side of the image.
