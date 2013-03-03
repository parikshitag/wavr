#include "shared.h"

/**
 * @param array Array to be searched from.
 * @param size  Size uptil value is searched in array.
 * @param value Value to be searched in array.
 * @return
 */
int wavrHelper::indexOf(const QString array[], int size, const QString &value) {
    for(int index = 0; index < size; index++) {
        if (value == array[index])
            return index;
    }
    return -1;
}
