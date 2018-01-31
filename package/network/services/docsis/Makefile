#
# Copyright (C) 2006-2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=docsis
PKG_VERSION:=0.99
PKG_RELEASE:=5
PKG_MAINTAINER:=Daniel Danzberger <daniel@dd-wrt.com>
PKG_LICENSE:=GPL-2.0

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/$(PKG_NAME)
PKG_HASH:=96120134274038c0f7c3f2f6a27c752389d3ed27f98d4de025db0d8995f1d3ef

PKG_BUILD_DEPENDS:=libnetsnmp

include $(INCLUDE_DIR)/package.mk

define Package/docsis/Default
  SECTION:=net
  CATEGORY:=Network
  TITLE:=DOCSIS config file tools
  SUBMENU:=ProvServer
endef

define Package/docsis
  $(call Package/docsis/Default)
  DEPENDS:=+libnetsnmp
  TITLE+= docsis tool
endef

CONFIGURE_ARGS += \
	--disable-libwrap

define Package/docsis/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/docsis $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,docsis))
