#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
 
#define NLOOP_FOR_ESTIMATION 1000000000UL

// 1000000ns = 1ms
#define NSECS_PER_MSEC 1000000UL

// 1000000000ns = 1sec
#define NSECS_PER_SEC 1000000000UL

static unsigned long nloop_per_resol;
static struct timespec start;

// 時刻の差をナノ秒で求める
// 実行速度を早くする目的で inline 化している ? 
static inline long diff_nsec(const struct timespec before, const struct timespec after)
{
    return ((after.tv_sec * NSECS_PER_SEC + after.tv_nsec)
            - (before.tv_sec * NSECS_PER_SEC + before.tv_nsec));
}

// 処理に約 1 ミリ秒かかるループ回数を求める
static unsigned long estimate_loops_per_msec()
{
    struct timespec before;
    clock_gettime(CLOCK_MONOTONIC, &before);

    for (unsigned long i = 0; i < NLOOP_FOR_ESTIMATION; i++)
    {
    }

    struct timespec after;
    clock_gettime(CLOCK_MONOTONIC, &after);

    return NLOOP_FOR_ESTIMATION * NSECS_PER_MSEC / diff_nsec(before, after);
}
 
static inline void load()
{
    // 統計情報採取間隔 1 回分の処理をさせる
    for (unsigned long i = 0; i < nloop_per_resol; i++)
    {
    }
}

static void child_fn(const int id, struct timespec *buf, const int nrecord)
{
    for (int i = 0; i < nrecord; i++)
    {
        // 統計情報採取間隔 1 回分の処理をさせる
        load();

        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        buf[i] = ts;
    }

    // 結果を出力する
    for (int i = 0; i < nrecord; i++)
    {
        printf("%d\t%ld\t%d\n", id, diff_nsec(start, buf[i]) / NSECS_PER_MSEC, (i + 1) * 100 / nrecord);
    }
    exit(EXIT_SUCCESS);
}
 
static pid_t *pids;
int main(const int argc, char *argv[])
{
    // 引数チェック
    if (argc < 4)
    {
        fprintf(stderr, "usage: %s <nproc> <total[ms]> <resolution[ms]>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    const int nproc = atoi(argv[1]);
    const int total = atoi(argv[2]);
    const int resol = atoi(argv[3]);
    if (nproc < 1)
    {
        fprintf(stderr, "<nproc>(%d) should be >= 1\n", nproc);
        exit(EXIT_FAILURE);
    }
    if (total < 1)
    {
        fprintf(stderr, "<total>(%d) should be >= 1\n", total);
        exit(EXIT_FAILURE);
    }
    if (resol < 1)
    {
        fprintf(stderr, "<resol>(%d) should be >= 1\n", resol);
        exit(EXIT_FAILURE);
    }
    if (total % resol)
    {
        fprintf(stderr, "<total>(%d) should be multiple of <resolution>(%d)\n", total, resol);
        exit(EXIT_FAILURE);
    }

    // 計測準備
    const int nrecord = total / resol;
    // 結果を入れるメモリを確保する
    // fork するので、1 プロセス分だけ確保すればいい
    struct timespec *logbuf = malloc(nrecord * sizeof(struct timespec));
    if (!logbuf)
    {
        err(EXIT_FAILURE, "failed to allocate log buffer");
    }

    puts("estimating the workload which takes just one milli-second...");
    nloop_per_resol = estimate_loops_per_msec() * resol;
    puts("end estimation");
    fflush(stdout);

    pids = malloc(nproc * sizeof(pid_t));
    if (pids == NULL)
    {
       err(EXIT_FAILURE, "failed to allocate pid table");
    }

    // 計測開始
    clock_gettime(CLOCK_MONOTONIC, &start);
    int ret = EXIT_SUCCESS;
    int ncreated = 0;
    for (int i = 0; i < nproc; i++, ncreated++)
    {
        pids[i] = fork();

        // fork に失敗したらすべての子プロセスを終了する
        if (pids[i] < 0)
        {
            for (int j = 0; j < ncreated; j++)
            {
                kill(pids[j], SIGKILL);
            }
            ret = EXIT_FAILURE;
            break;
        }
        else if (pids[i] == 0)
        {
            // children
            child_fn(i, logbuf, nrecord);
            /* shouldn't reach here */
            abort();
        }
    }

    // parent
    for (int i = 0; i < ncreated; i++)
    {
        if (wait(NULL) < 0)
        {
            warn("wait() failed.");
        }
    }

    exit(ret);
}
