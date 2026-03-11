#include "internal/dynarr.h"

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

int ensure_space(_dynarr *arr);
bool has_space(_dynarr *arr);

struct _dynarr {
    void *buf;

    size_t buf_size;

    size_t el_count;
    size_t el_size;
    size_t growth_factor;
};

int _dynarr_create(size_t el_size, size_t initial_size, size_t growth_factor, _dynarr **out_arr) {
    if (out_arr == NULL) {
        return -1;
    }

    _dynarr *arr = calloc(1, sizeof *arr);
    if (!arr) {
        return -1;
    }

    arr->el_size = el_size;
    arr->growth_factor = growth_factor > 0 ? growth_factor : 2;

    if (initial_size > 0) {
        arr->buf = calloc(1, initial_size);
        arr->buf_size = initial_size;
    }

    *out_arr = arr;
    return 0;
}

int _dynarr_push(_dynarr *arr, const void *el) {
    int result;
    if((result = ensure_space(arr)) != 0) {
        return result;
    }

    void *ptr = (char*)arr->buf + (arr->el_size * arr->el_count);

    memcpy(ptr, el, arr->el_size);

    arr->el_count++;

    return 0;
}


int _dynarr_at(const _dynarr *arr, size_t idx, void **out_el) {
    if (out_el == NULL) {
        return -1;
    }
    void *el = (char*)arr->buf + (idx * arr->el_size);

    *out_el = el;
    return 0;
}

int _dynarr_remove(_dynarr *arr, size_t idx) {
    assert(arr);
    assert(idx < arr->el_count);

    if (idx != arr->el_count - 1) {
        void *dest = (char*)arr->buf + (arr->el_size * idx);
        void *src = (char*)arr->buf + (arr->el_size * (arr->el_count - 1));
        memcpy(dest, src, arr->el_size);
    }

    arr->el_count--;

    return 0;
}

void _dynarr_destroy(_dynarr *arr) {
    if (arr == NULL) {
        return;
    }

    if (arr->buf != NULL) {
        free(arr->buf);
    }

    free(arr);
    return;
}

void *_dynarr_get_as_arr(_dynarr *arr) {
    return arr->buf;
}

size_t _dynarr_count(const _dynarr *arr) {
    return arr->el_count;
}

int ensure_space(_dynarr *arr) {
    if (has_space(arr)) return 0;

    size_t extra_size = arr->el_count > 0
                        ? arr->el_size * arr->el_count * arr->growth_factor
                        : arr->el_size * arr->growth_factor;

    void *temp = realloc(arr->buf, arr->buf_size + extra_size);
    if(!temp) {
        return -1;
    }

    arr->buf = temp;
    arr->buf_size += extra_size;

    return 0;
}

bool has_space(_dynarr *arr) {
    size_t buf_used = arr->el_count * arr->el_size;
    return arr->el_size + buf_used <= arr->buf_size;
}
