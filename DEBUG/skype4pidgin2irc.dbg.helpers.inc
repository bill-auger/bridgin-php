<?php

$LOCAL_OUTGOING_MESSAGE = 'PURPLE_MESSAGE_SEND' ;
$MESSGAE_FLAGS =
[
  hexdec('0x0001') => $LOCAL_OUTGOING_MESSAGE ,
  hexdec('0x0002') => 'PURPLE_MESSAGE_RECV' ,
  hexdec('0x0004') => 'PURPLE_MESSAGE_SYSTEM' ,
  hexdec('0x0008') => 'PURPLE_MESSAGE_AUTO_RESP' ,
  hexdec('0x0010') => 'PURPLE_MESSAGE_ACTIVE_ONLY' ,
  hexdec('0x0020') => 'PURPLE_MESSAGE_NICK' ,
  hexdec('0x0040') => 'PURPLE_MESSAGE_NO_LOG' ,
  hexdec('0x0080') => 'PURPLE_MESSAGE_WHISPER' ,
  hexdec('0x0200') => 'PURPLE_MESSAGE_ERROR' ,
  hexdec('0x0400') => 'PURPLE_MESSAGE_DELAYED' ,
  hexdec('0x0800') => 'PURPLE_MESSAGE_RAW' ,
  hexdec('0x1000') => 'PURPLE_MESSAGE_IMAGES' ,
  hexdec('0x2000') => 'PURPLE_MESSAGE_NOTIFY' ,
  hexdec('0x4000') => 'PURPLE_MESSAGE_NO_LINKIFY' ,
  hexdec('0x8000') => 'PURPLE_MESSAGE_INVISIBLE'
] ;

$DBG = "" ;


function DBGdumpBridge($aBridge)
{
  global $NAME_KEY , $CHANNELS_KEY , $ENABLED_KEY ;
  if (!$aBridge) { echo "DBGdumpBridge() invalid bridgeObj\n" ; return ; }
  echo "DBGdumpBridge() aBridge[NAME_KEY]=" . $aBridge[$BRIDGE_NAME_KEY] . "\n" ;
  echo "DBGdumpBridge() aBridge[ENABLED_KEY]=" . $aBridge[$ENABLED_KEY] . "\n" ;
  echo "DBGdumpBridge() aBridge[CHANNELS_KEY]=" . $aBridge[$CHANNELS_KEY] . " = ";
  var_dump($aBridge[$CHANNELS_KEY]) ;
}

function DBGdumpBridgeByName($bridgeName)
{
  global $Bridges ;
  if (!$Bridges[$bridgeName]) { echo "DBGdumpBridgeByName() no such bridge '$bridgeName'\n" ; return ; }

  echo "DBGdumpBridgeByName() bridgeName=$bridgeName\n" ;
  DBGdumpBridge($Bridges[$bridgeName]) ;
}

function DBGdumpAll() { global $Bridges ; foreach ($Bridges as $bridgeName => $aBridge) DBGdumpBridgeByName($bridgeName) ; }
