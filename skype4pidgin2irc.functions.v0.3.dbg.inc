<?php
function DBGdumpBridge($aBridge)
{
  global $NAME_KEY , $CHANNELS_KEY , $ENABLED_KEY ;
  if (!$aBridge) { echo "DBGdumpBridge() invalid bridgeObj\n" ; return ; }
  echo "DBGdumpBridge() aBridge[NAME_KEY]=" . $aBridge[$NAME_KEY] . "\n" ;
  echo "DBGdumpBridge() aBridge[CHANNELS_KEY]=" . $aBridge[$CHANNELS_KEY] . "\n" ;
  echo "DBGdumpBridge() aBridge[ENABLED_KEY]=" . $aBridge[$ENABLED_KEY] . "\n" ;
}
function DBGdumpBridgeByName($bridgeName)
{
  global $Bridges ;
  if (!$Bridges[$bridgeName]) { echo "DBGdumpBridgeByName() no such bridge '$bridgeName'\n" ; return ; }

  echo "DBGdumpBridgeByName() bridgeName=$bridgeName\n" ;
  DBGdumpBridge($Bridges[$bridgeName]) ;
}
function DBGdumpAll() { global $Bridges ; foreach ($Bridges as $bridgeName => $aBridge) DBGdumpBridgeByName($bridgeName) ; }


/* chat helpers */

function startsWith($msg , $token) { return (strpos($msg , $token) === 0) ; }

function contains($msg , $token) { return (strpos($msg , $token) !== false) ; }

function lstrip($msg , $token) { return substr($msg , strlen($token)) ; }

function chatOut($nick , $chatOut)
{
  global $NickPrefix , $NICK_POSTFIX ;
  return $NickPrefix . $nick . $NICK_POSTFIX . $chatOut ;
}

function postMessage($channelId , $msg)
{
  global $Proxy ;
  if ($msg) $Proxy->PurpleConvChatSend($Proxy->PurpleConvChat($channelId) , $msg) ;
}


/* admin helpers */

function &getBridgeByCh($channelId)
{
  global $Bridges , $CHANNELS_KEY ;
  foreach ($Bridges as $bridgeName => $aBridge)
    if (array_key_exists($channelId , $aBridge[$CHANNELS_KEY])) return $aBridge ;
  return [] ;
}

function getChannelName($channelId) { return "$channelId" ; }

function getChannelState($channelId)
{
  global $THIS_CHANNEL_MSG , $THIS_BRIDGE_MSG , $NAME_KEY , $UNBRIDGED_MSG ;

  $aBridge = &getBridgeByCh($channelId) ;
  return "$THIS_CHANNEL_MSG$channelId" .
      (($aBridge)? "$THIS_BRIDGE_MSG '" . $aBridge[$NAME_KEY] . "'" : $UNBRIDGED_MSG) ;
}

function getBridgeStats($bridgeName)
{
  global $STATS_MSGa , $STATS_MSGb1 , $STATS_MSGb2 , $STATS_MSGc ;
  global $NAME_KEY , $CHANNELS_KEY , $ENABLED_KEY ;
  global $NO_SUCH_BRIDGE_MSG , $NO_BRIDGES_MSG , $BRIDGE_DELETED_MSG , $Bridges ;

  if (!($bridge = $Bridges[$bridgeName]))
    return "\n" . ((count($Bridges))?
        "$NO_SUCH_BRIDGE_MSG '$bridgeName'" : $NO_BRIDGES_MSG) ;

  $nameMsg = "$STATS_MSGa '" . $bridge[$NAME_KEY] . "' " ;
  $enabledMsg = ($bridge[$ENABLED_KEY])? $STATS_MSGb1 : $STATS_MSGb2 ;
  $nBridges = count($bridge[$CHANNELS_KEY]) ;
  $countMsg = " " . (($nBridges)? "$nBridges $STATS_MSGc" : $BRIDGE_DELETED_MSG) ;
  return "\n$nameMsg$enabledMsg$countMsg" . getBridgeChannels($bridge) ;
}

function getBridgeChannels($aBridge)
{
  global $CHANNELS_KEY ;
  $channelsMsg = "" ;
  foreach ($aBridge[$CHANNELS_KEY] as $channelId => $channelName)
    $channelsMsg .= "\n\t[$channelId] $channelName" ;
  return "$channelsMsg" ;
}

function argsRequiredMsg($trigger)
{
  global $ARGS_REQUIRED_MSG , $DEFAULT_BRIDGE_NAME ;
  return "$ARGS_REQUIRED_MSG$trigger $DEFAULT_BRIDGE_NAME" ;
}
