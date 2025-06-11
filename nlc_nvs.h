#include "llc_string.h"

#ifndef NLC_NVS_H
#define NLC_NVS_H

namespace nlc
{
    using namespace llc;
    error_t nvsPreferenceSave   (vcst_c namespce, vcst_c key, u2_t value)                         ;
    error_t nvsPreferenceSave   (vcst_c namespce, vcst_c key, u2_t value)                         ;
    error_t nvsPreferenceSave   (vcst_c namespce, vcst_c key, u3_t value)                         ;
    error_t nvsPreferenceSave   (vcst_c namespce, vcst_c key, f2_t value)                         ;
    error_t nvsPreferenceSave   (vcst_c namespce, vcst_c key, f3_t value)                         ;
    error_t nvsPreferenceLoad   (vcst_c namespce, vcst_c key, u2_t & value, u2_t defaultValue)    ;
    error_t nvsPreferenceLoad   (vcst_c namespce, vcst_c key, u3_t & value, u3_t defaultValue)    ;
    error_t nvsPreferenceLoad   (vcst_c namespce, vcst_c key, f2_t & value, f2_t defaultValue)    ;
    error_t nvsPreferenceLoad   (vcst_c namespce, vcst_c key, f3_t & value, f3_t defaultValue)    ;

    error_t nvsPreferenceSave   (vcst_c namespce, vcst_c key, vcsc_c value)                  ;
    error_t nvsPreferenceSave   (vcst_c namespce, vcst_c key, vcu0_c value)                  ;
    error_t nvsPreferenceLoad   (vcst_c namespce, vcst_c key, asc_t   & value, vcsc_c defaultValue);
    error_t nvsPreferenceLoad   (vcst_c namespce, vcst_c key, au0_t   & value, vcu0_c defaultValue);
    error_t nvsPreferenceLoad   (vcst_c namespce, vcst_c key, string  & value, vcst_c defaultValue);
} // namespace 

#endif // SAI_NVS_H
