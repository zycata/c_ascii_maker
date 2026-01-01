# C ascii maker

Simple project that can take an input image and generator ascii-art from the image

Usage:
```bash
./ascii.exe path/to/image.png -other -flags
```

#
To build: 

```bash
# Regular build
make

# build with clang
make clang

# compiled with optimizations
make release
```

### Optional flags
- `-mw` maximum width  (Default terminal height or 96 characters)  
- `-mh` maximum height (Default terminal height or 64 characters)  
- `-est` Sobel edge detect threshold (DEFAULT: 67.0) 
- `-cr` Character Ratio (Default 2.0)
- `-ba` Brighten image amount (Default 1.25) 
- `-o`  Output to ASCII to file (DEFAULT not OUTPUT)
- `--usebw` use black and white (Default OFF) 
 


Example use cases 
```bash
# use default paramters for everything
./cascii.exe lightning.jpg

# Dont brighten image
./cascii.exe lightning.jpg -ba 1

# produce black and white output
./cascii.exe lightning.jpg --usebw

```
