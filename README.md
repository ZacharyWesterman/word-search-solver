This program takes an image of a word search and solves it.

There are two versions of this program: `wssolve-txt` and `wssolve-img`.
They allow solving of plain text and image-based word searches, respectively.
Note that the latter is more error-prone and requires a few more dependencies than the former.
If this is a one-time thing, or the image is exceptionally messy, I would suggest first transcribing the word search, then using wssolve-txt.

# Image Processing Dependencies
* [OpenCV](https://opencv.org) 2.4: Check their website for installation instructions.
* [tesseract-ocr](https://github.com/tesseract-ocr/tesseract): Your distro's package manager should have something like `tesseract-ocr` or `libtesseract-ocr`. If not, you can always build from source.

Note that the above are only needed by `wssolve-img`.

# Shared Dependencies:
* [cereal](https://github.com/USCiLab/cereal): For dictionary and word search data serialization.
* [libzed](https://github.com/ZacharyWesterman/libzed): Mostly just for text manipulation. See the readme for installation instructions.
* Lastly, Make and g++ are required.

Once all dependencies are satisfied, you should be able to compile just by running `make wssolve-txt` and/or `make wssolve-img`.
This should result in an executable "wssolve". To solve a word search do something like
```
./wssolve-img /path/to/image
```
or
```
./wssolve-txt /path/to/text/file
```

### Notes for processing images of word searches
Accurately processing the word search currently requires there to be a border around the letters, and no other borders (the word bank is assumed to be outside of the border). This program will **NOT** work with oddly-shaped word searches (e.g. triangular).

For best results, follow these guidelines with images of word searches:
* The letters in the word search should be well-aligned.
* The letters should not have anything between them.
* There should be no (or few) illustrations.
* There should be nothing obscuring the letters or word bank.
* All words in the word bank should be on the same side of the image.
