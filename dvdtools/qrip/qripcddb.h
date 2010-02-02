#ifndef _QRIP_CDDB_H
#define _QRIP_CDDB_H

#include <sys/types.h>
#include <cddb/cddb.h>

#define ENV_HTTP_PROXY "http_proxy"
#define HTTP_PREFIX "http://" 
#define HTTP_PREFIX_LEN 7

typedef struct cddb_opts_s 
{
  char          *email;  /* email to report to CDDB server. */
  char          *server; /* CDDB server to contact */
  int            port;   /* port number to contact CDDB server. */
  int            http;   /* 1 if use http proxy */
  int            timeout;
  bool           disable_cache; /* If set the below is meaningless. */
  char          *cachedir;
} cddb_opts_t;


/*!
   Compute the CDDB disk ID for an Audio disk.  This is a funny checksum
   consisting of the concatenation of 3 things:
      the sum of the decimal digits of sizes of all tracks, 
      the total length of the disk, and 
      the number of tracks.
*/
u_int32_t cddb_discid(CdIo_t *p_cdio, track_t i_tracks);

typedef void (*error_fn_t) (const char *msg);

bool init_cddb(
		CdIo_t *p_cdio,
		cddb_conn_t **pp_conn, 
	    cddb_disc_t **pp_cddb_disc,
		error_fn_t errmsg, 
	    track_t i_first_track,
		track_t i_tracks,
		int *i_cddb_matches
		);

void get_cddb_info(CdIo_t *, track_t, track_t );
void cddb_errmsg(const char *);
int init_protocol(cddb_conn_t **, const char *);

#endif
