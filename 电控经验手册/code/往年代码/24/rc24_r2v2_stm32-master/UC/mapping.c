//
// Created by tony on 23-10-24.
//

#include "mapping.h"
#include "stdlib.h"
#define FLOAT_MIN (-3.40E+38f)
#define FLOAT_MAX 3.40E+38f

void mapping_param_init(mapping_param_t *this, float i_a, float i_b, float i_offset, float o_a, float o_b, float o_offset)
{
    this->min_i = FLOAT_MIN;
    this->max_i = FLOAT_MAX;
    this->min_o = FLOAT_MIN;
    this->max_o = FLOAT_MAX;
    this->k_i2o = (float)(o_b - o_a) / (i_b - i_a);
    this->b_i2o = o_offset - (float )(this->k_i2o * i_offset);
    this->k_o2i = (float)(i_b - i_a) / (o_b - o_a);
    this->b_o2i = i_offset - (float )(this->k_o2i * o_offset);
}

void mapping_limit_o(mapping_param_t *this, float min_o, float max_o)
{
    this->min_o = min_o;
    this->max_o = max_o;
}
void mapping_limit_i(mapping_param_t *this, float min_i, float max_i)
{
    this->min_i = min_i;
    this->max_i = max_i;
}
float mapping_i2o(mapping_param_t *this,float val_i)
{
    float output_o = this->k_i2o * val_i + this->b_i2o;
    if (this->max_o != FLOAT_MAX && output_o > this->max_o)
        output_o = this->max_o;
    else if (this->min_o != FLOAT_MIN && output_o < this->min_o)
        output_o = this->min_o;
    //printf("k_i2o:%f,b_i2o:%d,output_o:%d\n", this->k_i2o, this->b_i2o, output_o);
    //this->pos_out = output_o;
    return output_o;
}
float mapping_o2i(mapping_param_t *this,float val_o)
{
    float output_i = this->k_o2i * val_o + this->b_o2i;
    if (this->max_i != FLOAT_MAX && output_i > this->max_i)
        output_i = this->max_i;
    else if (this->min_i != FLOAT_MIN && output_i < this->min_i)
        output_i = this->min_i;
    //printf("k_o2i:%f,b_o2i:%d,output_i:%d\n", this->k_o2i, this->b_o2i, output_i);
    //this->pos_out = output_o;
    return output_i;
}