//
// Created by tony on 23-10-24.
//

#ifndef MAPPING_H
#define MAPPING_H
#ifdef __cplusplus
extern "C" {
#endif
#include "main.h"
typedef struct{
    float k_i2o;
    float b_i2o;
    float k_o2i;
    float b_o2i;
    float min_o;
    float max_o;
    float min_i;
    float max_i;
}mapping_param_t;

void mapping_param_init(mapping_param_t *this, float i_a, float i_b, float i_offset, float o_a, float o_b, float o_offset);
void mapping_limit_o(mapping_param_t *this, float min_o, float max_o);
void mapping_limit_i(mapping_param_t *this, float min_i, float max_i);
float mapping_i2o(mapping_param_t *this,float val_i);
float mapping_o2i(mapping_param_t *this,float val_o);

#ifdef __cplusplus
}
#endif
#endif //MAPPING_H
