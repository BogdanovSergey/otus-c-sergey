## installation


`sudo apt install libgtk-4-dev`


## VSCode setup

Insert to includePath paths from cmd:


`pkg-config --cflags-only-I gtk4 | sed -E 's|-I([^ ]+)|"\1/**",\n|g'`


