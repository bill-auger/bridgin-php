<?php

if ($DEBUG_SESSION) include $DEBUG_SESSION_FILENAME ;


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
