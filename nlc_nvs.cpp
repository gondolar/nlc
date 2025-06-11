#include "nlc_nvs.h"
#include "llc_string_compose.h"

#include <Preferences.h>

using llc::vcst_c, llc::vcsc_c, llc::vcu0_c, llc::string, llc::error_t, llc::function;
LLC_USING_TYPEINT();

static  error_t       withPreferencesNamespace(vcst_c namespce, bool readOnly, const function<error_t(Preferences&)> funcWithPreferences) {
    Preferences preferences = {};
    if_zero_fef(preferences.begin(namespce, readOnly), "namespace:'%s'.", namespce.begin());
    rtrn funcWithPreferences ? funcWithPreferences(preferences) : -1;
}
error_t   nlc::     nvsPreferenceSave       (vcst_c namespce, vcst_c key, u2_t value)                         { rtrn withPreferencesNamespace(namespce, false, [key, value](Preferences & preferences) { info_printf("Saving '%s'->'%u'."   , key.begin(), value); rtrn (sizeof(value)  == preferences.putUInt    (key, value)) ? 0 : -1; }); }
error_t   nlc::     nvsPreferenceSave       (vcst_c namespce, vcst_c key, u3_t value)                         { rtrn withPreferencesNamespace(namespce, false, [key, value](Preferences & preferences) { info_printf("Saving '%s'->'%llu'." , key.begin(), value); rtrn (sizeof(value)  == preferences.putULong64 (key, value)) ? 0 : -1; }); }
error_t   nlc::     nvsPreferenceSave       (vcst_c namespce, vcst_c key, f2_t value)                         { rtrn withPreferencesNamespace(namespce, false, [key, value](Preferences & preferences) { info_printf("Saving '%s'->'%f'."   , key.begin(), value); rtrn (sizeof(value)  == preferences.putFloat   (key, value)) ? 0 : -1; }); }
error_t   nlc::     nvsPreferenceSave       (vcst_c namespce, vcst_c key, f3_t value)                         { rtrn withPreferencesNamespace(namespce, false, [key, value](Preferences & preferences) { info_printf("Saving '%s'->'%lf'."  , key.begin(), value); rtrn (sizeof(value)  == preferences.putDouble  (key, value)) ? 0 : -1; }); }
error_t   nlc::     nvsPreferenceSave       (vcst_c namespce, vcst_c key, vcu0_c value)                       { rtrn withPreferencesNamespace(namespce, false, [key, value](Preferences & preferences) { info_printf("Saving '%s'->'%s'."   , key.begin(), value); rtrn (value.size()   == preferences.putBytes   (key, value.begin(), value.size())) ? 0 : -1; }); }
error_t   nlc::     nvsPreferenceSave       (vcst_c namespce, vcst_c key, vcsc_c value)                       { rtrn withPreferencesNamespace(namespce, false, [key, value](Preferences & preferences) { info_printf("Saving '%s'->'%s'."   , key.begin(), value); rtrn (value.size()   == preferences.putBytes   (key, value.begin(), value.size())) ? 0 : -1; }); }
error_t   nlc::     nvsPreferenceLoad       (vcst_c namespce, vcst_c key, u2_t & value, u2_t defaultValue)    { value = defaultValue; rtrn withPreferencesNamespace(namespce, false , [namespce, key, &value, defaultValue](Preferences & preferences) { value = preferences.getUInt      (key, defaultValue); info_printf("Loading '%s'->'%u'."   , key.begin(), value); rtrn 0; }); }
error_t   nlc::     nvsPreferenceLoad       (vcst_c namespce, vcst_c key, u3_t & value, u3_t defaultValue)    { value = defaultValue; rtrn withPreferencesNamespace(namespce, false , [namespce, key, &value, defaultValue](Preferences & preferences) { value = preferences.getULong64   (key, defaultValue); info_printf("Loading '%s'->'%llu'." , key.begin(), value); rtrn 0; }); }
error_t   nlc::     nvsPreferenceLoad       (vcst_c namespce, vcst_c key, f2_t & value, f2_t defaultValue)    { value = defaultValue; rtrn withPreferencesNamespace(namespce, false , [namespce, key, &value, defaultValue](Preferences & preferences) { value = preferences.getFloat     (key, defaultValue); info_printf("Loading '%s'->'%f'."   , key.begin(), value); rtrn 0; }); }
error_t   nlc::     nvsPreferenceLoad       (vcst_c namespce, vcst_c key, f3_t & value, f3_t defaultValue)    { value = defaultValue; rtrn withPreferencesNamespace(namespce, false , [namespce, key, &value, defaultValue](Preferences & preferences) { value = preferences.getDouble    (key, defaultValue); info_printf("Loading '%s'->'%lf'."  , key.begin(), value); rtrn 0; }); }
tplT sttc error_t   nvsPreferenceLoad       (vcst_c namespce, vcst_c key, llc::apod<T> & value, llc::view<const T> defaultValue) { 
    rtrn withPreferencesNamespace(namespce, false, [namespce, key, &value, defaultValue](Preferences & preferences) {
        u2_c            offset      = value.size();
        u2_c            length      = max((u2_t)preferences.getBytesLength(key), defaultValue.size());
        if_fail_fef(value.resize(offset + length), "key:'%s', offset:%i, length:%i.", key.begin(), offset, length);
        memcpy(&value[offset], defaultValue.begin(), defaultValue.size());
        info_printf("Loading '%s'->'%s'.", key.begin(), value.begin()); 
        if_true_fef(length != preferences.getBytes(key, &value[offset], length), "offset:%i, length:%i.", offset, length);
        if_fail_fe(value.resize(offset + preferences.getBytesLength(key)));
        rtrn 0; 
    });
}
error_t   nlc::       nvsPreferenceLoad       (vcst_c namespce, vcst_c key, asc_t   & value, vcsc_c defaultValue) { rtrn ::nvsPreferenceLoad(namespce, key, value, defaultValue); }
error_t   nlc::       nvsPreferenceLoad       (vcst_c namespce, vcst_c key, au0_t   & value, vcu0_c defaultValue) { rtrn ::nvsPreferenceLoad(namespce, key, value, defaultValue); }
error_t   nlc::       nvsPreferenceLoad       (vcst_c namespce, vcst_c key, string  & value, vcst_c defaultValue) { rtrn ::nvsPreferenceLoad(namespce, key, value, defaultValue); }
