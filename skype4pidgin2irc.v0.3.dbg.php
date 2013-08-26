<?php

$APP_NAME = 'skype4pidgin2irc' ; $VERSION = '0.3' ;
$CONSTANTS_FILENAME = "skype4pidgin2irc.constants.v$VERSION.dbg.inc" ;
$FUNCTIONS_FILENAME = "skype4pidgin2irc.functions.v$VERSION.dbg.inc" ;
$GLOBALS_FILENAME   = "skype4pidgin2irc.globals.v$VERSION.dbg.inc" ;
$MAIN_FILENAME      = "skype4pidgin2irc.main.v$VERSION.dbg.inc" ;
error_reporting(E_ALL ^ E_NOTICE) ; // suppress 'Undefined index' PHP Notice


require $CONSTANTS_FILENAME ;
require $FUNCTIONS_FILENAME ;


/* init */

// initialize state
require $GLOBALS_FILENAME ;

// initialize messaging and register event dependencies
$Dbus = new Dbus(Dbus::BUS_SESSION) ;
$Dbus->addWatch($INTERFACE, $CHAT_METHOD) ;
$Proxy = $Dbus->createProxy($PURPLE_SERVICE , $PURPLE_OBJECT , $PURPLE_INTERFACE) ;

// lets rock
echo "\n$APP_NAME v$VERSION\n\n$USAGE_MSGa $TRIGGER_PREFIX$ADD_TRIGGER$USAGE_MSGb\n\n" ;
while (!$Done)
{
  // trap signals
  $signal = $Dbus->waitLoop(1000) ;
  if (!$signal instanceof DbusSignal) continue ;
  if (!$signal->matches($INTERFACE , $CHAT_METHOD)) continue ;

  // parse data
  $data = $signal->getData()->getData() ;
  $accountId = $data[0] ; $senderNick    = $data[1] ;
  $chatIn    = $data[2] ; $thisChannelId = $data[3] ;

  // validate data
  if (!$accountId || !$senderNick || !$chatIn || !$thisChannelId ||
      !$Proxy->PurpleAccountIsConnected($accountId))
        { echo $BOGUS_DATA_MSG ; return ["" , ""] ; }

  require $MAIN_FILENAME ;
}

?>
