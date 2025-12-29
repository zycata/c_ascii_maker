# C ascii maker

Simple project that can take an input image and generator ascii-art from the image

Usage:
'''
./ascii.exe path/to/image.png -other -flags
'''

#
To build: 

```bash
# Regular build
make

# compiled with optimizations
make release
```

### Optional flags
-`-mw` maximum width  (Default terminal height or 96 characters)
-`-mh` maximum height (Default terminal height or 64 characters)
-`-esl` Sobel edge detect threshold (TODO)
-`-ba` Brighten image amount (Default 1.25)
-`--usebw` use black and white (Default OFF)


Example use cases 
```bash
# use default paramters for everything
./cascii.exe lightning.jpg

# Dont brighten image
./cascii.exe lightning.jpg -ba 1

# produce black and white output
./cascii.exe lightning.jpg --usebw

```
