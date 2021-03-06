/**
 * @file config.h
 * @author Michal Vasko <mvasko@cesnet.cz>
 * @brief Netopeer compilation configuration
 *
 * Copyright (C) 2015 CESNET, z.s.p.o.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name of the Company nor the names of its contributors
 *    may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * ALTERNATIVELY, provided that this notice is retained in full, this
 * product may be distributed under the terms of the GNU General Public
 * License (GPL) version 2 or later, in which case the provisions
 * of the GPL apply INSTEAD OF those given above.
 *
 * This software is provided ``as is, and any express or implied
 * warranties, including, but not limited to, the implied warranties of
 * merchantability and fitness for a particular purpose are disclaimed.
 * In no event shall the company or contributors be liable for any
 * direct, indirect, incidental, special, exemplary, or consequential
 * damages (including, but not limited to, procurement of substitute
 * goods or services; loss of use, data, or profits; or business
 * interruption) however caused and on any theory of liability, whether
 * in contract, strict liability, or tort (including negligence or
 * otherwise) arising in any way out of the use of this software, even
 * if advised of the possibility of such damage.
 */

#ifndef _CONFIG_H_
#define _CONFIG_H_

#ifdef __GNUC__
#	define UNUSED(x) UNUSED_ ## x __attribute__((__unused__))
#else
#	define UNUSED(x) UNUSED_ ## x
#endif

#ifdef NP_SSH
#	include <libnetconf_ssh.h>
#	include "./ssh/server_ssh.h"
#	include "./ssh/cfgnetopeer_transapi_ssh.h"
#	include "./ssh/netconf_server_transapi_ssh.h"
#endif

#ifdef NP_TLS
#	include <libnetconf_tls.h>
#	include "./tls/server_tls.h"
#	include "./tls/cfgnetopeer_transapi_tls.h"
#	include "./tls/netconf_server_transapi_tls.h"
#endif

#ifndef MODULES_CFG_DIR
#	define MODULES_CFG_DIR "/etc/netopeer/modules.conf.d/"
#endif

/* maximal value from the sizes of specific client implementations */
#define CLIENT_STRUCT_MAX_SIZE sizeof(struct client_struct_ssh)

/* the initial size of the reading buffer */
#define BASE_READ_BUFFER_SIZE 2048

/* sleeping before retrying non-blocking reads */
#define READ_SLEEP 100

/* end tags of NETCONF messages */
#define NC_V10_END_MSG "]]>]]>"
#define NC_V11_END_MSG "\n##\n"

/* environment variable with verbose level */
#define ENVIRONMENT_VERBOSE "NETOPEER_VERBOSE"

/* names of the 2 base netopeer static transapi modules */
#define NETOPEER_MODULE_NAME "Netopeer"
#define NCSERVER_MODULE_NAME "NETCONF-server"

/* every number-of-secs will the last sent or received data timestamp be checked */
#define CALLHOME_PERIODIC_LINGER_CHECK 5

#endif /* _CONFIG_H_ */
