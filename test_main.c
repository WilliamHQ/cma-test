#include <linux/module.h>

#include "cma_test.h"

int __init test_init(void)
{
    cma_test_init();
    return 0;
}

void __exit test_exit(void)
{
    cma_test_exit();
    return;
}

module_init(test_init);
module_exit(test_exit);


MODULE_LICENSE("GPL");
MODULE_VERSION("dev");
MODULE_DESCRIPTION("menguozi_kernel_test");
