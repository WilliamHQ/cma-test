# Kernel Test Module -- CMA

测试Linux内核CMA(Contiguous Memory Allocator)功能的模块。

## Table of contents
* [Usage](#usage)
    * [Welcome to cma test](#welcome-to-cma-test)
    * [Building](#building)
        * [Makefile](#makefile)
    * [Preparing to run](#preparing-to-run)
        * [Kernel configure](#kernel-configure)
        * [Kernel parameer](#kernel-parameter)
        * [Kernel symbol](#kernel-symbol)
    * [Run module](#run-module)
        * [Module parameter](#module-parmeter)
        * [Load module](#load-module)
        * [Result](#result)
        * [Remove module](#remove-module)
* [Article](#article)

## Usage

### Welcome to cma test.
该内核模块用于测试内核CMA(Contiguous Memory Allocator)的使用。测试了cma_alloc和cma_release等主要操作接口。由于CMA的运行全部在内核中进行，其主要操作接口的符号并没有作为外部接口导出，所以在该模块运行时需要将内核符号地址作为参数传递给该模块。

### Building
首先将cma_test拷贝到有linux kernel编译环境的设备上，进入cma_test目录，然后执行make。
```
# make
```
如果执行成功，会在cma_test目录生成一个名为cmatest.ko的文件，这个就是可插入的模块。

#### Makefile
cma_test模块的Makefile包含了编译cma-test模块的方法，还有清除模块和转换文件格式的方法。

使用clean方法可以清除编译的结果文件和中间结果文件。
```
# make clean
```

如果你的编辑环境和编译环境不是同一个环境，如：你采用Windows系统上的编辑器编辑cma-test模块中的文件，再放到Linux上编译cma-test。在这种情况下，你可能需要转换文件格式后再进行编译，则你需要unix方法。
```
# make unix
```

### Preparing to run
在加载cma-test模块之前，先要确保运行内核是开启了CMA支持的，并要通过内核启动参数预留CMA内存。

#### Kernel configure
使用CMA功能，需要在内核编译时开启DMA_CMA选项，确认运行内核是否支持该选项可以使用下面的命令。

```
# cat /boot/config-$(uname -r) | grep DMA_CMA
```

如果有如下输出，则表示运行内核支持DMA_CMA选项。否则表示不支持，需要重新编译内核。
```
# CONFIG_DMA_CMA=y
```

#### Kernel parameter
在Linux内核在启动时，会根据启动参数预留CMA内存，cma内存预留的参数的格式如下：
```
cma=nn[MG]@[start[MG][-end[MG]]] [ARM,X86,KNL]
        Sets the size of kernel global memory area for
        contiguous memory allocations and optionally the
        placement constraint by the physical address range of
        memory allocations. A value of 0 disables CMA
        altogether. For more information, see
        include/linux/dma-contiguous.h
```
如：需要从内存物理地址5G开始预留1G的连续内存用于CMA，则可以在内核启动选项中添加如下参数：
```
cma=1G@5G
```

如果需要检查CMA是否预留成功，可以执行下面的命令：
~~~
# cat /proc/meminfo |  grep Cma
CmaTotal:        1048576 kB
CmaFree:         1048576 kB
~~~

#### Kernel symbol
CMA功能默认只提供给内核中的函数调用，CMA的相关接口没有做符号导出(EXPORT_SYMBOL)，启动后加载的内核模块要使用CMA功能需要获取对应的接口的符号地址。

cma-test模块需要如下三个内核符号的地址：
* `dma_contiguous_default_area`: DMA_CMA管理结构体指针；
* `cma_alloc`: cma内存分配函数地址；
* `cma_release`: cma内存释放函数地址；

可以在/proc/kallsyms中获取这三个符号地址。
```
# cat /proc/kallsyms | grep dma_contiguous_default_area
```
得到运行系统中的`dma_contiguous_default_area`符号地址：
```
ffffffff9445ba58 B dma_contiguous_default_area
```

前面的数值既是符号的地址，同样的方法可以获取：cma_alloc和cma_release的符号地址。

**NOTE:** 每次系统重启，这些符号的地址可能会发生变化。

### Run module
运行模块采用insmod命令，但要该模块正常使用CMA内存还需要将上面获取的三个内核符号地址。通过模块参数的形式我们可以做到这一点。

#### Module parameter
cma-test模块有三个模块参数，分别对应上述的三个内核符号地址：
* `area_base`: 对应`dma_contiguous_default_area` 符号地址；
* `alloc_fn`: cma内存分配函数符号地址；
* `free_fn`: cma内存释放函数符号地址；

#### Load module
现在你可以加载cma-test模块了：
~~~
# insmod cmatest.ko area_base=0xffffffff9445ba58 alloc_fn=0xffffffff93255140 free_fn=0xffffffff93255350
~~~

这里命令行中的参数数值就是在[Kernel symbol](#kernel-symbol)章节中获取的符号地址。

#### Result
模块加载会在系统中输出message，你可以从系统的dmesg中查询到模块加载的信息。
```
# dmesg | tail -20
```
你可以看到模块打印的信息：
```
[ 3609.360788] init CMA test mod.
[ 3609.369515] alloc pages:buf:0xffffea0005000000:0xffff880140000000.
```

#### Remove module
你可以使用下面命令卸载模块：
```
# rmmod cmatest
```

# Article
[在Linux内核模块中使用CMA内存分配](https://blog.csdn.net/willhq/article/details/124602370)
