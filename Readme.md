# SSD1315

Platform-agnostic library for SSD1315 OLED.

## Image generation configuration

For font and image output *lcd-image-converter* is used ( [Github](https://github.com/riuson/lcd-image-converter), [Sourceforge](https://sourceforge.net/projects/lcd-image-converter/) ). 

Generation configuration is in `SSD1315.xml` file, it are following:

* Font or image height must be exact multiple of 8 (padding can be used in *lcd-image-converter*).
* Width can be any (except zero).
* Conversion is monochrome.
* Conversion rules require custom script to achieve the following:
    * Each byte represents column segment (with height of 8) in the image, LSB on the top and MSB on the bottom.
    * Column segments go from left to right forming image row.
    * Image rows go from top to bottom forming image.

## Notes on display operations

1. Initialization code taken from [Github](https://github.com/kefir/ssd1315/blob/master/ssd1315.c). Changing initialization sequence brakes operation (display stays black or hanging until power reset) whilst this sequence is not based on official datasheet.
2. Horizontal addressing mode with setting page and column range (as described in datasheet) not works: changes not applied, display hanging, changing command order changes behaivior, etc. So only page addressing mode works well.


## Author

Yuriy Kapoyko ykapoyko@vk.com
