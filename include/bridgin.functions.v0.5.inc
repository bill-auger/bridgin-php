<?php

/* persistence helpers */

function validateSession() // see /* schema */ section in 'constants.inc'
{
global $DEBUG_SESSION ; if ($DEBUG_SESSION) return DBGvalidateSession() ;

  global $Bridges , $BRIDGE_NAME_KEY , $ENABLED_KEY , $CHANNELS_KEY , $PROTOCOL_KEY ;
  global $ACCOUNT_ID_KEY , $CHANNEL_ID_KEY , $CHANNEL_NAME_KEY , $USERNAME_KEY ;

  if (!is_array($Bridges)) return false ;

  foreach ($Bridges as $bridgeName => $aBridge)
  {
    if (!is_string($bridgeName) || !is_array($aBridge)) return false ;

    if (!array_key_exists($BRIDGE_NAME_KEY , $aBridge) ||
        !array_key_exists($ENABLED_KEY , $aBridge) ||
        !array_key_exists($CHANNELS_KEY , $aBridge)) return false ;

    if (!is_string($aBridge[$BRIDGE_NAME_KEY]) ||
        !is_bool($aBridge[$ENABLED_KEY]) ||
        !is_array($aBridge[$CHANNELS_KEY])) return false ;

    foreach ($aBridge[$CHANNELS_KEY] as $channelKey => $aChannel)
      if (!is_string($channelKey) ||
          !is_string($aChannel[$PROTOCOL_KEY]) ||
          !is_int($aChannel[$ACCOUNT_ID_KEY]) ||
          !is_int($aChannel[$CHANNEL_ID_KEY]) ||
          !is_string($aChannel[$CHANNEL_NAME_KEY]) ||
          !is_string($aChannel[$USERNAME_KEY])) return false ;
  }

  return true ;
}

function loadSession()
{
global $DEBUG_SESSION ;

  global $Bridges , $BRIDGES_FILENAME , $LOAD_FAIL_MSG , $LOAD_CONFIG_MSG ;
  global $CHANNELS_KEY , $CHANNEL_NAME_KEY , $ACCOUNT_ID_KEY , $CHANNEL_ID_KEY ;
  global $INIT_CONFIG_MSG , $LIST_BRIDGES_MSG ;

if ($DEBUG_SESSION) echo "\nloadSession() file_exists($BRIDGES_FILENAME)=" . (file_exists($BRIDGES_FILENAME)) . "\n" ;

  $hasSessionChanged = false ;
  if (file_exists($BRIDGES_FILENAME))
  {
    $Bridges = unserialize(file_get_contents($BRIDGES_FILENAME)) ;
    if (validateSession()) echo $LOAD_CONFIG_MSG ;
    else { $Bridges = [] ; echo $LOAD_FAIL_MSG ; }

    // clobber any orphaned channel ids
    foreach ($Bridges as $bridgeName => $aBridge)
      foreach ($aBridge[$CHANNELS_KEY] as $channelKey => $aChannel)
      {
        $channelName = $aChannel[$CHANNEL_NAME_KEY]  ;
        $accountId   = $aChannel[$ACCOUNT_ID_KEY] ;
        $prevId      = $aChannel[$CHANNEL_ID_KEY] ;
        $newId       = getChannelIdByName($channelName , $accountId) ;
        if ($newId != $prevId)
        {
          $Bridges[$bridgeName][$CHANNELS_KEY][$channelKey][$CHANNEL_ID_KEY] = $newId ;
          $hasSessionChanged = true ;
        }

if ($DEBUG_SESSION) echo "loadSession() $channelName  ((newId=$newId) " . (($newId != $prevId)? "!=" : "==") ." (prevId=$prevId))" . (($newId != $prevId)? " - clobbering" : "") . "\n" ;
      }
  }
  else { $hasSessionChanged = true ; echo $INIT_CONFIG_MSG ; }
  if ($hasSessionChanged) storeSession() ;

  // display session data
  if (!count($Bridges)) echo bridgeStatsMsg("") ;
  else
  {
    echo count($Bridges) . $LIST_BRIDGES_MSG ;
    foreach ($Bridges as $name => $aBridge) echo bridgeStatsMsg($name) ;
  }

if ($DEBUG_SESSION) { echo "loadSession() Bridges=" ; var_dump($Bridges) ; }
}

function storeSession()
{
  global $BRIDGES_FILENAME , $Bridges ;
  return file_put_contents($BRIDGES_FILENAME , serialize($Bridges)) ;
}


/* dbus helpers */

function initDbusProxy()
{
  global $Dbus , $Proxy ;
  global $PURPLE_INTERFACE , $PURPLE_SERVICE , $PURPLE_OBJECT ;
  global $CHANNEL_CLOSED_SIGNAL , $IM_METHOD , $CHAT_METHOD ;
  global $DBUS_OK_MSG , $DBUS_FATAL_MSG ;
  try
  {
    $Dbus = new Dbus(Dbus::BUS_SESSION) ;
    $Dbus->addWatch($PURPLE_INTERFACE , $CHANNEL_CLOSED_SIGNAL) ;
// TODO: postMessage() needs can not yet distinguish im from chat channels
//    $Dbus->addWatch($PURPLE_INTERFACE , $IM_METHOD) ;
    $Dbus->addWatch($PURPLE_INTERFACE , $CHAT_METHOD) ;
    $Proxy = $Dbus->createProxy($PURPLE_SERVICE , $PURPLE_OBJECT , $PURPLE_INTERFACE) ;
  }
  catch(Exception $ex) {}

  echo ($Proxy)? $DBUS_OK_MSG : $DBUS_FATAL_MSG ; return $Proxy ;
}

function getAccounts()
{
  global $PURPLE_OK_MSG , $PURPLE_ERROR_MSG , $LIST_ACCOUNTS_MSGa , $LIST_ACCOUNTS_MSGb ;
  global $Proxy ; $accounts = [] ;

  try { $accounts = $Proxy->PurpleAccountsGetAll() ; } catch(Exception $ex) {}

  if (!$accounts) { echo $PURPLE_ERROR_MSG ; return false ; }
  else
  {
    $accounts = $accounts->getData() ; $nAccounts = count($accounts) ;
    echo $PURPLE_OK_MSG . "$LIST_ACCOUNTS_MSGa $nAccounts $LIST_ACCOUNTS_MSGb\n" ;
    foreach ($accounts as $accountId)
      echo "\n\t" . getProtocol($accountId) . " (" . getUsername($accountId) . ")\n\t\t" .
          ((isConnected($accountId))? "" : "not ") . "connected" ;
    return true ;
  }
}

function isConnected($accountId)
{
  global $Proxy ;

  try { return $Proxy->PurpleAccountIsConnected($accountId) ; }
  catch(Exception $ex) { echo $DBUS_ERROR_MSG ; return false ; }
}

function getChannelIdByName($channelName , $accountId)
{
  global $Proxy , $PURPLE_CONV_TYPE_ANY ;

  $type = $PURPLE_CONV_TYPE_ANY = 4 ;
  return $Proxy->PurpleFindConversationWithAccount($type , $channelName , $accountId) ;
}

function getProtocol($accountId)
  { global $Proxy ; return $Proxy->PurpleAccountGetProtocolName($accountId) ; }

function getUsername($accountId)
  { global $Proxy ; return $Proxy->PurpleAccountGetUsername($accountId) ; }

function getChannelName($channelId)
  { global $Proxy ; return $Proxy->PurpleConversationGetName($channelId) ; }

function postMessage($channelId , $msg)
{
  global $Proxy ; if (!$channelId || !$msg) return ;
// TODO: need to distinguish im from chat channels
//  $Proxy->PurpleConvImSend($Proxy->PurpleConvIm($channelId) , $msg) ;
  $Proxy->PurpleConvChatSend($Proxy->PurpleConvChat($channelId) , $msg) ;
}

/* chat helpers */

function startsWith($msg , $token) { return (strpos($msg , $token) === 0) ; }

function lstrip($msg , $token) { return substr($msg , strlen($token)) ; }

function chatOut($prefix , $nick , $chatOut)
{
  global $NICK_POSTFIX ;
  return $prefix . $nick . $NICK_POSTFIX . $chatOut ;
}


/* admin trigger responses */

function addResp($bridgeName)
{
  global $CH_SET_MSG ;
  return "$CH_SET_MSG '$bridgeName'" . bridgeStatsMsg($bridgeName) ;
}

function addExistsResp($bridgeName)
{
  global $THIS_CHANNEL_MSG , $CHANNEL_EXISTS_MSG ;
  return "$THIS_CHANNEL_MSG $CHANNEL_EXISTS_MSG '$bridgeName'" ;
}

function addConflictResp() { global $BRIDGE_CONFLICT_MSG ; return $BRIDGE_CONFLICT_MSG ; }

function removeResp($bridgeName)
{
  global $CHANNEL_REMOVED_MSG , $BRIDGE_REMOVED_MSG ; $resp = $CHANNEL_REMOVED_MSG ;

  if (array_key_exists($bridgeName , $Bridges)) $resp .= bridgeStatsMsg($bridgeName) ;
  else $resp .= $BRIDGE_REMOVED_MSG ;
  return $resp ;
}

function removeUnbridgedResp($accountId , $channelId)
  { return channelStateMsg($accountId , $channelId) ; }

function enableNoneResp($bridgeName) { return bridgeStatsMsg($bridgeName) ; }

function enableAllResp($isEnable)
{
  global $ENABLING_ALL_MSG , $DISABLING_ALL_MSG ;
  return ($isEnable)? $ENABLING_ALL_MSG : $DISABLING_ALL_MSG ;
}

function enableResp($bridgeName , $enabledMsg)
  { global $ENABLE_MSG ; return "$ENABLE_MSG '$bridgeName' $enabledMsg" ; }

function echoResp($nick , $msg)
  { global $NICK_PREFIX ; return chatOut($NICK_PREFIX , $nick , $msg) ; }

function chatResp() { return "" ; }

function chatUnbridgedResp($accountId , $channelId)
  { return channelStateMsg($accountId , $channelId) ; }

function broadcastResp($channels)
{
  global $BROADCAST_MSGa , $BROADCAST_MSGb ;
  return $BROADCAST_MSGa . (nActiveChannels($channels) - 1) . $BROADCAST_MSGb ;
}

function statusResp($accountId , $channelId , $bridgeName)
{
  global $Bridges ;

  $resp = channelStateMsg($accountId , $channelId) ;
  if (!count($Bridges)) $resp .= "\n" . bridgeStatsMsg("") ;
  else if ($bridgeName) $resp .= "\n" . bridgeStatsMsg($bridgeName) ;
  else foreach ($Bridges as $name => $aBridge) $resp .= bridgeStatsMsg($name) ;
  return $resp ;
}

function helpResp()
{
  global $HELP_MSG , $HELP_MSGS ; $resp = $HELP_MSG ;
  foreach ($HELP_MSGS as $trigger => $desc) $resp .= "\n$trigger\n\t=> $desc" ;
  return $resp ;
}

function exitResp() { global $EXIT_MSG ; return $EXIT_MSG ; }

function defaultResp()
{
  global $UNKNOWN_MSG , $TRIGGER_PREFIX ;
  return "$UNKNOWN_MSG '$TRIGGER_PREFIX$trigger'" ;
}


/* model helpers */

function makeChannelKey($thisAccountId , $thisChannelId)
{
  global $CHANNEL_KEY_SEPERATOR ;
  return $thisAccountId . $CHANNEL_KEY_SEPERATOR . getChannelName($thisChannelId) ;
}

function &getBridge($accountId , $channelId)
{
  global $Bridges , $CHANNELS_KEY , $CHANNEL_ID_KEY ;

  $channelKey = makeChannelKey($accountId , $channelId) ;
  foreach ($Bridges as $bridgeName => $aBridge)
  {
    if (!array_key_exists($channelKey , $aBridge[$CHANNELS_KEY])) continue ;

    if ($Bridges[$bridgeName][$CHANNELS_KEY][$channelKey][$CHANNEL_ID_KEY] != $channelId)
    {
      $Bridges[$bridgeName][$CHANNELS_KEY][$channelKey][$CHANNEL_ID_KEY] = $channelId ;
      storeSession() ;
    }
    return $aBridge ;
  }
  $aBridge = [] ; return $aBridge ;
}

function setChannel($bridgeName , $accountId , $channelId)
{
  global $Bridges , $BRIDGE_NAME_KEY , $ENABLED_KEY , $CHANNELS_KEY , $PROTOCOL_KEY ;
  global $ACCOUNT_ID_KEY , $CHANNEL_ID_KEY , $CHANNEL_NAME_KEY , $USERNAME_KEY ;

  if (!array_key_exists($bridgeName , $Bridges))
  {
    $Bridges[$bridgeName]                   = [] ;
    $Bridges[$bridgeName][$BRIDGE_NAME_KEY] = $bridgeName ;
    $Bridges[$bridgeName][$ENABLED_KEY]     = true ;
    $Bridges[$bridgeName][$CHANNELS_KEY]    = [] ;
  }

  $channel                    = [] ;
  $channel[$PROTOCOL_KEY]     = getProtocol($accountId) ;
  $channel[$ACCOUNT_ID_KEY]   = $accountId ;
  $channel[$CHANNEL_ID_KEY]   = $channelId ;
  $channel[$CHANNEL_NAME_KEY] = getChannelName($channelId) ;
  $channel[$USERNAME_KEY]     = getUsername($accountId) ;

  $channelKey = makeChannelKey($accountId , $channelId) ;
  $Bridges[$bridgeName][$CHANNELS_KEY][$channelKey] = $channel ;
}

function unsetChannel($bridgeName , $accountId , $channelId)
{
  global $Bridges , $CHANNELS_KEY ;

  $channelKey = makeChannelKey($accountId , $channelId) ;
  unset($Bridges[$bridgeName][$CHANNELS_KEY][$channelKey]) ;
  if (!count($Bridges[$bridgeName][$CHANNELS_KEY])) unset($Bridges[$bridgeName]) ;
}

function enableBridge($bridgeName , $isEnable)
{
  global $Bridges , $ENABLED_KEY , $ENABLED_MSG , $DISABLED_MSG ;

  if (!$Bridges[$bridgeName]) return enableNoneResp($bridgeName) ;

  $Bridges[$bridgeName][$ENABLED_KEY] = $isEnable ;
  return enableResp($bridgeName , (($isEnable)? $ENABLED_MSG : $DISABLED_MSG)) ;
}

function nActiveChannels($channels)
{
  global $CHANNEL_ID_KEY ; $nChannels = 0 ;
  foreach ($channels as $channelKey => $aChannel)
    if ($aChannel[$CHANNEL_ID_KEY]) ++$nChannels ;
  return $nChannels ;
}

function channelStateMsg($accountId , $channelId)
{
  global $THIS_CHANNEL_MSG , $THIS_BRIDGE_MSG , $BRIDGE_NAME_KEY , $UNBRIDGED_MSG ;

  $aBridge = &getBridge($accountId , $channelId) ;
  return "$THIS_CHANNEL_MSG " . ((!$aBridge)?
      $UNBRIDGED_MSG : "$THIS_BRIDGE_MSG '" . $aBridge[$BRIDGE_NAME_KEY] . "'") ;
}

function bridgeStatsMsg($bridgeName)
{
  global $NO_SUCH_BRIDGE_MSG , $NO_BRIDGES_MSG , $STATS_DELETED_MSG , $Bridges ;
  global $BRIDGE_NAME_KEY , $CHANNELS_KEY , $ENABLED_KEY , $CHANNEL_KEY_SEPERATOR ;
  global $STATS_MSGa , $STATS_ENABLED_MSG , $STATS_DISABLED_MSG , $STATS_MSGb ;
  global $CHANNEL_NAME_KEY , $PROTOCOL_KEY , $USERNAME_KEY , $CHANNEL_ID_KEY ;

  if (array_key_exists($bridgeName , $Bridges)) $bridge = $Bridges[$bridgeName] ;
  else return ((count($Bridges))? "\n$NO_SUCH_BRIDGE_MSG '$bridgeName'" : "\n$NO_BRIDGES_MSG") ;

  $nChannels = count($bridge[$CHANNELS_KEY]) ; $channelsMsg = "" ;
  $nameMsg = "$STATS_MSGa '" . $bridge[$BRIDGE_NAME_KEY] . "' " ;
  $enabledMsg = ((!$nChannels)? $STATS_DELETED_MSG : (($bridge[$ENABLED_KEY])?
      $STATS_ENABLED_MSG : $STATS_DISABLED_MSG) . " $nChannels $STATS_MSGb") ;

  foreach ($bridge[$CHANNELS_KEY] as $channelKey => $aChannel)
  {
    $channelsMsg .= "\n\t" .
        $aChannel[$CHANNEL_NAME_KEY] . " on " .
        $aChannel[$PROTOCOL_KEY] . " as '" .
        $aChannel[$USERNAME_KEY] . "'" .
        (($aChannel[$CHANNEL_ID_KEY])? " (active)" : " (inactive)") ;
  }
  return "\n$nameMsg$enabledMsg$channelsMsg" ;
}
