#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/ktime.h>
#include <linux/cma.h>
#include <linux/dma-map-ops.h>

#include "cma_test.h"

static unsigned long alloc_count = 0x100000;
module_param(alloc_count, ulong, 0600);

struct page *buf = NULL;

ktime_t start, end;
s64 delta_ns;

int cma_test_init(void)
{
    void *p = NULL;

    printk("init CMA test mod.\n");
    printk("Measuring cma_alloc...\n");

    start = ktime_get();
    buf = cma_alloc(dma_contiguous_default_area, alloc_count, PAGE_SHIFT, true);
    end = ktime_get();
    delta_ns = ktime_to_ns(ktime_sub(end, start));
    printk("cma_alloc succeeded. Time taken: %lld ns (%lld us)\n",
           delta_ns, delta_ns / 1000);

    p = page_to_virt(buf);

    printk("alloc pages:buf:0x%lx:0x%lx.\n", (long unsigned int)buf, (long unsigned int)p);

    memset(p, 0x0e, 64);

    return 0;
}

void cma_test_exit(void)
{
    printk("exit CMA test mod.\n");

    if (buf)
    {
        printk("Measuring cma_release...\n");
        start = ktime_get();
        cma_release(dma_contiguous_default_area, buf, alloc_count);
        end = ktime_get();
        delta_ns = ktime_to_ns(ktime_sub(end, start));
        printk("cma_release succeeded. Time taken: %lld ns (%lld us)\n",
               delta_ns, delta_ns / 1000);
    }

    return;
}
