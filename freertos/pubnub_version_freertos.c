/* -*- c-file-style:"stroustrup"; indent-tabs-mode: nil -*- */
#include "pubnub_version.h"

#define PUBNUB_SDK_NAME "FreeRTOS"
#define PUBNUB_SDK_VERSION "8.2.1"


char const *pubnub_sdk_name(void)
{
    return PUBNUB_SDK_NAME;
}


char const *pubnub_version(void)
{
    return PUBNUB_SDK_VERSION;
}


char const *pubnub_uname(void)
{
    return PUBNUB_SDK_NAME "%2F" PUBNUB_SDK_VERSION;
}

