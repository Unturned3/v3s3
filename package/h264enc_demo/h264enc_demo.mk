################################################################################
#
# H264ENC_DEMO
#
################################################################################

H264ENC_DEMO_SITE = $(BR2_EXTERNAL_V3S3_PATH)/package/h264enc_demo/src
H264ENC_DEMO_SITE_METHOD = local
H264ENC_DEMO_DEPENDENCIES = libcedarc libv4l

define H264ENC_DEMO_BUILD_CMDS
	$(MAKE) CC="$(TARGET_CC)" LD="$(TARGET_LD)" -C $(@D)
endef

define H264ENC_DEMO_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/main $(TARGET_DIR)/root/h264enc_demo
endef

$(eval $(generic-package))

