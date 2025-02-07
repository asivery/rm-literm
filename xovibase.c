#define _GNU_SOURCE
#include <dlfcn.h>
#include <stddef.h>
#include <stdio.h>
#include "xovi.h"

void registerQMLTypes();

void _xovi_construct(){
    // Load dependency tree
    Environment->requireExtension("qt-resource-rebuilder", 0, 1, 1);

    // Merge the QT objects into the main tree
    registerQMLTypes();

    // Add the terminal button in xochitl
    qt_resource_rebuilder$qmldiff_add_external_diff(r$qmldiff, "Terminal");
}
