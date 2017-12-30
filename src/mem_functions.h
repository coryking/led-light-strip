//
// Created by Cory King on 12/30/17.
//

#ifndef RGBWPLAY_MEM_FUNCTIONS_H
#define RGBWPLAY_MEM_FUNCTIONS_H

template<typename TYPE>
TYPE *reverse_memcpy(TYPE *dst, TYPE *src, size_t items, boolean reverse) {
    if (reverse) {
        for (size_t i = 0; i < items; ++i)
            dst[items - 1 - i] = src[i];
    } else {
        memcpy(dst, src, sizeof(TYPE) * items);
    }

    return dst;

}

#endif //RGBWPLAY_MEM_FUNCTIONS_H
