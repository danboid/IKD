# IKD

IKD is a Uzebox remake of the tank game(s) in Atari's Combat, released September 11 1977.

Combat on the 2600 was one of the first video games I ever played. This is my first C project.

I Kill Death has the same value in Jewish Gematria as the word Combat, 186.

## Building IKD

To build IKD under Debian or Ubuntu Linux you must first have built the uzebox repo and packrom:

```
$ sudo apt install build-essential git avr-libc gcc-avr libsdl2-dev
$ git clone https://github.com/Uzebox/uzebox.git
$ cd uzebox
$ make
$ cd tools/packrom
$ make
```

Then you can build IKD. Clone it into the dir you cloned the Uzebox repo:

```
$ git clone https://github.com/danboid/IKD.git
$ cd IKD/default
$ make
```

If you don't have a real Uzebox to play it on, the recommended emulator is [cuzebox](https://github.com/Jubatian/cuzebox).
