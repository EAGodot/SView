#ifndef COMMON_H
#define COMMON_H
#define MIN(A,B)    (((A)<(B)) ? (A) : (B))
#define MAX(A,B)    (((A)>(B)) ? (A) : (B))

//V限定在给定的范围[VMIN, VMAX]之内V
#define PUT_IN_RANGE(V,VMIN,VMAX) MAX(VMIN,MIN(VMAX,V))
//这个宏定义 MAP_TO_RANGE 的主要作用是将一个值 V 从一个原始的数值范围 [VMIN0, VMAX0] 映射到另一个目标数值范围 [VMIN1, VMAX1] 内。
//它常用于在不同的数值区间进行数据转换、归一化或者适配等操作场景，通过数学计算来实现数值范围的转换
#define MAP_TO_RANGE(V,VMIN0,VMAX0,VMIN1,VMAX1) ( (VMIN1) +  ( (V) - (VMIN0) ) * ( (VMAX1) - (VMIN1) ) / ( (VMAX0) - (VMIN0) ) )



#endif //COMMON_H