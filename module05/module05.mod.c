#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;
BUILD_LTO_INFO;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0x2c635209, "module_layout" },
	{ 0x37a0cba, "kfree" },
	{ 0xcefb0c9f, "__mutex_init" },
	{ 0x6bc3fbc0, "__unregister_chrdev" },
	{ 0xaf88e69b, "kmem_cache_alloc_trace" },
	{ 0x30a93ed, "kmalloc_caches" },
	{ 0x8120e312, "__register_chrdev" },
	{ 0x13c49cc2, "_copy_from_user" },
	{ 0xd0da656b, "__stack_chk_fail" },
	{ 0x6b10bee1, "_copy_to_user" },
	{ 0x88db9f48, "__check_object_size" },
	{ 0xbb9ed3bf, "mutex_trylock" },
	{ 0x92997ed8, "_printk" },
	{ 0x3213f038, "mutex_unlock" },
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0xbdfb6dbb, "__fentry__" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "B1069C3FB1AC0CDD1B8A000");
