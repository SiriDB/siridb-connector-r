/*
 * req.h
 *
 *  Created on: May 04, 2017
 *      Author: Jeroen van der Heijden <jeroen@transceptor.technology>
 */

#ifndef SIRIDB_REQ_H_
#define SIRIDB_REQ_H_

#include <inttypes.h>
#include "siridb.h"

/* type definitions */
typedef struct siridb_s siridb_t;
typedef struct siridb_req_s siridb_req_t;
typedef void (*siridb_cb) (siridb_req_t * req);

/* public functions */
#ifdef __cplusplus
extern "C" {
#endif

siridb_req_t * siridb_req_create(siridb_t * siridb, siridb_cb cb, int * rc);
void siridb_req_destroy(siridb_req_t * req);
void siridb_req_cancel(siridb_req_t * req);

#ifdef __cplusplus
}
#endif

/* struct definitions */
struct siridb_req_s
{
    void * data;                /* public */
    int status;
    uint16_t pid;
    uint16_t _pad0; // dummy
    siridb_pkg_t * pkg;
    siridb_cb cb;
    siridb_t * siridb;
};

#endif /* SIRIDB_REQ_H_ */
