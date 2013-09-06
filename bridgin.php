<?php

$VERSION = trim(file_get_contents('./VERSION')) ;
require "./include/bridgin.constants.v$VERSION.inc" ;
require "./include/bridgin.functions.v$VERSION.inc" ;
require "./include/bridgin.globals.v$VERSION.inc" ;
require "./include/bridgin.main.v$VERSION.inc" ;


if ($DEBUG) include $DEBUG_HELPERS_FILENAME ;


/* init */

echo "\n$APP_NAME v$VERSION\n\n" ;

// initialize messaging and register event dependencies
if (!initDbusProxy()) exit(0) ;

// assert pidgin is accessible via dbus
if (!getAccounts()) exit(0) ;

// config sanity checks
if (startsWith($NICK_PREFIX , $TRIGGER_PREFIX) ||
    startsWith($STAR_PREFIX , $TRIGGER_PREFIX)) { echo $INVALID_CONFIG_MSG ; exit(0) ; }

// restore previous session
loadSession() ;


/* main */

echo "\n\n$APP_NAME - $READY_MSGa" . ((!count($Bridges))? $READY_MSGb : "") . "\n" ;

// lets rock
while (!$Done)
{
  // trap signals
  $signal = $Dbus->waitLoop($DBUS_POLL_IVL) ;
  if (!$signal instanceof DbusSignal) continue ;

  // trap channel closed signal
  if ($signal->matches($PURPLE_INTERFACE , $CHANNEL_CLOSED_SIGNAL))
  {
if ($DEBUG_SESSION) $found = false ;

    // disable chat for closed channel
    $channelId = $signal->getData() ;
    foreach ($Bridges as $bridgeName => $aBridge)
      foreach ($aBridge[$CHANNELS_KEY] as $channelKey => $aChannel)
        if ($aChannel[$CHANNEL_ID_KEY] == $channelId)
        {
          $Bridges[$bridgeName][$CHANNELS_KEY][$channelKey][$CHANNEL_ID_KEY] = 0 ;
          storeSession() ;

if ($DEBUG_SESSION) $found = true ;
        }

if ($DEBUG_SESSION) echo "$CHANNEL_CLOSED_SIGNAL ($channelId) - " . (($found)? "" : "un") . "bridged\n" ;

    continue ;
  }

  // trap chat signals
  if (!$signal->matches($PURPLE_INTERFACE , $IM_METHOD) &&
      !$signal->matches($PURPLE_INTERFACE , $CHAT_METHOD)) continue ;

  // parse message data
  $msgData = $signal->getData()->getData() ;
  $accountId = $msgData[0] ; $senderNick = $msgData[1] ;
  $chatIn    = $msgData[2] ; $channelId  = $msgData[3] ;
  $msgFlags  = $msgData[4] ;
  $msgType   = $MESSGAE_FLAGS[(int)$msgFlags] ; // TODO: flags are likely OR'ed

  // validate message data
  if (!$accountId || !$senderNick || !$chatIn || !$channelId || !$msgType)
    { echo $BOGUS_DATA_MSG ; continue ; } else if (!isConnected($accountId)) continue ;

if ($DEBUG)
{
  include $DEBUG_IN_FILENAME ;
  $dbg = handleChat($accountId , $senderNick , $chatIn , $channelId , $msgType) ;
  $theseChannels = $dbg[0] ; $chatOut = $dbg[1] ; $adminChatOut = $dbg[2] ;
  include $DEBUG_OUT_FILENAME ;
}
else

  // invoke chat handler
  handleChat($accountId , $senderNick , $chatIn , $channelId , $msgType) ;
}

exit(0) ;

?>
