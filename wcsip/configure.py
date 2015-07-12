import os
import sipconfig
build_file = "pywebconnect.sbf"

sip_path1 = "/home/sk/work/odesk/wxgecko/wxparty/Phoenix/sip/gen"
sip_path2 = "/home/sk/work/odesk/wxgecko/wxparty/Phoenix/src"

#wx_inc = ""

config = sipconfig.Configuration()

os.system(" ".join([config.sip_bin, "-c", ".", "-I", sip_path1, "-I", sip_path2, "-b", build_file, "pywebconnect.sip"]))
#os.system(" ".join([config.sip_bin, "-c", ".", "-b", build_file, "-I../webconnect", "webconnectwrap.sip"]))
#os.system(" ".join([config.sip_bin, "-c", ".", "-b", build_file, "-I", config.pyqt_sip_dir, pyqt_sip_flags, "hello.sip"]))

makefile = sipconfig.SIPModuleMakefile(config, build_file)

makefile.extra_defines = ["__WXGTK__"]
makefile.extra_include_dirs = [sip_path1, sip_path2]
makefile.extra_libs = ["pywebconnect"]

makefile.generate()
