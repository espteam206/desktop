#pragma once

#include <cstdint>

#define CEMENT_TYPES\
    T(CEMENT_TYPE_ONE, "Type one")\
    T(CEMENT_TYPE_TWO, "Type two")\
    T(CEMENT_TYPE_THREE, "Type three")\
    T(CEMENT_TYPE_FOUR, "Type four")

#define TRANSPORT_TYPES\
    T(CEMENT_TYPE_TRUCK, "Truck")\
    T(CEMENT_TYPE_TRAIN, "Train")\
    T(CEMENT_TYPE_PLANE, "Plane")\
    T(CEMENT_TYPE_BOAT,  "Boat")

#define T(ENUM, STR) ENUM,
enum CementType : int32_t {
    CEMENT_TYPES
    CEMENT_TYPES_CNT
};
#undef T

#define T(ENUM, STR) STR,
inline const char* CEMENT_STRS[] {
    CEMENT_TYPES
};
#undef T

#define T(ENUM, STR) ENUM,
enum TransportType : int32_t {
    TRANSPORT_TYPES
    TRANSPORT_TYPES_CNT
};
#undef T

#define T(ENUM, STR) STR,
inline const char* TRANSPORT_STRS[] {
    TRANSPORT_TYPES
};
#undef T
