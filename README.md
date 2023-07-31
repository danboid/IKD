# IKD

IKD is a Uzebox remake of the tank game in Atari's Combat, the original 1977 Atari 2600 pack-in game.

Combat on the Atari 2600 was one of the first video games I ever played. This is my first C project.

Massive thanks to Lee Weber for his fantastic rendition of CAPCOM'S Commando theme, largely inspired by
Rob Hubbard's legendary C64 SID arrangement.

Lee documented some of the techniques that he used to create the Uzebox sound engine patches
used in the IKD menu music on the [Uzebox forum.](https://uzebox.org/forums/viewtopic.php?t=11281)

## Downloading IKD

You can download a IKD **.uze** file from its [Uzebox forum thread](https://uzebox.org/forums/viewtopic.php?t=11136) or the [Uzebox wiki IKD page](https://uzebox.org/wiki/index.php?title=IKD).

## Building IKD

To build IKD under Debian or Ubuntu Linux you must first build the Uzebox repo, **packrom** and **midiconv**:

```
sudo apt install build-essential git avr-libc gcc-avr libsdl2-dev
git clone https://github.com/Uzebox/uzebox.git
cd uzebox/demos
git clone https://github.com/danboid/IKD
cd ../tools/packrom/
make
cd ../midiconv/
make
cd ../../demos/IKD/default/
make
```


If you don't have a real Uzebox to play it on, the recommended emulator is [cuzebox](https://github.com/Jubatian/cuzebox).

## Controls

**A** - shoot

**B** or **UP** - move forward

**LEFT** or **RIGHT** - rotate tank

**POWER** - hyper space (both tanks)
