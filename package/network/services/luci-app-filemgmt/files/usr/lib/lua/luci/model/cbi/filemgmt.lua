local fs = require "nixio.fs"
local http = luci.http

ful = SimpleForm("upload", translate("Upload"), nil)
ful.reset = false
ful.submit = false

sul = ful:section(SimpleSection, "", translate("Upload file to '/tmp/upload/'"))
fu = sul:option(FileUpload, "")
fu.template = "cbi/other_upload"
um = sul:option(DummyValue, "", nil)
um.template = "cbi/other_dvalue"

local dir, fd
dir = "/tmp/upload/"
fs.mkdir(dir)
http.setfilehandler(
	function(meta, chunk, eof)
		if not fd then
			if (not meta) or (not meta.name) or (not meta.file) then return end
			fd = nixio.open(dir .. meta.file, "w")
			if not fd then
				um.value = translate("Create upload file error.")
				return
			end
		end
		if chunk and fd then
			fd:write(chunk)
		end
		if eof and fd then
			fd:close()
			fd = nil
			um.value = translate("File saved to") .. ' "/tmp/upload/' .. meta.file .. '"'
		end
	end
)

local inits, i, attr = {}, 1
local files = fs.dir("/tmp/upload/")
if files then
	for f in files do
		attr = fs.stat("/tmp/upload/" .. f)
		if attr then
			inits[i] = {}
			inits[i].name = f
			inits[i].mtime = os.date("%Y-%m-%d %H:%M:%S", attr.mtime)
			inits[i].modestr = attr.modestr
			inits[i].size = tostring(attr.size)
			inits[i].remove = 0
			inits[i].install = false
			i = i + 1
		end
	end
end

form = SimpleForm("filelist", translate("ProvServer file list"), nil)
form.reset = false
form.submit = false

tb = form:section(Table, inits)
dm = tb:option(DummyValue, "", nil)
nm = tb:option(DummyValue, "name", translate("File name"))
mt = tb:option(DummyValue, "mtime", translate("Modify time"))
ms = tb:option(DummyValue, "modestr", translate("Mode string"))
sz = tb:option(DummyValue, "size", translate("Size"))
btnrm = tb:option(Button, "remove", translate("Remove"))
btnrm.render = function(self, section, scope)
	self.inputstyle = "remove"
	Button.render(self, section, scope)
end

btnrm.write = function(self, section)
	local v = fs.unlink("/tmp/upload/" .. fs.basename(inits[section].name))
	if v then table.remove(inits, section) end
	return v
end


btnis = tb:option(Button, "Download", translate("Download"))
btnis.template = "cbi/other_button"
btnis.render = function(self, section, scope)
	if not inits[section] then return false end
	scope.display = ""
	self.inputstyle = "apply"
	Button.render(self, section, scope)
end

btnis.write = function(self, section)
	local sPath, sFile, fd, block
	sPath = string.format("/tmp/upload/%s", inits[section].name)
	sFile = fs.basename(sPath)
	if fs.stat(sPath, "type") == "dir" then
		dm.value = translate("Couldn't open path for download: ") .. sPath
		return
	else
		fd = nixio.open(sPath, "r")
	end
	if not fd then
		dm.value = translate("Couldn't open file: ") .. sPath
		return
	end
	dm.value = nil
	http.header('Content-Disposition', 'attachment; filename="%s"' % {sFile})
	http.prepare_content("application/octet-stream")
	while true do
		block = fd:read(nixio.const.buffersize)
		if (not block) or (#block ==0) then
			break
		else
			http.write(block)
		end
	end
	fd:close()
	http.close()
end

return ful, form
