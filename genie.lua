ROOT_DIR = path.getabsolute('.')
BGFX_DIR = path.join(ROOT_DIR, '3rdparty/bgfx')
BX_DIR   = path.join(ROOT_DIR, '3rdparty/bx')
BIMG_DIR = path.join(ROOT_DIR, '3rdparty/bimg')
SPDLOG_DIR = path.join(ROOT_DIR, '3rdparty/spdlog')
local BUILD_DIR = ".build"
local LIB_DIR = ".build/lib"


solution('bgfx-starter')
    configurations({'Debug','Release'})
    platforms({'x32','x64','Native'})
    language('C++')
    startproject('starter')

    dofile (path.join(BX_DIR, "scripts/toolchain.lua"))
    if not toolchain(BUILD_DIR, LIB_DIR) then
        return -- no action specified
    end


-- project bgfx
function copyLib() end
dofile (path.join(BGFX_DIR, 'scripts/bgfx.lua'))
bgfxProject("", "StaticLib", {})

-- project example-common
dofile (path.join(BGFX_DIR, 'scripts/example-common.lua'))

-- project bx
dofile (path.join(BX_DIR, 'scripts/bx.lua'))

-- project bimg
dofile (path.join(BIMG_DIR, 'scripts/bimg.lua'))
dofile (path.join(BIMG_DIR, 'scripts/bimg_decode.lua'))
dofile (path.join(BIMG_DIR, 'scripts/bimg_encode.lua'))

-- project starter
project('starter')
    kind('WindowedApp')
    files({'src/**.h', 'src/**.cpp'})
    includedirs({
        path.join(BGFX_DIR, 'include'),
        path.join(BGFX_DIR, 'examples/common'),
        path.join(BGFX_DIR, '3rdparty'),
        path.join(BX_DIR, 'include'),
        path.join(BIMG_DIR, 'include'),
        path.join(SPDLOG_DIR, 'include')
    })
    links({
        'bx',
        'bgfx',
        'example-common'
    })
    configuration({ "vs20*", "x32 or x64" })
       links {
           "gdi32",
           "psapi",
           "bimg",
           "bimg_decode",
       }
    configuration({})

-- tools
dofile (path.join(BGFX_DIR, 'scripts/shaderc.lua'))
dofile (path.join(BGFX_DIR, 'scripts/texturec.lua'))

