/* -*- c-file-style:"stroustrup"; indent-tabs-mode: nil -*- */
#include "pubnub_version.h"

#define PUBNUB_SDK_NAME "Windows"
#define PUBNUB_SDK_VERSION "2.1-alpha"


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

