-- Copyright 2011 Jo-Philipp Wich <jow@openwrt.org>
-- Licensed to the public under the Apache License 2.0.

local fs = require "nixio.fs"

local f = SimpleForm("docsis",
        "CM config editing section")

local o = f:field(Value, "_custom")

o.template = "cbi/tvalue"
o.rows = 20

function o.cfgvalue(self, section)
        require("luci.sys").call("/usr/sbin/docsis -d /tmp/upload/bob_cm.cfg > /etc/cmcfg.txt")
        return fs.readfile("/etc/cmcfg.txt")
end

function o.write(self, section, value)
        value = value:gsub("\r\n?", "\n")
        fs.writefile("/etc/cmcfg.txt", value)
        require("luci.sys").call("/usr/sbin/docsis -e /etc/cmcfg.txt /etc/keyfile /tmp/upload/bob_cm.cfg >/dev/null 2<&1")
        require("nixio").syslog('info', 'Regenerated cm config file to /tmp/upload/bob_cm.cfg')
end

f.submit ="Generate CM config file"

return f