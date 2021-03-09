proc getScriptDirectory {} {
    set dispScriptFile [file normalize [info script]]
    set scriptFolder [file dirname $dispScriptFile]
    return $scriptFolder
}

set SCRIPT_DIR [getScriptDirectory]
set ROOT_DIR $SCRIPT_DIR/..
set SPRITES_DIR $ROOT_DIR/sprites/kb_sprites

dow -data $SPRITES_DIR/Key1.data 0x5000000
dow -data $SPRITES_DIR/Key2.data 0x5009c40
dow -data $SPRITES_DIR/Key3.data 0x5013880
dow -data $SPRITES_DIR/Key4.data 0x501d4c0
dow -data $SPRITES_DIR/Key5.data 0x5027100
dow -data $SPRITES_DIR/Key6.data 0x5030d40
dow -data $SPRITES_DIR/Key7.data 0x503a980
dow -data $SPRITES_DIR/Key8.data 0x50445c0
dow -data $SPRITES_DIR/Key9.data 0x504e200
dow -data $SPRITES_DIR/Key0.data 0x5057e40
dow -data $SPRITES_DIR/Minus.data 0x5061a80
dow -data $SPRITES_DIR/Backspace.data 0x506b6c0
dow -data $SPRITES_DIR/Q.data 0x507ef40
dow -data $SPRITES_DIR/W.data 0x5088b80
dow -data $SPRITES_DIR/E.data 0x50927c0
dow -data $SPRITES_DIR/R.data 0x509c400
dow -data $SPRITES_DIR/T.data 0x50a6040
dow -data $SPRITES_DIR/Y.data 0x50afc80
dow -data $SPRITES_DIR/U.data 0x50b98c0
dow -data $SPRITES_DIR/I.data 0x50c3500
dow -data $SPRITES_DIR/O.data 0x50cd140
dow -data $SPRITES_DIR/P.data 0x50d6d80
dow -data $SPRITES_DIR/Slash.data 0x50e09c0
dow -data $SPRITES_DIR/CapsLock.data 0x50ea600
dow -data $SPRITES_DIR/A.data 0x50fde80
dow -data $SPRITES_DIR/S.data 0x5107ac0
dow -data $SPRITES_DIR/D.data 0x5111700
dow -data $SPRITES_DIR/F.data 0x511b340
dow -data $SPRITES_DIR/G.data 0x5124f80
dow -data $SPRITES_DIR/H.data 0x512ebc0
dow -data $SPRITES_DIR/J.data 0x5138800
dow -data $SPRITES_DIR/K.data 0x5142440
dow -data $SPRITES_DIR/L.data 0x514c080
dow -data $SPRITES_DIR/Colon.data 0x5155cc0
dow -data $SPRITES_DIR/Apostrophe.data 0x515f900
dow -data $SPRITES_DIR/Enter.data 0x5169540
dow -data $SPRITES_DIR/Z.data 0x517cdc0
dow -data $SPRITES_DIR/X.data 0x5186a00
dow -data $SPRITES_DIR/C.data 0x5190640
dow -data $SPRITES_DIR/V.data 0x519a280
dow -data $SPRITES_DIR/B.data 0x51a3ec0
dow -data $SPRITES_DIR/N.data 0x51adb00
dow -data $SPRITES_DIR/M.data 0x51b7740
dow -data $SPRITES_DIR/Comma.data 0x51c1380
dow -data $SPRITES_DIR/Period.data 0x51cafc0
dow -data $SPRITES_DIR/QuestionMark.data 0x51d4c00
dow -data $SPRITES_DIR/ExclamationMark.data 0x51de840
dow -data $SPRITES_DIR/Return.data 0x51e8480
dow -data $SPRITES_DIR/Space.data 0x51fbd00