# Makefile for TX ISP T31 kernel module
# This builds the ISP driver as a loadable kernel module

# Default kernel source directory - can be overridden
KERNEL_SRC ?= /lib/modules/$(shell uname -r)/build

# Module name
MODULE_NAME := tx-isp-t31

# Default target
all: modules

# Build kernel modules
modules:
	$(MAKE) -C $(KERNEL_SRC) M=$(PWD)/driver modules

# Clean build artifacts
clean:
	$(MAKE) -C $(KERNEL_SRC) M=$(PWD)/driver clean
	rm -f driver/*.o driver/*.ko driver/*.mod.c driver/*.mod driver/.*.cmd
	rm -rf driver/.tmp_versions driver/Module.symvers driver/modules.order

# Install modules (requires root)
install: modules
	$(MAKE) -C $(KERNEL_SRC) M=$(PWD)/driver modules_install
	depmod -a

# Uninstall modules (requires root)
uninstall:
	rm -f /lib/modules/$(shell uname -r)/extra/$(MODULE_NAME).ko
	rm -f /lib/modules/$(shell uname -r)/extra/tx-isp-trace.ko
	depmod -a

# Load module (requires root)
load:
	insmod driver/$(MODULE_NAME).ko

# Unload module (requires root)
unload:
	rmmod $(MODULE_NAME) || true

# Show module info
info:
	modinfo driver/$(MODULE_NAME).ko

# Check syntax without building
check:
	$(MAKE) -C $(KERNEL_SRC) M=$(PWD)/driver C=1 modules

# Help target
help:
	@echo "Available targets:"
	@echo "  all/modules - Build kernel modules"
	@echo "  clean       - Clean build artifacts"
	@echo "  install     - Install modules (requires root)"
	@echo "  uninstall   - Uninstall modules (requires root)"
	@echo "  load        - Load module (requires root)"
	@echo "  unload      - Unload module (requires root)"
	@echo "  info        - Show module information"
	@echo "  check       - Check syntax with sparse"
	@echo "  help        - Show this help"
	@echo ""
	@echo "Environment variables:"
	@echo "  KERNEL_SRC  - Kernel source directory (default: /lib/modules/\$$(uname -r)/build)"

.PHONY: all modules clean install uninstall load unload info check help
