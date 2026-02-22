/*
 * ipc_play - Simple QNX message passing demo
 *
 * Usage:
 *   ./ipc_play server
 *   ./ipc_play client <server_pid>
 *
 * Environment:
 *   QNX_CHID=<chid> (default 1)
 */

#include <errno.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/netmgr.h>
#include <sys/neutrino.h>
#include <time.h>
#include <unistd.h>

#define REQ_MAGIC 0x514e5851u /* "QNXQ" */
#define REP_MAGIC 0x51525852u /* "QRXR" */

struct request {
	uint32_t magic;
	uint32_t client_id;
	uint32_t seq;
	uint32_t payload;
};

struct reply {
	uint32_t magic;
	uint32_t worker_id;
	uint32_t seq;
	uint32_t result;
};

struct worker_arg {
	int chid;
	int worker_id;
};

struct client_arg {
	int coid;
	uint32_t client_id;
	uint32_t thread_id;
	uint32_t count;
};

static void die(const char *msg)
{
	perror(msg);
	exit(EXIT_FAILURE);
}

static uint64_t now_ns(void)
{
	struct timespec ts;

	if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0)
		die("clock_gettime");

	return (uint64_t)ts.tv_sec * 1000000000ull +
	       (uint64_t)ts.tv_nsec;
}

static void *server_worker(void *argp)
{
	struct worker_arg *arg = argp;
	int chid = arg->chid;
	int wid = arg->worker_id;

	for (;;) {
		struct request req;
		struct reply rep;
		int rcvid;
		uint32_t result;

		rcvid = MsgReceive(chid, &req, sizeof(req), NULL);
		if (rcvid == -1) {
			if (errno == EINTR)
				continue;
			die("MsgReceive");
		}

		if (rcvid == 0)
			continue;

		if (req.magic != REQ_MAGIC) {
			rep.magic = REP_MAGIC;
			rep.worker_id = (uint32_t)wid;
			rep.seq = req.seq;
			rep.result = 0xdeadbeef;

			MsgReply(rcvid, EOK, &rep, sizeof(rep));
			continue;
		}

		result = req.payload * 3u + req.seq;

		rep.magic = REP_MAGIC;
		rep.worker_id = (uint32_t)wid;
		rep.seq = req.seq;
		rep.result = result;

		if (MsgReply(rcvid, EOK, &rep, sizeof(rep)) == -1)
			perror("MsgReply");
	}

	return NULL;
}

static int run_server(void)
{
	pthread_t t1, t2;
	struct worker_arg a1;
	struct worker_arg a2;
	int chid;
	pid_t pid;

	chid = ChannelCreate(0);
	if (chid == -1)
		die("ChannelCreate");

	pid = getpid();

	printf("[server] pid=%d chid=%d\n", pid, chid);
	printf("[server] starting worker threads\n");

	a1.chid = chid;
	a1.worker_id = 1;

	a2.chid = chid;
	a2.worker_id = 2;

	if (pthread_create(&t1, NULL, server_worker, &a1))
		die("pthread_create t1");

	if (pthread_create(&t2, NULL, server_worker, &a2))
		die("pthread_create t2");

	pthread_join(t1, NULL);
	pthread_join(t2, NULL);

	return 0;
}

static void *client_sender(void *argp)
{
	struct client_arg *arg = argp;
	uint32_t i;

	for (i = 0; i < arg->count; i++) {
		struct request req;
		struct reply rep;
		uint64_t t0, t1;
		int r;

		req.magic = REQ_MAGIC;
		req.client_id = arg->client_id;
		req.seq = (arg->thread_id << 24) | i;
		req.payload = 100 + arg->thread_id;

		t0 = now_ns();
		r = MsgSend(arg->coid, &req, sizeof(req),
			    &rep, sizeof(rep));
		t1 = now_ns();

		if (r == -1) {
			perror("MsgSend");
			break;
		}

		if (rep.magic != REP_MAGIC) {
			printf("[client %u/%u] bad reply magic\n",
			       arg->client_id,
			       arg->thread_id);
			continue;
		}

		printf("[client %u thr %u] seq=%u worker=%u result=%u latency=%.3f ms\n",
		       arg->client_id,
		       arg->thread_id,
		       rep.seq,
		       rep.worker_id,
		       rep.result,
		       (double)(t1 - t0) / 1e6);

		usleep(50000);
	}

	return NULL;
}

static int run_client(pid_t server_pid)
{
	pthread_t t1, t2;
	struct client_arg a1;
	struct client_arg a2;
	const char *env;
	uint32_t client_id;
	int chid = 1;
	int coid;

	env = getenv("QNX_CHID");
	if (env)
		chid = atoi(env);

	coid = ConnectAttach(ND_LOCAL_NODE,
			     server_pid,
			     chid,
			     _NTO_SIDE_CHANNEL,
			     0);
	if (coid == -1)
		die("ConnectAttach");

	client_id = (uint32_t)getpid();

	printf("[client] pid=%d -> server_pid=%d chid=%d coid=%d\n",
	       getpid(),
	       server_pid,
	       chid,
	       coid);

	a1.coid = coid;
	a1.client_id = client_id;
	a1.thread_id = 1;
	a1.count = 1000;

	a2.coid = coid;
	a2.client_id = client_id;
	a2.thread_id = 2;
	a2.count = 1000;

	if (pthread_create(&t1, NULL, client_sender, &a1))
		die("pthread_create client t1");

	if (pthread_create(&t2, NULL, client_sender, &a2))
		die("pthread_create client t2");

	pthread_join(t1, NULL);
	pthread_join(t2, NULL);

	ConnectDetach(coid);

	return 0;
}

int main(int argc, char **argv)
{
	if (argc < 2) {
		fprintf(stderr,
			"Usage:\n"
			"  %s server\n"
			"  %s client <server_pid>\n"
			"Env:\n"
			"  QNX_CHID=<chid> (default 1)\n",
			argv[0],
			argv[0]);
		return EXIT_FAILURE;
	}

	if (!strcmp(argv[1], "server"))
		return run_server();

	if (!strcmp(argv[1], "client")) {
		pid_t spid;

		if (argc < 3) {
			fprintf(stderr, "missing server_pid\n");
			return EXIT_FAILURE;
		}

		spid = (pid_t)atoi(argv[2]);
		return run_client(spid);
	}

	fprintf(stderr, "unknown mode: %s\n", argv[1]);
	return EXIT_FAILURE;
}