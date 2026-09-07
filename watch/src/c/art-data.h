/* Generated from resources/pebble.svg by resources/convert-art.py. */
#pragma once
#include <pebble.h>
typedef struct { const GPoint *points; uint8_t count, paint, stroke; bool closed; } PondPath;
static const GPoint pond_points_0[] = {{34,96},{26,99},{21,102},{20,106},{23,109},{29,112},{38,115},{52,117},{69,119},{83,120},{95,119},{105,117},{113,115},{119,112},{122,109},{123,106},{120,103}};
static const GPoint pond_points_1[] = {{115,97},{110,95}};
static const GPoint pond_points_2[] = {{44,101},{43,103},{43,105},{46,106},{50,108},{56,109},{62,110},{70,110},{78,110},{85,110},{91,109},{96,108},{100,107},{102,105},{103,104},{102,102},{99,101}};
static const GPoint pond_points_3[] = {{37,57},{40,52},{43,47},{47,43},{51,40},{55,37},{60,35},{65,35},{70,35},{94,42},{97,44},{100,46},{102,49},{104,52},{105,56},{105,59},{105,64},{104,68},{96,81},{94,84},{91,87},{87,89},{83,90},{79,91},{74,91},{69,90},{64,89},{44,84},{40,82},{37,80},{34,77},{33,74},{33,70},{33,66},{35,61},{37,57}};
static const GPoint pond_points_4[] = {{37,57},{40,52},{43,47},{47,43},{51,40},{55,37},{60,35},{65,35},{70,35},{94,42},{96,43},{98,44},{100,45},{101,47},{102,48},{103,49},{104,51},{104,52},{96,50},{87,49},{79,49},{70,50},{61,52},{52,56},{44,61},{35,67},{37,57}};
static const GPoint pond_points_5[] = {{96,81},{94,84},{91,87},{87,89},{83,90},{79,91},{74,91},{69,90},{64,89},{44,84},{42,83},{40,82},{38,81},{37,80},{35,78},{34,77},{34,75},{33,73},{41,77},{49,80},{57,82},{66,82},{75,82},{83,80},{92,77},{101,73},{96,81}};
static const GPoint pond_points_6[] = {{55,63},{55,67}};
static const GPoint pond_points_7[] = {{78,60},{78,64}};
static const GPoint pond_points_8[] = {{64,75},{66,76},{67,77},{68,77},{70,77},{72,77},{73,76},{74,75},{76,73}};
static const PondPath pond_paths[] = {
  {pond_points_0, 17, 0, 4, false}, /* Loose-outer-ripple */
  {pond_points_1, 2, 0, 4, false}, /* Loose-outer-ripple */
  {pond_points_2, 17, 1, 4, false}, /* Near-ripple */
  {pond_points_3, 36, 2, 0, true}, /* River-stone */
  {pond_points_4, 27, 1, 0, true}, /* Sunlit-top */
  {pond_points_5, 27, 3, 0, true}, /* Little-shadow */
  {pond_points_6, 2, 4, 4, false}, /* Quiet-eyes */
  {pond_points_7, 2, 4, 4, false}, /* Quiet-eyes */
  {pond_points_8, 9, 4, 3, false}, /* Small-smile */
};
