#include <cdio/cdio.h>
#include <cdio/audio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "qripcddb.h"

#define program_name	"qrip"

/*!
   Returns the sum of the decimal digits in a number. Eg. 1955 = 20
*/
static int
cddb_dec_digit_sum(int n)
{
  int ret=0;
  
  for (;;) {
    ret += n%10;
    n    = n/10;
    if (!n) return ret;
  }
}

/*!
   Compute the CDDB disk ID for an Audio disk.  This is a funny checksum
   consisting of the concatenation of 3 things:
      the sum of the decimal digits of sizes of all tracks, 
      the total length of the disk, and 
      the number of tracks.
*/
u_int32_t
cddb_discid(CdIo_t *p_cdio, track_t i_tracks)
{
  int i,t,n=0;
  msf_t start_msf;
  msf_t msf;
  
  for (i = 1; i <= i_tracks; i++) {
    cdio_get_track_msf(p_cdio, i, &msf);
    n += cddb_dec_digit_sum(cdio_audio_get_msf_seconds(&msf));
  }

  cdio_get_track_msf(p_cdio, 1, &start_msf);
  cdio_get_track_msf(p_cdio, CDIO_CDROM_LEADOUT_TRACK, &msf);
  
  t = cdio_audio_get_msf_seconds(&msf)-cdio_audio_get_msf_seconds(&start_msf);
  
  return ((n % 0xff) << 24 | t << 8 | i_tracks);
}

bool 
init_cddb(
		 CdIo_t *p_cdio,
		cddb_conn_t **pp_conn,
		cddb_disc_t **pp_cddb_disc, 
		error_fn_t errmsg,
		track_t i_first_track,
		track_t i_tracks,
		int *i_cddb_matches
	   )
{
  track_t i;
  cddb_opts_t qripcddb_opts;
  qripcddb_opts.email = NULL;
  qripcddb_opts.server = NULL;
  qripcddb_opts.http = 1;	// http ou proxy
  qripcddb_opts.timeout = 30;
  qripcddb_opts.disable_cache = false;
  qripcddb_opts.cachedir = NULL;
  
  *pp_conn =  cddb_new();
  
  cddb_set_charset(*pp_conn, "UTF-8");

  char *prox = getenv(ENV_HTTP_PROXY);
  if (prox == NULL)
  {
	// "environment variable 'http_proxy' not set"
    qripcddb_opts.port = 80;
    if ( init_protocol( pp_conn, "http" ) < 0 )
    	return false;
  }
  else
  {
	// "environment variable 'http_proxy' IS set"
    qripcddb_opts.port = 0;
    if ( init_protocol( pp_conn, "proxy" ) < 0 )
    	return false;
  }

  *pp_cddb_disc = NULL;
  
  if (!*pp_conn) {
    fprintf(stderr,"unable to initialize libcddb");
    return false;
  }
  
  if (NULL == qripcddb_opts.email) 
    cddb_set_email_address(*pp_conn, "me@home");
  else 
    cddb_set_email_address(*pp_conn, qripcddb_opts.email);
  
  if (NULL == qripcddb_opts.server) 
    cddb_set_server_name(*pp_conn, "freedb.freedb.org");
  else 
    cddb_set_server_name(*pp_conn, qripcddb_opts.server);
  
  if (qripcddb_opts.timeout >= 0) 
    cddb_set_timeout(*pp_conn, qripcddb_opts.timeout);
  
  cddb_set_server_port(*pp_conn, qripcddb_opts.port);
  
  if (qripcddb_opts.http) 
    cddb_http_enable(*pp_conn);
  else 
    cddb_http_disable(*pp_conn);
  
  if (NULL != qripcddb_opts.cachedir) 
    cddb_cache_set_dir(*pp_conn, qripcddb_opts.cachedir);
  
  if (qripcddb_opts.disable_cache) 
    cddb_cache_disable(*pp_conn);
  
  *pp_cddb_disc = cddb_disc_new();
  if (!*pp_cddb_disc) {
    fprintf(stderr,"unable to create CDDB disc structure");
    cddb_destroy(*pp_conn);
    return false;
  }
  for(i = 0; i < i_tracks; i++) {
    cddb_track_t *t = cddb_track_new(); 
    cddb_track_set_frame_offset(t, 
				cdio_get_track_lba(p_cdio, i+i_first_track));
    cddb_disc_add_track(*pp_cddb_disc, t);
  }
  
  cddb_disc_set_length(*pp_cddb_disc, 
		       cdio_get_track_lba(p_cdio, CDIO_CDROM_LEADOUT_TRACK) 
		       / CDIO_CD_FRAMES_PER_SEC);
  
  if (!cddb_disc_calc_discid(*pp_cddb_disc)) {
    fprintf(stderr,"libcddb calc discid failed.");
    cddb_destroy(*pp_conn);
    return false;
  }
  
  *i_cddb_matches = cddb_query(*pp_conn, *pp_cddb_disc);
  
  if (-1 == *i_cddb_matches) 
    fprintf(stderr,cddb_error_str(cddb_errno(*pp_conn)));

  cddb_read(*pp_conn, *pp_cddb_disc);
  return true;
}



void cddb_errmsg(const char *msg)
{
	fprintf(stderr, "%s: %s\n", program_name, msg);
}

int init_protocol(cddb_conn_t **conn, const char *proto)
{
    int len, port;
    char *aux, *host, *portstr, *password;

    if (strcmp(proto, "cddbp") == 0) {
        /* Enable the CDDBP protocol, i.e. disable the usage of HTTP.
           This is the default so actually this function call is not
           needed. */
        cddb_http_disable(*conn);
    } else if (strcmp(proto, "http") == 0) {
        /* Enable the HTTP protocol.  We will also set the server port
           to 80, i.e. the default HTTP port. */
        cddb_http_enable(*conn);
        cddb_set_server_port(*conn, 80);
    } else if (strcmp(proto, "proxy") == 0) {
        /* Enable the HTTP protocol through a proxy server.  Enabling
           the proxy will automatically enable HTTP.  We will also set
           the server port to 80, i.e. the default HTTP port.  */
        cddb_http_proxy_enable(*conn);
        cddb_set_server_port(*conn, 80);
        /* We will retrieve the proxy settings from the environment
           variable 'http_proxy'.  If these do not exist, an error
           will be signaled. */
        aux = getenv(ENV_HTTP_PROXY);
        if (aux == NULL) {
            // "environment variable 'http_proxy' not set"
			return -1;
        }
        /* Check the prefix.  It should be 'http://'. */
        if (strncmp(aux, HTTP_PREFIX, HTTP_PREFIX_LEN) != 0) {
            // "environment variable 'http_proxy' invalid"
			return -2;
}
        host = aux + HTTP_PREFIX_LEN;
        /* Check if a proxy username:password pair is provided */
        aux = strchr(host, '@');
        if (aux != NULL) {
            *aux = '\0';
            password = strchr(host, ':');
            if (password != NULL) {
                *password = '\0';
                password++;
            }
            cddb_set_http_proxy_credentials(*conn, host, password);
            host = aux + 1;
        }
        /* Check if a proxy port is specified. */
        portstr = strchr(host, ':');
        if (portstr == NULL) {
            /* No port, set proxy server name and use default port 80. */
            cddb_set_http_proxy_server_name(*conn, host);
            cddb_set_http_proxy_server_port(*conn, 80);
        } else {
            /* A proxy port is present.  Parse it and initialize the
               connection structure with it. */
            portstr++;          /*  Skip colon. */
            port = strtol(portstr, &aux, 10);
            if (*aux != '\0' && *aux != '/') {
                // "environment variable 'http_proxy' port invalid"
				return -3;
            }
            len = portstr - host - 1;
            aux = (char *)malloc(len + 1);
            strncpy(aux, host, len);
            aux[len] = '\0';
            cddb_set_http_proxy_server_name(*conn, aux);
            cddb_set_http_proxy_server_port(*conn, port);
        }

	  } else {
      // "Invalid server protocol '%s'", proto
		  return -4;
    }
	return 0;
}
