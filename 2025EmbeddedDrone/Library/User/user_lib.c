#include "user_lib.h"
#include "arm_math.h"

/**
 * @brief:       invSqrt: 快速开方函数
 * @param[in]:   num: 输入的浮点数
 * @retval:      返回输入数的平方根的倒数
 * @details:     使用快速开方算法（如魔术数法）计算浮点数的平方根倒数。
 */
fp32 invSqrt(fp32 num)
{
    fp32 halfnum = 0.5f * num;
    fp32 y = num;
    long i = *(long *)&y;
    i = 0x5f3759df - (i >> 1);
    y = *(fp32 *)&i;
    y = y * (1.5f - (halfnum * y * y));
    return y;
}

/**
 * @brief:       ramp_init: 斜波函数初始化
 * @param[in]:   ramp_source_type: 斜波函数结构体
 * @param[in]:   frame_period: 间隔时间（单位：秒）
 * @param[in]:   max: 最大值
 * @param[in]:   min: 最小值
 * @retval:      返回空
 * @details:     初始化斜波函数的结构体，设置斜波的时间间隔、最大值、最小值以及初始值。
 */
void ramp_init(ramp_function_source_t *ramp_source_type, fp32 frame_period, fp32 max, fp32 min)
{
    ramp_source_type->frame_period = frame_period;
    ramp_source_type->max_value = max;
    ramp_source_type->min_value = min;
    ramp_source_type->input = 0.0f;
    ramp_source_type->out = 0.0f;
}

/**
 * @brief:       ramp_calc: 斜波函数计算
 * @param[in]:   ramp_source_type: 斜波函数结构体
 * @param[in]:   input: 输入值（单位：/s，即每秒增加的值）
 * @retval:      返回空
 * @details:     根据输入的值进行叠加，计算当前斜波函数的输出值，结果在指定的最大值和最小值之间。
 */
void ramp_calc(ramp_function_source_t *ramp_source_type, fp32 input)
{
    ramp_source_type->input = input;
    ramp_source_type->out += ramp_source_type->input * ramp_source_type->frame_period;
    if (ramp_source_type->out > ramp_source_type->max_value)
    {
        ramp_source_type->out = ramp_source_type->max_value;
    }
    else if (ramp_source_type->out < ramp_source_type->min_value)
    {
        ramp_source_type->out = ramp_source_type->min_value;
    }
}

/**
 * @brief:       first_order_filter_init: 一阶低通滤波初始化
 * @param[in]:   first_order_filter_type: 一阶低通滤波结构体
 * @param[in]:   frame_period: 间隔时间（单位：秒）
 * @param[in]:   num: 滤波参数
 * @retval:      返回空
 * @details:     初始化一阶低通滤波结构体，设置滤波的时间间隔和滤波参数。
 */
void first_order_filter_init(first_order_filter_type_t *first_order_filter_type, fp32 frame_period, const fp32 num[1])
{
    first_order_filter_type->frame_period = frame_period;
    first_order_filter_type->num[0] = num[0];
    first_order_filter_type->input = 0.0f;
    first_order_filter_type->out = 0.0f;
}

/**
 * @brief:       first_order_filter_cali: 一阶低通滤波计算
 * @param[in]:   first_order_filter_type: 一阶低通滤波结构体
 * @param[in]:   input: 输入值（单位：秒）
 * @retval:      返回空
 * @details:     对输入值进行一阶低通滤波处理，输出滤波后的结果。
 */
void first_order_filter_cali(first_order_filter_type_t *first_order_filter_type, fp32 input)
{
    first_order_filter_type->input = input;
    first_order_filter_type->out =
        first_order_filter_type->num[0] / (first_order_filter_type->num[0] +
        first_order_filter_type->frame_period) * first_order_filter_type->out +
        first_order_filter_type->frame_period / (first_order_filter_type->num[0] +
        first_order_filter_type->frame_period) * first_order_filter_type->input;
}

/**
 * @brief:       abs_limit: 绝对限制
 * @param[in/out]: num: 输入/输出值，经过限制后返回
 * @param[in]:   Limit: 限制值
 * @retval:      返回空
 * @details:     对输入值进行绝对值限制，将其限制在指定的范围内。
 */
void abs_limit(fp32 *num, fp32 Limit)
{
    if (*num > Limit)
    {
        *num = Limit;
    }
    else if (*num < -Limit)
    {
        *num = -Limit;
    }
}

/**
 * @brief:       sign: 判断符号位
 * @param[in]:   value: 输入值
 * @retval:      返回符号：正数返回 1，负数返回 -1，零返回 1
 * @details:     判断输入值的符号位，返回 1 或 -1。
 */
fp32 sign(fp32 value)
{
    if (value >= 0.0f)
    {
        return 1.0f;
    }
    else
    {
        return -1.0f;
    }
}

/**
 * @brief:       fp32_deadline: 浮点数死区
 * @param[in]:   Value: 输入的浮点数
 * @param[in]:   minValue: 死区下限
 * @param[in]:   maxValue: 死区上限
 * @retval:      返回处理后的值，若输入值在死区内，返回 0
 * @details:     对输入的浮点数进行死区处理，如果其在指定范围内，则返回 0。
 */
fp32 fp32_deadline(fp32 Value, fp32 minValue, fp32 maxValue)
{
    if (Value < maxValue && Value > minValue)
    {
        Value = 0.0f;
    }
    return Value;
}

/**
 * @brief:       int16_deadline: int16型整数死区
 * @param[in]:   Value: 输入的整数
 * @param[in]:   minValue: 死区下限
 * @param[in]:   maxValue: 死区上限
 * @retval:      返回处理后的整数，若输入值在死区内，返回 0
 * @details:     对输入的整数进行死区处理，如果其在指定范围内，则返回 0。
 */
int16_t int16_deadline(int16_t Value, int16_t minValue, int16_t maxValue)
{
    if (Value < maxValue && Value > minValue)
    {
        Value = 0;
    }
    return Value;
}

/**
 * @brief:       fp32_constrain: 限幅函数（浮点型）
 * @param[in]:   Value: 输入值
 * @param[in]:   minValue: 最小值
 * @param[in]:   maxValue: 最大值
 * @retval:      限幅后的结果
 * @details:     对输入值进行限幅处理，限制在指定的最小值和最大值之间。
 */
fp32 fp32_constrain(fp32 Value, fp32 minValue, fp32 maxValue)
{
    if (Value < minValue)
        return minValue;
    else if (Value > maxValue)
        return maxValue;
    else
        return Value;
}

/**
 * @brief:       int16_constrain: 限幅函数（整数型）
 * @param[in]:   Value: 输入值
 * @param[in]:   minValue: 最小值
 * @param[in]:   maxValue: 最大值
 * @retval:      限幅后的结果
 * @details:     对输入的整数值进行限幅处理，限制在指定的最小值和最大值之间。
 */
int16_t int16_constrain(int16_t Value, int16_t minValue, int16_t maxValue)
{
    if (Value < minValue)
        return minValue;
    else if (Value > maxValue)
        return maxValue;
    else
        return Value;
}

/**
 * @brief:       loop_fp32_constrain: 循环限幅函数（浮点型）
 * @param[in]:   Input: 输入值
 * @param[in]:   minValue: 最小值
 * @param[in]:   maxValue: 最大值
 * @retval:      限幅后的结果
 * @details:     对输入的浮点数进行循环限幅，即使输入超出范围，也会循环回到指定范围内。
 */
fp32 loop_fp32_constrain(fp32 Input, fp32 minValue, fp32 maxValue)
{
    if (maxValue < minValue)
    {
        return Input;
    }

    if (Input > maxValue)
    {
        fp32 len = maxValue - minValue;
        while (Input > maxValue)
        {
            Input -= len;
        }
    }
    else if (Input < minValue)
    {
        fp32 len = maxValue - minValue;
        while (Input < minValue)
        {
            Input += len;
        }
    }
    return Input;
}

/**
 * @brief:       theta_format: 角度格式化为 -180 ~ 180
 * @param[in]:   Ang: 输入角度
 * @retval:      格式化后的角度
 * @details:     将输入角度格式化为 -180 到 180 度之间。
 */
fp32 theta_format(fp32 Ang)
{
    return loop_fp32_constrain(Ang, -180.0f, 180.0f);
}

/**
 * @brief:       rad_format: 角度格式化为 -180 ~ 180
 * @param[in]:   Ang: 输入角度
 * @retval:      格式化后的角度
 * @details:     将输入角度格式化为 -180 到 180 度之间。
 */
fp32 rad_format(fp32 Ang)
{
    return loop_fp32_constrain(Ang, -PI, PI);
}

/**
 * @brief:       float_to_uint: 浮点数转换为无符号整数函数
 * @param[in]:   x_float:待转换的浮点数
 * @param[in]:   x_min:范围最小值
 * @param[in]:   x_max:范围最大值
 * @param[in]:   bits:目标无符号整数的位数
 * @retval:      无符号整数结果
 * @details:     将给定的浮点数 x 在指定范围 [x_min, x_max] 内进行线性映射，映射结果为一个指定位数的无符号整数。
 */
int float_to_uint(float x_float, float x_min, float x_max, int bits)
{
    /* Converts a float to an unsigned int, given range and number of bits */
    float span = x_max - x_min;
    float offset = x_min;
    return (int) ((x_float-offset)*((float)((1<<bits)-1))/span);
}

/**
 * @brief:       uint_to_float: 无符号整数转换为浮点数函数
 * @param[in]:   x_int: 待转换的无符号整数
 * @param[in]:   x_min: 范围最小值
 * @param[in]:   x_max: 范围最大值
 * @param[in]:   bits:  无符号整数的位数
 * @retval:      浮点数结果
 * @details:     将给定的无符号整数 x_int 在指定范围 [x_min, x_max] 内进行线性映射，映射结果为一个浮点数。
 */
float uint_to_float(int x_int, float x_min, float x_max, int bits)
{
    /* converts unsigned int to float, given range and number of bits */
    float span = x_max - x_min;
    float offset = x_min;
    return ((float)x_int)*span/((float)((1<<bits)-1)) + offset;
}

int16_t int16_abs(int16_t val)
{
    if (val < 0){
        return -val;
    }
    else return val;
}

