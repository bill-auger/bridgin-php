<?php

function DBGvalidateSession() // drop-in replacement for validateSession()
{
  global $Bridges , $BRIDGE_NAME_KEY , $ENABLED_KEY , $CHANNELS_KEY , $PROTOCOL_KEY ;
  global $ACCOUNT_ID_KEY , $CHANNEL_ID_KEY , $CHANNEL_NAME_KEY , $USERNAME_KEY ;

echo "validateSession() " . ((is_array($Bridges))? "" : "! ") . "is_array(Bridges) nBridges = " . count($Bridges) . "\n" ;

  if (!is_array($Bridges)) return false ;

  foreach ($Bridges as $bridgeName => $aBridge)
  {
echo "validateSession() " . ((is_string($bridgeName))? "" : "! ") . "is_string($bridgeName) = $bridgeName\n" ;
echo "validateSession() " . ((is_array($aBridge))? "" : "! ") . "is_array(aBridge)\n" ;

    if (!is_string($bridgeName) || !is_array($aBridge)) return false ;

echo "validateSession() " . ((array_key_exists($BRIDGE_NAME_KEY , $aBridge))? "" : "! ") . "array_key_exists(BRIDGE_NAME_KEY)\n" ;
echo "validateSession() " . ((array_key_exists($ENABLED_KEY , $aBridge))? "" : "! ") . "array_key_exists(ENABLED_KEY)\n" ;
echo "validateSession() " . ((array_key_exists($CHANNELS_KEY , $aBridge))? "" : "! ") . "array_key_exists(CHANNELS_KEY)\n" ;

    if (!array_key_exists($BRIDGE_NAME_KEY , $aBridge) ||
        !array_key_exists($ENABLED_KEY , $aBridge) ||
        !array_key_exists($CHANNELS_KEY , $aBridge)) return false ;

echo "validateSession() " . ((is_string($aBridge[$BRIDGE_NAME_KEY]))? "" : "! ") . "is_string(BRIDGE_NAME_KEY) = " . $aBridge[$BRIDGE_NAME_KEY] . "\n" ;
echo "validateSession() " . ((is_bool($aBridge[$ENABLED_KEY]))? "" : "! ") . "is_bool(ENABLED_KEY) = " . $aBridge[$ENABLED_KEY] . "\n" ;
echo "validateSession() " . ((is_array($aBridge[$CHANNELS_KEY]))? "" : "! ") . "is_array(CHANNELS_KEY) nChannels = " . count($aBridge[$CHANNELS_KEY]) . "\n" ;

    if (!is_string($aBridge[$BRIDGE_NAME_KEY]) ||
        !is_bool($aBridge[$ENABLED_KEY]) ||
        !is_array($aBridge[$CHANNELS_KEY])) return false ;
    foreach ($aBridge[$CHANNELS_KEY] as $channelKey => $aChannel)
{echo "validateSession() " . ((is_string($channelKey))? "" : "! ") . "is_string($channelKey) = $channelKey\n" ;
 echo "validateSession() " . ((is_string($aChannel[$PROTOCOL_KEY]))? "" : "! ") . "is_string(aChannel[PROTOCOL_KEY]) = " . $aChannel[$PROTOCOL_KEY] . "\n" ;
 echo "validateSession() " . ((is_int($aChannel[$ACCOUNT_ID_KEY]))? "" : "! ") . "is_int(aChannel[ACCOUNT_ID_KEY]) = " . $aChannel[$ACCOUNT_ID_KEY] . "\n" ;
 echo "validateSession() " . ((is_int($aChannel[$CHANNEL_ID_KEY]))? "" : "! ") . "is_int(aChannel[CHANNEL_ID_KEY]) = " . $aChannel[$CHANNEL_ID_KEY] . "\n" ;
 echo "validateSession() " . ((is_string($aChannel[$CHANNEL_NAME_KEY]))? "" : "! ") . "is_string(aChannel[CHANNEL_NAME_KEY]) = " . $aChannel[$CHANNEL_NAME_KEY] . "\n" ;
 echo "validateSession() " . ((is_string($aChannel[$USERNAME_KEY]))? "" : "! ") . "is_string(aChannel[USERNAME_KEY]) = " . $aChannel[$USERNAME_KEY] . "\n" ;

      if (!is_string($channelKey) ||
          !is_string($aChannel[$PROTOCOL_KEY]) ||
          !is_int($aChannel[$ACCOUNT_ID_KEY]) ||
          !is_int($aChannel[$CHANNEL_ID_KEY]) ||
          !is_string($aChannel[$CHANNEL_NAME_KEY]) ||
          !is_string($aChannel[$USERNAME_KEY])) return false ;
}
  }

  return true ;
}
