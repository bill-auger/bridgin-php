<?php

$APP_NAME = 'skype4pidgin2irc' ; $VERSION = '0.3' ;
$CONSTANTS_FILENAME = "skype4pidgin2irc.constants.v$VERSION.dbg.inc" ;
$FUNCTIONS_FILENAME = "skype4pidgin2irc.functions.v$VERSION.dbg.inc" ;
$GLOBALS_FILENAME   = "skype4pidgin2irc.globals.v$VERSION.dbg.inc" ;
$MAIN_FILENAME      = "skype4pidgin2irc.main.v$VERSION.dbg.inc" ;
error_reporting(E_ALL ^ E_NOTICE) ; // suppress 'Undefined index' PHP Notice


include $CONSTANTS_FILENAME ;
include $FUNCTIONS_FILENAME ;
include $GLOBALS_FILENAME ;


function handleChat($accountId , $senderNick , $chatIn , $thisCh)
{
  global $MAIN_FILENAME ; // TODO: and many more

  include $MAIN_FILENAME ;
  return [$chatOut , $adminChatOut] ; // $chatOut , $adminChatOut in $MAIN_FILENAME
}

?>
