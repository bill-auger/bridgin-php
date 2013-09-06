<?php

$VERSION = trim(file_get_contents('./VERSION')) ;
require "./include/bridgin.constants.v$VERSION.inc" ;
require "./include/bridgin.functions.v$VERSION.inc" ;
require "./include/bridgin.globals.v$VERSION.inc" ;
require "./include/bridgin.main.v$VERSION.inc" ;


function doTest($thisAccountId , $senderNick , $chatIn , $thisChannelId , $msgType , $testName , $channelsExp , $chatOutExp , $adminChatOutExp)
{
  $res = handleChat($thisAccountId , $senderNick , $chatIn , $thisChannelId , $msgType) ;
  echo "test '$testName'\n" . (($res == [$channelsExp , $chatOutExp , $adminChatOutExp])? "pass" : "fail") . "\n\n" ;
}

?>
