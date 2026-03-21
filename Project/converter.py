from PIL import Image

INPUT_IMAGE = "board.png"
OUTPUT_FILE = "image.h"

WIDTH = 320
HEIGHT = 240

def rgb888_to_rgb565(r, g, b):
    r5 = r >> 3
    g6 = g >> 2
    b5 = b >> 3
    return (r5 << 11) | (g6 << 5) | b5


img = Image.open(INPUT_IMAGE).convert("RGB")
img = img.resize((WIDTH, HEIGHT))

with open(OUTPUT_FILE, "w") as f:

    f.write("#ifndef IMAGE_H\n")
    f.write("#define IMAGE_H\n\n")

    f.write("unsigned short image[240][320] = {\n")

    for y in range(HEIGHT):
        f.write("{")
        for x in range(WIDTH):
            r, g, b = img.getpixel((x, y))
            color = rgb888_to_rgb565(r, g, b)
            f.write(f"0x{color:04X}")
            if x != WIDTH - 1:
                f.write(",")
        f.write("}")

        if y != HEIGHT - 1:
            f.write(",\n")

    f.write("\n};\n\n")
    f.write("#endif\n")

print("image.h")