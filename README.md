png2fb converts PNG image to 16 bit 565 raw format.
Can be used for framebuffer splash screens for TV-sets or IPTV/hybrid STBs.
How to build:
  make 
or something like
  make CROSS=sh4-linux-
for targetted builds
Options:
--help, -h	Short description.
--usage		The same.
--verbose, -v	Detailed output.
--input, -i	Source PNG file, required parameter.
--output, -o	Destination file. Default is stdout.
--width
--height	Output width and height. Default values will be taken 
		from source PNG.
--valign	[top|center|bottom] Vertical
--halign	[left|center|right] and horisontal alignment rules.
--color		Background color in HEX RGB. Like 0xff0000 for red. 
		If source image less then destination output, free space 
		will be filled with specified color.
Example 1:
suppose we have logo.png 400x400. Then command
png2fb --input logo.png --output logo.fb --width 1280 --height 720 \
--valign center --halign center --color 0x00ff00
will create file "logo.fb" wich can be showed on TV screen with resolution 720p 
by command
cat logo.fb > /dev/fb0
Destination image will be contain initial "logo.png" centered vertically and
horisontally, and free space will be filled green.
Example 2:
suppose we have logo.png 1920x1080. Then the same command will create
"logo.fb" for 720p-resoluted framebuffer with a vertically and horisontally
centered part of initial image. No scale and background color of course will
be ignored.
License: GPLv3

