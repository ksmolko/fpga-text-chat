# TCL Script to generate the platform and application vitis projects, and copy the contents over from the version-controlled directories

proc getScriptDirectory {} {
    set dispScriptFile [file normalize [info script]]
    set scriptFolder [file dirname $dispScriptFile]
    return $scriptFolder
}

set SCRIPT_DIR [getScriptDirectory]
set ROOT_DIR $SCRIPT_DIR/..
set PROJ_DIR $ROOT_DIR/proj
set APP_DIR $PROJ_DIR/application

setws $PROJ_DIR/
app create -name application -hw $ROOT_DIR/platform.xsa -proc ps7_cortexa9_0 -os standalone -lang C -template {Empty Application}
importsources -name application -path $ROOT_DIR/src/ -linker-script
exec mkdir $APP_DIR/include
app config -name application -add include-path $APP_DIR/include
exec cp $ROOT_DIR/include/* $APP_DIR/include
bsp setlib -name lwip211 -ver 1.3
bsp write
bsp reload
catch {bsp regenerate}
app config -name application -add compiler-misc {-Werror}

# Clean up junk that gets generated
exec rm -rf $ROOT_DIR/ps7* $ROOT_DIR/*.bit .Xil $ROOT_DIR/.Xil