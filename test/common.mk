ifndef QCONFIG
QCONFIG=qconfig.mk
endif
include $(QCONFIG)

NAME=ollama_test
USEFILE=

libvar          =$(filter le be , $(VARIANT_LIST))
LIBVARDIR       =so$(if $(libvar),-$(subst $(space),-,$(strip $(libvar))))
EXTRA_INCVPATH += $(PROJECT_ROOT)/../lib/public $(QNX_TARGET)/usr/local/include
EXTRA_LIBVPATH += $(PROJECT_ROOT)/../lib/$(OS)/$(CPU)/$(LIBVARDIR)
LIBS           += ollama curl cJSON

TEST_TARGETS    = $(basename $(notdir $(SRCS)))
EXTRA_CLEAN     = $(TEST_TARGETS)

include $(MKFILES_ROOT)/qmacros.mk

ALL_DEPENDENCIES = $(TEST_TARGETS)
INSTALL_DIRECTORY = /dev/null

include $(MKFILES_ROOT)/qtargets.mk

$(TEST_TARGETS): %: %.o
	$(TARGET_BUILD)

