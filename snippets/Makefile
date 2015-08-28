SOURCEFILES = ../core/pubnub_coreapi.c ../core/pubnub_ccore.c ../core/pubnub_netcore.c  ../lib/sockets/pbpal_sockets.c ../lib/sockets/pbpal_resolv_and_connect_sockets.c ../core/pubnub_alloc_std.c ../core/pubnub_assert_std.c ../core/pubnub_generate_uuid.c ../core/pubnub_blocking_io.c ../core/pubnub_json_parse.c ../core/pubnub_helper.c  ../posix/pubnub_version_posix.c ../posix/pubnub_generate_uuid_posix.c ../posix/pbpal_posix_blocking_io.c

#CFLAGS =-g -D VERBOSE_DEBUG -I ../core -I ../posix -I ../cpp -Wall
CFLAGS =-g -I ../core -I ../posix -I ../cpp -Wall
# -g enables debugging, remove to get a smaller executable

all: clean pubnub_sync

pubnub_sync: pubnub_sync.cpp $(SOURCEFILES) ../core/pubnub_ntf_sync.c ../cpp/pubnub_futres_sync.cpp
	$(CXX) -o pubnub_sync $(CFLAGS) pubnub_sync.cpp ../core/pubnub_ntf_sync.c ../cpp/pubnub_futres_sync.cpp $(SOURCEFILES)
#-D VERBOSE_DEBUG

clean:
	rm pubnub_sync
