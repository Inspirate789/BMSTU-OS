#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <rpc/pmap_clnt.h>
#include <string.h>
#include <memory.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "bakery.h"

#ifndef SIG_PF
#define SIG_PF void (*)(int)
#endif

struct thr_data
{
	struct svc_req *rqstp;
	SVCXPRT *transp;
};

unsigned long id = 0;
pthread_t p_thread[MAX_CLIENTS_COUNT];
pthread_attr_t attr[MAX_CLIENTS_COUNT];

void *serv_request(void *arg)
{
	struct thr_data *data_ptr = arg;
	struct svc_req *rqstp = data_ptr->rqstp;
	register SVCXPRT *transp = data_ptr->transp;

	union
	{
		struct REQUEST get_number_1_arg;
		struct REQUEST bakery_service_1_arg;
	} argument;
	char *result;
	xdrproc_t _xdr_argument, _xdr_result;
	char *(*local)(char *, struct svc_req *);

	switch (rqstp->rq_proc)
	{
	case NULLPROC:
		(void)svc_sendreply(transp, (xdrproc_t)xdr_void, (char *)NULL);
		pthread_exit(0);

	case GET_NUMBER:
		_xdr_argument = (xdrproc_t)xdr_REQUEST;
		_xdr_result = (xdrproc_t)xdr_int;
		id = (id + 1) % MAX_CLIENTS_COUNT;
		local = (char *(*)(char *, struct svc_req *))get_number_1_svc;
		break;

	case BAKERY_SERVICE:
		_xdr_argument = (xdrproc_t)xdr_REQUEST;
		_xdr_result = (xdrproc_t)xdr_int;
		id = (id + 1) % MAX_CLIENTS_COUNT;
		local = (char *(*)(char *, struct svc_req *))bakery_service_1_svc;
		break;

	default:
		svcerr_noproc(transp);
		pthread_exit(0);
	}
	memset((char *)&argument, 0, sizeof(argument));
	if (!svc_getargs(transp, (xdrproc_t)_xdr_argument, (caddr_t)&argument))
	{
		svcerr_decode(transp);
		pthread_exit(0);
	}
	result = (*local)((char *)&argument, rqstp);
	if (result != NULL && !svc_sendreply(transp, (xdrproc_t)_xdr_result, result))
	{
		svcerr_systemerr(transp);
	}
	if (!svc_freeargs(transp, (xdrproc_t)_xdr_argument, (caddr_t)&argument))
	{
		fprintf(stderr, "%s", "unable to free arguments");
		exit(1);
	}
	
	pthread_exit(0);
}

static void
bakery_prog_1(struct svc_req *rqstp, register SVCXPRT *transp)
{
	struct thr_data *data_ptr = malloc(sizeof(struct thr_data));

	if (!data_ptr)
	{
		fprintf(stderr, "%s", "error (malloc)");
		exit(1);
	}

	data_ptr->rqstp = rqstp;
	data_ptr->transp = transp;

	pthread_attr_setdetachstate(&attr[id], PTHREAD_CREATE_DETACHED);
	pthread_create(&p_thread[id], &attr[id], serv_request, data_ptr);
}

int main(int argc, char **argv)
{
	register SVCXPRT *transp;

	pmap_unset(BAKERY_PROG, BAKERY_VER);

	transp = svcudp_create(RPC_ANYSOCK);
	if (transp == NULL)
	{
		fprintf(stderr, "%s", "cannot create udp service.");
		exit(1);
	}
	if (!svc_register(transp, BAKERY_PROG, BAKERY_VER, bakery_prog_1, IPPROTO_UDP))
	{
		fprintf(stderr, "%s", "unable to register (BAKERY_PROG, BAKERY_VER, udp).");
		exit(1);
	}

	transp = svctcp_create(RPC_ANYSOCK, 0, 0);
	if (transp == NULL)
	{
		fprintf(stderr, "%s", "cannot create tcp service.");
		exit(1);
	}
	if (!svc_register(transp, BAKERY_PROG, BAKERY_VER, bakery_prog_1, IPPROTO_TCP))
	{
		fprintf(stderr, "%s", "unable to register (BAKERY_PROG, BAKERY_VER, tcp).");
		exit(1);
	}

	svc_run();
	fprintf(stderr, "%s", "svc_run returned");
	exit(1);
	/* NOTREACHED */
}
