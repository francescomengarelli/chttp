#ifndef _dynarr_H
#define _dynarr_H

#include <stddef.h>
typedef struct _dynarr _dynarr;

int _dynarr_create(size_t el_size, size_t initial_size, size_t growth_factor, _dynarr **out_arr);

int _dynarr_at(const _dynarr *arr, size_t idx, void **out_el);

int _dynarr_push(_dynarr *arr, const void *el);

int _dynarr_remove(_dynarr *arr, size_t idx);

void *_dynarr_get_as_arr(_dynarr *arr);

void _dynarr_destroy(_dynarr *arr);

size_t _dynarr_count(const _dynarr *arr);
#endif // !_dynarr_H
