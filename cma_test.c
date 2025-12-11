#include "test_khead.h"
#include "cma_test.h"
#include <linux/ktime.h>


static unsigned long area_base = 0;
module_param(area_base, ulong, 0600);

static unsigned long alloc_fn = 0;
module_param(alloc_fn, ulong, 0600);
static unsigned long free_fn = 0;
module_param(free_fn, ulong, 0600);


typedef struct page *(*cma_alloc_t)(struct cma *, size_t, unsigned int);
typedef bool (*cma_release_t)(struct cma *, const struct page *, unsigned int);

cma_alloc_t cma_alloc=NULL;
cma_release_t cma_release=NULL;

struct cma * dma_cma_p = NULL;

struct page * buf=NULL;

ktime_t start, end;
s64 delta_ns;

int cma_test_init(void)
{
    void * p = NULL;
    printk("init CMA test mod.\n");

    if((alloc_fn == 0)||(free_fn == 0))
    {
        printk("no alloc or free func.\n");
        return 0;
    }

    cma_alloc = (cma_alloc_t)alloc_fn;
    cma_release = (cma_release_t)free_fn;

    if(area_base == 0)
    {
        printk("area base == 0.\n");
        return 0;
    }
    
    dma_cma_p = (struct cma *)(*(unsigned long *)area_base);

    printk("Measuring cma_alloc...\n");
    start = ktime_get();
    buf = cma_alloc(dma_cma_p, 0x120000, (1<<PAGE_SHIFT));
    end = ktime_get();
    delta_ns = ktime_to_ns(ktime_sub(end, start));
    printk("cma_alloc succeeded. Time taken: %lld ns (%lld ms)\n",
                delta_ns, delta_ns / 1000000);

    p = page_to_virt(buf);
    
    printk("alloc pages:buf:0x%lx:0x%lx.\n",buf,p);

    memset(p,0x0e,64);

    return 0;
 
}

void cma_test_exit(void)
{
    printk("exit CMA test mod.\n");

    if(buf)
    {
        printk("Measuring cma_release...\n");
        start = ktime_get();
        cma_release(dma_cma_p, buf, 0x120000);
        end = ktime_get();
        delta_ns = ktime_to_ns(ktime_sub(end, start));
        printk("cma_release succeeded. Time taken: %lld ns (%lld ms)\n",
                delta_ns, delta_ns / 1000000);
    }

    return;    
}


