<?php

/* persistence helpers */

function validateConfig() // see /* schema */ section in 'constants.inc'
{
  global $Bridges , $BRIDGE_NAME_KEY , $ENABLED_KEY , $CHANNELS_KEY ;

// echo "validateConfig() is_array(Bridges)=" . is_array($Bridges) . "\n" ;
// echo "validateConfig() count(Bridges)=" . count($Bridges) . "\n" ;

  if (!is_array($Bridges)) return false ;

  foreach ($Bridges as $bridgeName => $aBridge)
  {
// echo "validateConfig() is_array($bridgeName)=" . is_array($aBridge) . "\n" ;

    if (!is_array($aBridge)) return false ;

// echo "validateConfig() array_key_exists(BRIDGE_NAME_KEY)=" . array_key_exists($BRIDGE_NAME_KEY , $aBridge) . "\n" ;
// echo "validateConfig() array_key_exists(ENABLED_KEY)=" . array_key_exists($ENABLED_KEY , $aBridge) . "\n" ;
// echo "validateConfig() array_key_exists(CHANNELS_KEY)=" . array_key_exists($CHANNELS_KEY , $aBridge) . "\n" ;

    if (!array_key_exists($BRIDGE_NAME_KEY , $aBridge) ||
        !array_key_exists($ENABLED_KEY , $aBridge) ||
        !array_key_exists($CHANNELS_KEY , $aBridge)) return false ;

// echo "validateConfig() is_string(BRIDGE_NAME_KEY)=" . is_string($aBridge[$BRIDGE_NAME_KEY]) . "\n" ;
// echo "validateConfig() is_bool(ENABLED_KEY)=" . is_bool($aBridge[$ENABLED_KEY]) . "\n" ;
// echo "validateConfig() is_array(CHANNELS_KEY)=" . is_array($aBridge[$CHANNELS_KEY]) . "\n" ;

    if (!is_string($aBridge[$BRIDGE_NAME_KEY]) ||
        !is_bool($aBridge[$ENABLED_KEY]) ||
        !is_array($aBridge[$CHANNELS_KEY])) return false ;
    foreach ($aBridge[$CHANNELS_KEY] as $channelId => $channelName)
//{ echo "validateConfig() is_string($channelName)=" . is_string($channelName) . "\n" ;
//echo "validateConfig() is_int($channelId)=" . is_int($channelId) . "\n" ;

      if (!is_string($channelName) || !is_int($channelId)) return false ;
//}
  }
echo "ok\n" ;
  return true ;
}

function loadConfig()
{
  global $BRIDGES_FILENAME , $Bridges ;

//echo "loadConfig() file_exists($BRIDGES_FILENAME)=" . (file_exists($BRIDGES_FILENAME)) . "\n" ;

  if (!file_exists($BRIDGES_FILENAME)) return false ;

  $Bridges = unserialize(file_get_contents($BRIDGES_FILENAME)) ;

//echo "\nloadConfig() Bridges=" ; var_dump($Bridges) ;

  return validateConfig() ;
}

function storeConfig()
{
  global $BRIDGES_FILENAME , $Bridges ;
  return file_put_contents($BRIDGES_FILENAME , serialize($Bridges)) ;
}


/* dbus helpers */

function initDbusProxy()
{
  global $Dbus , $Proxy ;
  global $PURPLE_INTERFACE , $CHAT_METHOD , $PURPLE_SERVICE , $PURPLE_OBJECT ;
  try
  {
    $Dbus = new Dbus(Dbus::BUS_SESSION) ;
    $Dbus->addWatch($PURPLE_INTERFACE , $CHAT_METHOD) ;
    $Proxy = $Dbus->createProxy($PURPLE_SERVICE , $PURPLE_OBJECT , $PURPLE_INTERFACE) ;
  }
  catch(Exception $ex) {}
}

function getAccounts()
{
  global $Proxy ; $accounts = [] ;
  try { $accounts = $Proxy->PurpleAccountsGetAll() ; } catch(Exception $ex) {}
  return $accounts ;
}

function isConnected($accountId)
{
  global $Proxy ; $isConnected = false ;

  try { $isConnected = $Proxy->PurpleAccountIsConnected($accountId) ; }
  catch(Exception $ex) { echo $DBUS_ERROR_MSG ; }
  return $isConnected ;
}

function getProtocol($accountId)
  { global $Proxy ; return $Proxy->PurpleAccountGetProtocolName($accountId) ; }

function getUsername($accountId)
  { global $Proxy ; return $Proxy->PurpleAccountGetUsername($accountId) ; }

function getAccount($channelId)
  { global $Proxy ; return $Proxy->PurpleConversationGetAccount($channelId) ; }

function getChannelName($channelId)
  { global $Proxy ; return $Proxy->PurpleConversationGetName($channelId) ; }


/* chat helpers */

function startsWith($msg , $token) { return (strpos($msg , $token) === 0) ; }

//function contains($msg , $token) { return (strpos($msg , $token) !== false) ; }

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


/* model helpers */

function &getBridgeByCh($channelId)
{
  global $Bridges , $CHANNELS_KEY ; $bridge = [] ;
  foreach ($Bridges as $bridgeName => $aBridge)
    if (array_key_exists($channelId , $aBridge[$CHANNELS_KEY])) $bridge = $aBridge ;
  return $bridge ;
}

function channelStateMsg($channelId)
{
  global $THIS_CHANNEL_MSG , $THIS_BRIDGE_MSG , $BRIDGE_NAME_KEY , $UNBRIDGED_MSG ;

  $aBridge = &getBridgeByCh($channelId) ;
  return "$THIS_CHANNEL_MSG " . ((!$aBridge)?
      $UNBRIDGED_MSG : "$THIS_BRIDGE_MSG '" . $aBridge[$BRIDGE_NAME_KEY] . "'") ;
}

function bridgeStatsMsg($bridgeName)
{
  global $STATS_MSGa , $STATS_ENABLED_MSG , $STATS_DISABLED_MSG , $STATS_MSGb ;
  global $BRIDGE_NAME_KEY , $CHANNELS_KEY , $ENABLED_KEY ;
  global $NO_SUCH_BRIDGE_MSG , $NO_BRIDGES_MSG , $STATS_DELETED_MSG , $Bridges ;

  if (array_key_exists($bridgeName , $Bridges)) $bridge = $Bridges[$bridgeName] ;
  else return "\n" . ((count($Bridges))?
        "$NO_SUCH_BRIDGE_MSG '$bridgeName'" : $NO_BRIDGES_MSG) ;

  $nChannels = count($bridge[$CHANNELS_KEY]) ; $channelsMsg = "" ;
  $nameMsg = "$STATS_MSGa '" . $bridge[$BRIDGE_NAME_KEY] . "' " ;
  $enabledMsg = ((!$nChannels)? $STATS_DELETED_MSG : (($bridge[$ENABLED_KEY])?
      $STATS_ENABLED_MSG : $STATS_DISABLED_MSG) . " $nChannels $STATS_MSGb") ;
  foreach ($bridge[$CHANNELS_KEY] as $channelId => $channelName)
  {
    $accountId = getAccount($channelId) ; $protocol = getProtocol($accountId) ;
    $accountName = getUsername($accountId) ;
    $channelsMsg .= "\n\t$channelName ($protocol:$accountName)" ;
  }
  return "\n$nameMsg$enabledMsg$channelsMsg" ;
}

function getAdmins()
{
  global $Admins ; $adminsMsg = "\nremote admins:" ;
  foreach ($Admins as $accountlId => $admins)
  {
    $adminsMsg .= "\n\t" . getProtocol($accountlId) . " ($accountlId)" ;
    foreach ($admins as $adminNick) $adminsMsg .= "\n\t\t$adminNick" ;
  }
  return $adminsMsg ;
}
