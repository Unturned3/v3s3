################################################################################
#
# H264ENC_DEMO
#
################################################################################

HELLO_VERSION = 1.0
HELLO_SITE = package/h264enc_demo/src
HELLO_SITE_METHOD = local

define HELLO_BUILD_CMDS
	$(MAKE) CC="$(TARGET_CC)" LD="$(TARGET_LD)" -C $(@D)
endef

define HELLO_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/hello $(TARGET_DIR)/home/root
endef

$(eval $(generic-package))

