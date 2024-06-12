/*
 * Copyright 2024, UNSW
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#pragma once

#include <stdint.h>

int driver_init(void **maps, uintptr_t *maps_phys, int num_maps, int argc, char **argv);
void driver_notified();
