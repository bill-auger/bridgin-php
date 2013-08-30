<?php

$APP_NAME = 'skype4pidgin2irc' ; $VERSION = '0.4' ;
$CONSTANTS_FILENAME = "skype4pidgin2irc.constants.v$VERSION.inc" ;
$FUNCTIONS_FILENAME = "skype4pidgin2irc.functions.v$VERSION.inc" ;
$GLOBALS_FILENAME   = "skype4pidgin2irc.globals.v$VERSION.inc" ;
$MAIN_FILENAME      = "skype4pidgin2irc.main.v$VERSION.inc" ;
error_reporting(E_ALL ^ E_NOTICE) ; // suppress 'Undefined index' PHP Notice


require $CONSTANTS_FILENAME ;
require $FUNCTIONS_FILENAME ;
require $GLOBALS_FILENAME ;


function handleChat($accountId , $senderNick , $chatIn , $thisCh)
{
  global $MAIN_FILENAME ; // TODO: and many more

  require $MAIN_FILENAME ;
  return [$chatOut , $adminChatOut] ; // $chatOut , $adminChatOut in $MAIN_FILENAME
}

?>
