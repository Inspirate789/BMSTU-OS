#include <windows.h>
#include <cstdio>
#include <ctime>

#define READERS_CNT 4
#define WRITERS_CNT 3

#define ITER_CNT 16

static HANDLE canRead;
static HANDLE canWrite;
static HANDLE mutex;

static LONG waitingWritersCount = 0;
static LONG waitingReadersCount = 0;
static LONG activeReadersCount = 0;
static bool writing = false;

static HANDLE readerThreads[READERS_CNT];
static HANDLE writerThreads[WRITERS_CNT];

static int readerDelays[READERS_CNT * ITER_CNT];
static int writerDelays[READERS_CNT * ITER_CNT];

static int value = 0;

void StartRead()
{
	InterlockedIncrement(&waitingReadersCount);

	if (writing || WaitForSingleObject(canWrite, 0) == WAIT_OBJECT_0)
		WaitForSingleObject(canRead, INFINITE);

	WaitForSingleObject(mutex, INFINITE);
	InterlockedDecrement(&waitingReadersCount);
	InterlockedIncrement(&activeReadersCount);
	SetEvent(canRead);
	ReleaseMutex(mutex);
}

void StopRead()
{
	InterlockedDecrement(&activeReadersCount);

	if (!activeReadersCount)
		SetEvent(canWrite);
}

void StartWrite()
{
	InterlockedIncrement(&waitingWritersCount);

	if (activeReadersCount > 0 || writing)
		WaitForSingleObject(canWrite, INFINITE);

	InterlockedDecrement(&waitingWritersCount);
	writing = true;
    ResetEvent(canWrite);
}

void StopWrite()
{
	writing = false;

	if (waitingReadersCount)
		SetEvent(canRead);
	else
		SetEvent(canWrite);
}

DWORD WINAPI ReaderRun(CONST LPVOID param)
{
    int id = *(int *)param;
    int begin = id * ITER_CNT;

    for (int i = 0; i < ITER_CNT; i++)
    {
        Sleep(readerDelays[begin + i]);

        StartRead();
        printf("Reader %d read:  %d (sleep time: %d)\n", id, value, readerDelays[begin + i]);
        StopRead();
    }
}

DWORD WINAPI WriterRun(CONST LPVOID param)
{
    int id = *(int *)param;
    int begin = id * ITER_CNT;
    for (int i = 0; i < ITER_CNT; i++)
    {
        Sleep(writerDelays[begin + i]);

        StartWrite();
        printf("Writer %d write: %d (sleep time: %d)\n", id, ++value, writerDelays[begin + i]);
        StopWrite();
    }
}

int main()
{
	setbuf(stdout, NULL);
	srand(time(NULL));

    for (int i = 0; i < READERS_CNT * ITER_CNT; i++)
        readerDelays[i] = rand() % 100 + 100;

    for (int i = 0; i < WRITERS_CNT * ITER_CNT; i++)
        writerDelays[i] = rand() % 100 + 100;

    if ((mutex = CreateMutex(NULL, FALSE, NULL)) == NULL)
    {
        perror("CreateMutex");
        exit(1);
    }

    if ((canRead = CreateEvent(NULL, FALSE, FALSE, NULL)) == NULL)
    {
        perror("CreateEvent (canRead)");
        exit(1);
    }

    if ((canWrite = CreateEvent(NULL, TRUE, FALSE, NULL)) == NULL)
    {
        perror("CreateEvent (canWrite)");
        exit(1);
    }

    int readersID[READERS_CNT];
    DWORD id = 0;
    for (int i = 0; i < READERS_CNT; i++)
    {
        readersID[i] = i;
        if ((readerThreads[i] = CreateThread(NULL, 0, ReaderRun, readersID + i, 0, &id)) == NULL)
        {
            perror("CreateThread (reader)");
            exit(1);
        }
    }

    int writersID[WRITERS_CNT];
    for (int i = 0; i < WRITERS_CNT; i++)
    {
        writersID[i] = i;

        if ((writerThreads[i] = CreateThread(NULL, 0, WriterRun, writersID + i, 0, &id)) == NULL)
        {
            perror("CreateThread (writer)");
            exit(1);
        }
    }

	WaitForMultipleObjects(READERS_CNT, readerThreads, TRUE, INFINITE);
	WaitForMultipleObjects(WRITERS_CNT, writerThreads, TRUE, INFINITE);

    for (int i = 0; i < READERS_CNT; i++)
        CloseHandle(readerThreads[i]);

    for (int i = 0; i < WRITERS_CNT; i++)
        CloseHandle(writerThreads[i]);

    CloseHandle(canRead);
    CloseHandle(canWrite);
    CloseHandle(mutex);

	return 0;
}
