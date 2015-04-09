#ifndef GALERAROUTER_HG
#define GALERAROUTER_HG
/*
 * This file is distributed as part of the MariaDB Corporation MaxScale.  It is free
 * software: you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation,
 * version 2.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 51
 * Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Copyright MariaDB Corporation Ab 2015
 */

/**
 * @file galerarouter.h - Galera cluster load balancer
 *
 * @verbatim
 * Revision History
 *
 * 08/04/2015	Markus Mäkelä	Initial implementation
 *
 * @endverbatim
 */

#include <skygw_utils.h>
#include <skygw_types.h>
#include <mysql_client_server_protocol.h>
typedef struct galera_instance_t{
  SERVICE* service; /*< Owning service */
  bool safe_reads; /*< If reads should be guaranteed to return up to date data */
}GALERA_INSTANCE;

typedef struct galera_session_t{
  SESSION* session; /*< Owning session */
  slist_cursor_t* nodes; /*< A list of all the connected DCBs */
  DCB* active_node;/*< Active node */
  bool trx_open; /*< Transaction open or not */
  bool autocommit; /*< Autocommit on or off */
  bool closed; /*< If session is ready for routing queries */
/*< Session commands */
}GALERA_SESSION;

#endif
