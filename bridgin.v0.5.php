<?php

$APP_NAME           = 'bridgin' ; $VERSION = '0.5' ;
$CONSTANTS_FILENAME = "./include/bridgin.constants.v$VERSION.inc" ;
$FUNCTIONS_FILENAME = "./include/bridgin.functions.v$VERSION.inc" ;
$GLOBALS_FILENAME   = "./include/bridgin.globals.v$VERSION.inc" ;
$MAIN_FILENAME      = "./include/bridgin.main.v$VERSION.inc" ;


require $CONSTANTS_FILENAME ;
require $FUNCTIONS_FILENAME ;


if ($DEBUG) include $DEBUG_HELPERS_FILENAME ;


/* init */

echo "\n$APP_NAME v$VERSION\n\n" ;

// initialize state
require $GLOBALS_FILENAME ;
if (loadConfig()) echo "$LOAD_CONFIG_MSG\n" ; else echo "$LOAD_FAIL_MSG\n" ;

// config sanity checks
if (startsWith($NICK_PREFIX , $TRIGGER_PREFIX) ||
    startsWith($STAR_PREFIX , $TRIGGER_PREFIX)) { echo $INVALID_CONFIG_MSG ; exit() ; }

// initialize messaging and register event dependencies
initDbusProxy() ;
if ($Proxy) echo "$DBUS_OK_MSG\n" ; else { echo "$DBUS_FATAL_MSG\n" ; exit() ; }

// assert pidgin is accessible via dbus
if ($accounts = getAccounts()) echo "$PURPLE_OK_MSG\n" ;
else { echo "$PURPLE_ERROR_MSG\n\n" ; exit() ; }

// list pidgin accounts
echo "$LIST_ACCOUNTS_MSGa " . count($accounts->getData()) . " $LIST_ACCOUNTS_MSGb\n\n" ;
foreach ($accounts->getData() as $accountId)
  echo "\t" . getProtocol($accountId) . " (" . getUsername($accountId) . ")\n\t\t" .
      ((isConnected($accountId))? "" : "not ") . "connected\n" ;

// lets rock
echo "\n$APP_NAME - $READY_MSGa" . ((!count($Bridges))? $READY_MSGb : "") . "\n\n" ;
while (!$Done)
{
  // trap signals
  $signal = $Dbus->waitLoop($DBUS_POLL_IVL) ;
  if (!$signal instanceof DbusSignal) continue ;
  if (!$signal->matches($PURPLE_INTERFACE , $CHAT_METHOD)) continue ;

  // parse data
  $msgData = $signal->getData()->getData() ;
  $thisAccountId = $msgData[0] ; $senderNick    = $msgData[1] ;
  $chatIn        = $msgData[2] ; $thisChannelId = $msgData[3] ;
  $msgFlags      = $msgData[4] ;
  $msgType       = $MESSGAE_FLAGS[(int)$msgFlags] ; // TODO: flags are likely OR'ed

  // validate data
  if (!$thisAccountId || !$senderNick || !$chatIn || !$thisChannelId ||
      !isConnected($thisAccountId)) { echo $BOGUS_DATA_MSG ; return ["" , ""] ; }

  require $MAIN_FILENAME ;
}

exit() ;

?>
