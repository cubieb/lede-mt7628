module("luci.controller.docsis", package.seeall)
function index()
        if not nixio.fs.access("/etc/cmcfg.txt") then
                return
        end
        entry({"admin", "services", "docsis"}, cbi("docsis"), _("docsis")).dependent = true
end