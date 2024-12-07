#define _GNU_SOURCE
#include <dlfcn.h>
#include <stddef.h>
#include <stdio.h>
void registerQMLTypes();
extern const struct XoViEnvironment {
    char *(*getExtensionDirectory)(const char *family);
    void (*requireExtension)(const char *name, unsigned char major, unsigned char minor, unsigned char patch);
} *Environment;

void _xovi_construct(){
    // Load dependency tree
    Environment->requireExtension("qt-resource-rebuilder", 0, 1, 1);

    // Merge the QT objects into the main tree
    registerQMLTypes();

    // Add the terminal button in xochitl
    qt_resource_rebuilder$qmldiff_add_external_diff(r$qmldiff, "Terminal");
}
