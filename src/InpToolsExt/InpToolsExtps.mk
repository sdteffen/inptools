
InpToolsExtps.dll: dlldata.obj InpToolsExt_p.obj InpToolsExt_i.obj
	link /dll /out:InpToolsExtps.dll /def:InpToolsExtps.def /entry:DllMain dlldata.obj InpToolsExt_p.obj InpToolsExt_i.obj \
		kernel32.lib rpcndr.lib rpcns4.lib rpcrt4.lib oleaut32.lib uuid.lib \

.c.obj:
	cl /c /Ox /DWIN32 /D_WIN32_WINNT=0x0400 /DREGISTER_PROXY_DLL \
		$<

clean:
	@del InpToolsExtps.dll
	@del InpToolsExtps.lib
	@del InpToolsExtps.exp
	@del dlldata.obj
	@del InpToolsExt_p.obj
	@del InpToolsExt_i.obj
