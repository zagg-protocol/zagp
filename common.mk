# Hand-written file with variables common to all makefiles

# AM_CPPFLAGS = -DSQLITE_OMIT_LOAD_EXTENSION=1
AM_CPPFLAGS = -isystem "$(top_srcdir)" -I"$(top_srcdir)/src" -I"$(top_builddir)/src"
AM_CPPFLAGS += $(libsodium_CFLAGS) $(xdrpp_CFLAGS) $(libmedida_CFLAGS)	\
	$(soci_CFLAGS) $(sqlite3_CFLAGS) $(libasio_CFLAGS) 
	
#$(libsecp256k1_LIBS) $(libbitcoin_crypto_base_LIBS) 

# $(libunivalue_CFLAGS)

AM_CPPFLAGS += -isystem "$(top_srcdir)/lib"			\
	-isystem "$(top_srcdir)/lib/autocheck/include"		\
	-isystem "$(top_srcdir)/lib/cereal/include"		\
	-isystem "$(top_srcdir)/lib/util"			\
	-isystem "$(top_srcdir)/lib/soci/src/core"	\
	-isystem "$(top_srcdir)/lib/bitcoin/src" \
	-isystem "$(top_srcdir)/lib/bitcoin/src/univalue/include" \
	-isystem "$(top_srcdir)/lib/bitcoin/src/secp256k1/include" \
	-isystem "$(top_srcdir)/lib/bitcoin/src/crypto"

	
#  
# -isystem "/usr/include/boost/"
# -isystem "$(top_srcdir)/lib/bitcoin/src/univalue/include"

if USE_POSTGRES
AM_CPPFLAGS += -DUSE_POSTGRES=1 $(libpq_CFLAGS)
endif # USE_POSTGRES
