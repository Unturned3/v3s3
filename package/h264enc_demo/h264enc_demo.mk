################################################################################
#
# H264ENC_DEMO
#
################################################################################

H264ENC_DEMO_VERSION = master
H264ENC_DEMO_SITE = $(call github,Unturned3,h264enc_demo,$(H264ENC_DEMO_VERSION))
H264ENC_DEMO_DEPENDENCIES = libcedarc libv4l

define H264ENC_DEMO_BUILD_CMDS
	$(MAKE) CC="$(TARGET_CC)" LD="$(TARGET_LD)" -C $(@D) all
endef

define H264ENC_DEMO_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/main $(TARGET_DIR)/root/h264enc_demo
endef

$(eval $(generic-package))

