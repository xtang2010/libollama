
ifndef QCONFIG
QCONFIG=qconfig.mk
endif
include $(QCONFIG)

include $(MKFILES_ROOT)/qmacros.mk

NAME=ollama

#That will produce library named $(NAME).so
libvar=$(filter le be , $(VARIANT_LIST))
LIBVARDIR=so$(if $(libvar),-$(subst $(space),-,$(strip $(libvar))))
PUBLIC_INCVPATH += $(PROJECT_ROOT)/lib/public
LDVFLAG_test += -L$(PROJECT_ROOT)/lib/$(OS)/$(CPU)/$(LIBVARDIR) -l$(NAME) 
LDVFLAG_test += -lcurl -lcjson
CCVFLAG_test += -I$(QNX_TARGET)/usr/local/include
USEFILE=
EXTRA_SILENT_VARIANTS=lib test

# only install the library
PREFIX ?= /usr/local
INSTALL_ROOT_SO = $(QNX_TARGET)/$(CPU)$(PREFIX)
INSTALL_ROOT_AR = $(QNX_TARGET)/$(CPU)$(PREFIX)
INSTALL_ROOT_HDR = $(QNX_TARGET)$(PREFIX)/include

#===== VERSION_TAG_SO - undefine version tag for SONAME. If you want to define version, remove this and set SO_VERSION
override VERSION_TAG_SO=

#This has to be included last
include $(MKFILES_ROOT)/qtargets.mk


