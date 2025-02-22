/* -*- c-file-style:"stroustrup"; indent-tabs-mode: nil -*- */
#include "pbpal.h"

#include "pubnub_ntf_sync.h"
#include "pubnub_netcore.h"
#include "pubnub_internal.h"
#include "pubnub_assert.h"

#include "openssl/ssl.h"


#include <string.h>


#define HTTP_PORT 80


static void buf_setup(pubnub_t *pb)
{
    pb->ptr = (uint8_t*)pb->core.http_buf;
    pb->left = sizeof pb->core.http_buf;
}


static int pal_init(void)
{
    static bool s_init = false;
    if (!s_init) {
        ERR_load_BIO_strings();
        SSL_load_error_strings();
        SSL_library_init();
        OpenSSL_add_all_algorithms();

        pbntf_init();
        s_init = true;
    }
    return 0;
}


void pbpal_init(pubnub_t *pb)
{
    pal_init();
    pb->pal.bio = NULL;
    pb->pal.ctx = NULL;
    pb->use_blocking_io = true;
    pb->ssl.use = pb->ssl.fallback = pb->ssl.ignore = true;
    pb->sock_state = STATE_NONE;
    pb->readlen = 0;
    buf_setup(pb);
}


int pbpal_send(pubnub_t *pb, void *data, size_t n)
{
    if (n == 0) {
        return 0;
    }
    if (pb->sock_state != STATE_NONE) {
        DEBUG_PRINTF("pbpal_send(): pb->sock_state != STATE_NONE (=%d)\n", pb->sock_state);
        return -1;
    }
    pb->sendptr = data;
    pb->sendlen = n;
    pb->sock_state = STATE_NONE;

    return pbpal_sent(pb) ? 0 : +1;
}


int pbpal_send_str(pubnub_t *pb, char *s)
{
    return pbpal_send(pb, s, strlen(s));
}


bool pbpal_sent(pubnub_t *pb)
{
    DEBUG_PRINTF("pbpal_sent(): pb->sendlen = %d\n", pb->sendlen);
    int r = BIO_write(pb->pal.bio, pb->sendptr, pb->sendlen);
    if (r < 0) {
        DEBUG_PRINTF("pbpal_sent(): r = %d\n", r);
        ERR_print_errors_fp(stderr);
        /* Maybe an error should be handled somehow... */
        return false;
    }
    pb->sendptr += r;
    pb->sendlen -= r;
    return r >= pb->sendlen;
}


int pbpal_start_read_line(pubnub_t *pb)
{
    if (pb->sock_state != STATE_NONE) {
        DEBUG_PRINTF("pbpal_start_read_line(): pb->sock_state != STATE_NONE: "); WATCH(pb->sock_state, "%d");
        return -1;
    }

    if (pb->ptr > (uint8_t*)pb->core.http_buf) {
        unsigned distance = pb->ptr - (uint8_t*)pb->core.http_buf;
        memmove(pb->core.http_buf, pb->ptr, pb->readlen);
        pb->ptr -= distance;
        pb->left += distance;
    }
    else {
        if (pb->left == 0) {
            /* Obviously, our buffer is not big enough, maybe some
               error should be reported */
            buf_setup(pb);
        }
    }

    pb->sock_state = STATE_READ_LINE;
    return +1;
}


bool pbpal_line_read(pubnub_t *pb)
{
    uint8_t c;

    DEBUG_PRINTF("pbpal_line_read()\n");
    WATCH(pb->sock_state, "%d");
    if (pb->readlen == 0) {
        int recvres = BIO_read(pb->pal.bio, pb->ptr, pb->left);
        if (recvres <= 0) {
            /* This is error or connection close, which may be handled
               in some way...
             */
            return false;
        }
        DEBUG_PRINTF("have new data of length=%d: %s\n", recvres, pb->ptr);
        pb->sock_state = STATE_READ_LINE;
        pb->readlen = recvres;
    } 

    while (pb->left > 0 && pb->readlen > 0) {
        c = *pb->ptr++;

        --pb->readlen;
        --pb->left;
        
        if (c == '\n') {
            DEBUG_PRINTF("\\n found: "); WATCH(pbpal_read_len(pb), "%d"); WATCH(pb->readlen, "%d");
            pb->sock_state = STATE_NONE;
            return true;
        }
    }

    if (pb->left == 0) {
        /* Buffer has been filled, but new-line char has not been
         * found.  We have to "reset" this "mini-fsm", as otherwise we
         * won't read anything any more. This means that we have lost
         * the current contents of the buffer, which is bad. In some
         * general code, that should be reported, as the caller could
         * save the contents of the buffer somewhere else or simply
         * decide to ignore this line (when it does end eventually).
         */
        pb->sock_state = STATE_NONE;
    }
    else {
        pb->sock_state = STATE_NEWDATA_EXHAUSTED;
    }

    return false;
}


int pbpal_read_len(pubnub_t *pb)
{
    return sizeof pb->core.http_buf - pb->left;
}


int pbpal_start_read(pubnub_t *pb, size_t n)
{
    if (pb->sock_state != STATE_NONE) {
        DEBUG_PRINTF("pbpal_start_read(): pb->sock_state != STATE_NONE: "); WATCH(pb->sock_state, "%d");
        return -1;
    }
    if (pb->ptr > (uint8_t*)pb->core.http_buf) {
        unsigned distance = pb->ptr - (uint8_t*)pb->core.http_buf;
        memmove(pb->core.http_buf, pb->ptr, pb->readlen);
        pb->ptr -= distance;
        pb->left += distance;
    }
    else {
        if (pb->left == 0) {
            /* Obviously, our buffer is not big enough, maybe some
               error should be reported */
            buf_setup(pb);
        }
    }
    pb->sock_state = STATE_READ;
    pb->len = n;
    return +1;
}


bool pbpal_read_over(pubnub_t *pb)
{
    unsigned to_read = 0;
    WATCH(pb->sock_state, "%d");
    WATCH(pb->readlen, "%d");
    WATCH(pb->left, "%d");
    WATCH(pb->len, "%d");

    if (pb->readlen == 0) {
        int recvres;
        to_read =  pb->len - pbpal_read_len(pb);
        if (to_read > pb->left) {
            to_read = pb->left;
        }
        recvres = BIO_read(pb->pal.bio, pb->ptr, to_read);
        if (recvres <= 0) {
            /* This is error or connection close, which may be handled
               in some way...
             */
            return false;
        }
        pb->sock_state = STATE_READ;
        pb->readlen = recvres;
    } 

    pb->ptr += pb->readlen;
    pb->left -= pb->readlen;
    pb->readlen = 0;

    if (pbpal_read_len(pb) >= pb->len) {
        /* If we have read all that was requested, we're done. */
        DEBUG_PRINTF("Read all that was to be read.\n");
        pb->sock_state = STATE_NONE;
        return true;
    }

    if ((pb->left > 0)) {
        pb->sock_state = STATE_NEWDATA_EXHAUSTED;
        return false;
    }

    /* Otherwise, we just filled the buffer, but we return 'true', to
     * enable the user to copy the data from the buffer to some other
     * storage.
     */
    DEBUG_PRINTF("Filled the buffer, but read %d and should %d\n", pbpal_read_len(pb), pb->len);
    pb->sock_state = STATE_NONE;
    return true;
}


bool pbpal_closed(pubnub_t *pb)
{
    return pb->pal.bio == NULL;
}


void pbpal_forget(pubnub_t *pb)
{
    /* a no-op under OpenSSL */
}


void pbpal_close(pubnub_t *pb)
{
    DEBUG_PRINTF("pbpal_close()\n");
    pb->readlen = 0;
    if (pb->pal.bio != NULL) {
        pbntf_lost_socket(pb, (pb_socket_t)pb->pal.bio);
        BIO_free_all(pb->pal.bio);
        pb->pal.bio = NULL;
        if (pb->pal.ctx != NULL) {
            SSL_CTX_free(pb->pal.ctx);
            pb->pal.ctx = NULL;
        }
    }
}


bool pbpal_connected(pubnub_t *pb)
{
    return pb->pal.bio != NULL;
}
