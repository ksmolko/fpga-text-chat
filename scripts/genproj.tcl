# TCL Script to generate the platform and application vitis projects, and copy the contents over from the version-controlled directories

proc getScriptDirectory {} {
    set dispScriptFile [file normalize [info script]]
    set scriptFolder [file dirname $dispScriptFile]
    return $scriptFolder
}

set SCRIPT_DIR [getScriptDirectory]
set ROOT_DIR $SCRIPT_DIR/..

setws $ROOT_DIR/proj/
app create -name application -hw $ROOT_DIR/platform.xsa -proc ps7_cortexa9_0 -os standalone -lang C -template {Empty Application}
importsources -name application -path $ROOT_DIR/src/ -linker-script
bsp setlib -name lwip211 -ver 1.3
bsp write
bsp reload
catch {bsp regenerate}
exec rm $ROOT_DIR/ps7* $ROOT_DIR/*.bit