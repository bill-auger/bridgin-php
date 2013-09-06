<?php

if ($isFromAdmin && startsWith($chatIn , $TRIGGER_PREFIX))
{
// NOTE: $tokens and $trigger should mirror their counterparts in 'main.inc'
  $tokens = explode(" " , lstrip($chatIn , $TRIGGER_PREFIX) , 2) ; $trigger = $tokens[0] ;
  $DBG .= "parsed trigger '$TRIGGER_PREFIX$trigger'" .
      ((startsWith($adminChatOut , chatOut($NICK_PREFIX , $BRIDGE_NICK , "") . $UNKNOWN_MSG))?
          " ($UNKNOWN_MSG)" : "") . "\n" ;
}

if ($DEBUGVB && count($theseChannels) > 1)
  $DBG .= "msgOut = '$chatOut'\nrelaying to " . (nActiveChannels($theseChannels) - 1) . " channels\n" ;

echo $DBG ; $DBG = "" ;
