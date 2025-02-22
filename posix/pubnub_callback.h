/* -*- c-file-style:"stroustrup"; indent-tabs-mode: nil -*- */
#if !defined INC_PUBNUB_POSIX_CALLBACK
#define	INC_PUBNUB_POSIX_CALLBACK

/** @mainpage The POSIX Pubnub client library - callback interface

    This is the "callback" interface of the Pubnub client library for
    POSIX compatible OSes (Linux, BSD...). 

    The "callback" interface has these characteristics:
    - It is not thread safe. 
    - User cannot make any assumptions about the thread on which
    the callback is called. 

    So, she should synchronise access to the Pubnub context from the
    callback and the rest of the code. This can be done via some sync
    mechanism (mutex et al) or simply by not touching the context from
    the "initiator" thread after a transaction is initiated.

    A common method to handle the callback is to simply notify the
    thread that initiated the transaction (via some message queue
    facility) and not do any processing in the callback itself. Of
    course, it is not always the right one.

    You can have multiple pubnub contexts established; in each
    context, at most one Pubnub API call/transaction may be ongoing
    (typically a "publish" or a "subscribe"). You can use more than
    one context in a thread, but, as mentioned above, should not use
    the same context from more than one thread.
    
 */

/** @file pubnub_contiki.h */

#include "pubnub_config.h"

/* -- You should not change anything below this line -- */

#if !defined(PUBNUB_CALLBACK_API)
#error You have to define PUBNUB_CALLBACK_API
#endif

#include "pubnub_alloc.h"
#include "pubnub_assert.h"
#include "pubnub_coreapi.h"
#include "pubnub_ntf_callback.h"


#endif /* !defined INC_PUBNUB_CALLBACK */
