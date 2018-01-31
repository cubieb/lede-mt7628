--[[
Other module
Description: File upload / download, web camera
Author: yuleniwo  xzm2@qq.com  QQ:529698939
]]--

module("luci.controller.filemgmt", package.seeall)

function index()
	entry({"admin", "services", "filemgmt"}, cbi("filemgmt"), _("filemgmt")).dependent = true
end