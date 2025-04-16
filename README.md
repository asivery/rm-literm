# porting

This version of literm has been ported to work with the rMPP tablet. It uses [xovi](https://github.com/asivery/xovi)
to load itself into xochitl. It depends on the [qt-resource-rebuilder](https://github.com/asivery/rmpp-xovi-extensions/) extension.

All the code responsible for merging into the system UI is stored in `literm.qmd`.

The font I added instead of the system default is [hack](https://github.com/source-foundry/Hack)

To build, run `qmake .; make` after setting up the XOVI_ROOT environmental variable, and sourcing the toolchain's env. file.

Since this project modifies the rM's UI, it is not version agnostic. To update from one version to another, change the values in `literm.qmd`.

This branch is built for `3.16.2.3`. Feel free to try it on other versions, to see if it works.

To change terminal settings, please tap the top-right hand corner of the screen. A toolbar should open.

# installation

To install this on your rMPP:

- Install xovi ([instructions](https://github.com/asivery/rmpp-xovi-extensions/blob/master/INSTALL.MD))
- Download the `extensions.zip` release file from the aforementioned page
- Copy over the `qt-resource-rebuilder.so` file from the zip file into `/home/root/xovi/extensions.d`
- Follow the instructions on how to rebuild the hashtab ([instructions](https://github.com/asivery/rmpp-xovi-extensions/blob/master/INSTALL.MD))
- Download `libliterm.so` from this repository's releases and copy it over to the same directory
- Run `xovi/start` over SSH to restart everything.

I am not responsible for any damages you might end up doing to your device.

# about

literm is a terminal emulator for Linux first and foremost, but it is also
usable elsewhere (on Mac). The design goal is to be simplistic while still
providing a reasonable amount of features.

![Travis CI status](https://travis-ci.org/rburchell/literm.svg?branch=master)

# status

This probably won't eat your homework, but I'd treat it with a dose of caution
all the same. It has seen a fair amount of real world use through fingerterm,
but there may still be bugs lurking.

This having been said, feel free to give it a shot and file bugs - ideally with
pull requests, but at the least with as much information about how to reproduce
them as possible.

# technology

literm is implemented using QML to provide a fast, and fluid user interface.
The terminal emulator side is in C++ (also using Qt). It is exposed as a plugin
to allow reuse in other applications or contexts.

# history

literm started off life as fingerterm, a terminal emulator designed for
touch-based interaction, written by Heikki Holstila for the Nokia N9 and Jolla's
Sailfish OS devices.

I decided to take it a bit further, specifically: giving it a desktop-friendly
interface, adding some odd features here and there, and fixing things up as I
found them.

It is also partly inspired by [Yat, a terminal emulator by Jørgen
Lind](https://github.com/jorgen/yat), but it does not share any code from it.

## differences from fingerterm

Notable changes since forking from fingerterm:

* Separate desktop UI (mobile UI is still intact)
* Rewritten terminal rendering using QtQuick rather than software rendering
* 24bit color support
* Improved parsing performance
* Support for underlined, italic, and blinking text attributes
* Support for bracketed paste mode (used by zsh and others), and other escapes
* Sends cursor scroll escapes when scrolling in applications like vim

