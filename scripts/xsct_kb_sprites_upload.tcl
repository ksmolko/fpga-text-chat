proc getScriptDirectory {} {
    set dispScriptFile [file normalize [info script]]
    set scriptFolder [file dirname $dispScriptFile]
    return $scriptFolder
}

set SCRIPT_DIR [getScriptDirectory]
set ROOT_DIR $SCRIPT_DIR/..
set KB_NORMAL_SPRITES_DIR $ROOT_DIR/sprites/normal_kb_sprites
set KB_SELECTED_SPRITES_DIR $ROOT_DIR/sprites/selected_kb_sprites

dow -data $KB_NORMAL_SPRITES_DIR/Key1.data 0x5000000
dow -data $KB_NORMAL_SPRITES_DIR/Key2.data 0x5009c40
dow -data $KB_NORMAL_SPRITES_DIR/Key3.data 0x5013880
dow -data $KB_NORMAL_SPRITES_DIR/Key4.data 0x501d4c0
dow -data $KB_NORMAL_SPRITES_DIR/Key5.data 0x5027100
dow -data $KB_NORMAL_SPRITES_DIR/Key6.data 0x5030d40
dow -data $KB_NORMAL_SPRITES_DIR/Key7.data 0x503a980
dow -data $KB_NORMAL_SPRITES_DIR/Key8.data 0x50445c0
dow -data $KB_NORMAL_SPRITES_DIR/Key9.data 0x504e200
dow -data $KB_NORMAL_SPRITES_DIR/Key0.data 0x5057e40
dow -data $KB_NORMAL_SPRITES_DIR/Minus.data 0x5061a80
dow -data $KB_NORMAL_SPRITES_DIR/Backspace.data 0x506b6c0
dow -data $KB_NORMAL_SPRITES_DIR/Q.data 0x507ef40
dow -data $KB_NORMAL_SPRITES_DIR/W.data 0x5088b80
dow -data $KB_NORMAL_SPRITES_DIR/E.data 0x50927c0
dow -data $KB_NORMAL_SPRITES_DIR/R.data 0x509c400
dow -data $KB_NORMAL_SPRITES_DIR/T.data 0x50a6040
dow -data $KB_NORMAL_SPRITES_DIR/Y.data 0x50afc80
dow -data $KB_NORMAL_SPRITES_DIR/U.data 0x50b98c0
dow -data $KB_NORMAL_SPRITES_DIR/I.data 0x50c3500
dow -data $KB_NORMAL_SPRITES_DIR/O.data 0x50cd140
dow -data $KB_NORMAL_SPRITES_DIR/P.data 0x50d6d80
dow -data $KB_NORMAL_SPRITES_DIR/Slash.data 0x50e09c0
dow -data $KB_NORMAL_SPRITES_DIR/CapsLock.data 0x50ea600
dow -data $KB_NORMAL_SPRITES_DIR/A.data 0x50fde80
dow -data $KB_NORMAL_SPRITES_DIR/S.data 0x5107ac0
dow -data $KB_NORMAL_SPRITES_DIR/D.data 0x5111700
dow -data $KB_NORMAL_SPRITES_DIR/F.data 0x511b340
dow -data $KB_NORMAL_SPRITES_DIR/G.data 0x5124f80
dow -data $KB_NORMAL_SPRITES_DIR/H.data 0x512ebc0
dow -data $KB_NORMAL_SPRITES_DIR/J.data 0x5138800
dow -data $KB_NORMAL_SPRITES_DIR/K.data 0x5142440
dow -data $KB_NORMAL_SPRITES_DIR/L.data 0x514c080
dow -data $KB_NORMAL_SPRITES_DIR/Colon.data 0x5155cc0
dow -data $KB_NORMAL_SPRITES_DIR/Apostrophe.data 0x515f900
dow -data $KB_NORMAL_SPRITES_DIR/Enter.data 0x5169540
dow -data $KB_NORMAL_SPRITES_DIR/Z.data 0x517cdc0
dow -data $KB_NORMAL_SPRITES_DIR/X.data 0x5186a00
dow -data $KB_NORMAL_SPRITES_DIR/C.data 0x5190640
dow -data $KB_NORMAL_SPRITES_DIR/V.data 0x519a280
dow -data $KB_NORMAL_SPRITES_DIR/B.data 0x51a3ec0
dow -data $KB_NORMAL_SPRITES_DIR/N.data 0x51adb00
dow -data $KB_NORMAL_SPRITES_DIR/M.data 0x51b7740
dow -data $KB_NORMAL_SPRITES_DIR/Comma.data 0x51c1380
dow -data $KB_NORMAL_SPRITES_DIR/Period.data 0x51cafc0
dow -data $KB_NORMAL_SPRITES_DIR/QuestionMark.data 0x51d4c00
dow -data $KB_NORMAL_SPRITES_DIR/ExclamationMark.data 0x51de840
dow -data $KB_NORMAL_SPRITES_DIR/Return.data 0x51e8480
dow -data $KB_NORMAL_SPRITES_DIR/Space.data 0x51fbd00

dow -data $KB_SELECTED_SPRITES_DIR/Key1.data 0x5236680
dow -data $KB_SELECTED_SPRITES_DIR/Key2.data 0x52402c0
dow -data $KB_SELECTED_SPRITES_DIR/Key3.data 0x5249f00
dow -data $KB_SELECTED_SPRITES_DIR/Key4.data 0x5253b40
dow -data $KB_SELECTED_SPRITES_DIR/Key5.data 0x525d780
dow -data $KB_SELECTED_SPRITES_DIR/Key6.data 0x52673c0
dow -data $KB_SELECTED_SPRITES_DIR/Key7.data 0x5271000
dow -data $KB_SELECTED_SPRITES_DIR/Key8.data 0x527ac40
dow -data $KB_SELECTED_SPRITES_DIR/Key9.data 0x5284880
dow -data $KB_SELECTED_SPRITES_DIR/Key0.data 0x528e4c0
dow -data $KB_SELECTED_SPRITES_DIR/Minus.data 0x5298100
dow -data $KB_SELECTED_SPRITES_DIR/Backspace.data 0x52a1d40
dow -data $KB_SELECTED_SPRITES_DIR/Q.data 0x52b55c0
dow -data $KB_SELECTED_SPRITES_DIR/W.data 0x52bf200
dow -data $KB_SELECTED_SPRITES_DIR/E.data 0x52c8e40
dow -data $KB_SELECTED_SPRITES_DIR/R.data 0x52d2a80
dow -data $KB_SELECTED_SPRITES_DIR/T.data 0x52dc6c0
dow -data $KB_SELECTED_SPRITES_DIR/Y.data 0x52e6300
dow -data $KB_SELECTED_SPRITES_DIR/U.data 0x52eff40
dow -data $KB_SELECTED_SPRITES_DIR/I.data 0x52f9b80
dow -data $KB_SELECTED_SPRITES_DIR/O.data 0x53037c0
dow -data $KB_SELECTED_SPRITES_DIR/P.data 0x530d400
dow -data $KB_SELECTED_SPRITES_DIR/Slash.data 0x5317040
dow -data $KB_SELECTED_SPRITES_DIR/CapsLock.data 0x5320c80
dow -data $KB_SELECTED_SPRITES_DIR/A.data 0x5334500
dow -data $KB_SELECTED_SPRITES_DIR/S.data 0x533e140
dow -data $KB_SELECTED_SPRITES_DIR/D.data 0x5347d80
dow -data $KB_SELECTED_SPRITES_DIR/F.data 0x53519c0
dow -data $KB_SELECTED_SPRITES_DIR/G.data 0x535b600
dow -data $KB_SELECTED_SPRITES_DIR/H.data 0x5365240
dow -data $KB_SELECTED_SPRITES_DIR/J.data 0x536ee80
dow -data $KB_SELECTED_SPRITES_DIR/K.data 0x5378ac0
dow -data $KB_SELECTED_SPRITES_DIR/L.data 0x5382700
dow -data $KB_SELECTED_SPRITES_DIR/Colon.data 0x538c340
dow -data $KB_SELECTED_SPRITES_DIR/Apostrophe.data 0x5395f80
dow -data $KB_SELECTED_SPRITES_DIR/Enter.data 0x539fbc0
dow -data $KB_SELECTED_SPRITES_DIR/Z.data 0x53b3440
dow -data $KB_SELECTED_SPRITES_DIR/X.data 0x53bd080
dow -data $KB_SELECTED_SPRITES_DIR/C.data 0x53c6cc0
dow -data $KB_SELECTED_SPRITES_DIR/V.data 0x53d0900
dow -data $KB_SELECTED_SPRITES_DIR/B.data 0x53da540
dow -data $KB_SELECTED_SPRITES_DIR/N.data 0x53e4180
dow -data $KB_SELECTED_SPRITES_DIR/M.data 0x53eddc0
dow -data $KB_SELECTED_SPRITES_DIR/Comma.data 0x53f7a00
dow -data $KB_SELECTED_SPRITES_DIR/Period.data 0x5401640
dow -data $KB_SELECTED_SPRITES_DIR/QuestionMark.data 0x540b280
dow -data $KB_SELECTED_SPRITES_DIR/ExclamationMark.data 0x5414ec0
dow -data $KB_SELECTED_SPRITES_DIR/Return.data 0x541eb00
dow -data $KB_SELECTED_SPRITES_DIR/Space.data 0x5432380
