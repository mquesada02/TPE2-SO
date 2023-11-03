#include <library.h>
#include <test_util.h>
#include <MemoryManagerADT.h>

extern void * memset(void * destiny, int32_t c, uint64_t length);

#define MAX_BLOCKS 128

typedef struct MM_rq {
  void *address;
  uint32_t size;
} mm_rq;

uint64_t test_mm(uint64_t argc, char *argv[]) {
  mm_rq mm_rqs[MAX_BLOCKS];
  uint8_t rq;
  uint32_t total;
  uint64_t max_memory;

  if (argc != 1)
    return -1;

  if ((max_memory = satoi(argv[0])) <= 0)
    return -1;

  for(int p=0;p<15;p++) {
    rq = 0;
    total = 0;
    printf("Ciclo %d\n", p);
    // Request as many blocks as we can
    while (rq < MAX_BLOCKS && total < max_memory) {
      mm_rqs[rq].size = GetUniform(max_memory - total - 1) + 1;
      mm_rqs[rq].address = allocMemory(mm_rqs[rq].size);
      if (mm_rqs[rq].address) {
        total += mm_rqs[rq].size;
        rq++;
      }
    }

    // Set
    uint32_t i;
    for (i = 0; i < rq; i++)
      if (mm_rqs[i].address) {
        memset(mm_rqs[i].address, i, mm_rqs[i].size);
        printf("Alloc'd %d\n", mm_rqs[i].address);
      }

    // Check
    for (i = 0; i < rq; i++)
      if (mm_rqs[i].address)
        if (!memcheck(mm_rqs[i].address, i, mm_rqs[i].size)) {
          printf("test_mm ERROR\n");
          return -1;
        }
    printf("About to free\n");
    // Free
    for (i = 0; i < rq; i++)
      if (mm_rqs[i].address) {
        freeMemory(mm_rqs[i].address);
        printf("Free'd %d\n", mm_rqs[i].address);
      } 
  }
}