<?php

if ($isFromAdmin)
  $DBG .= "parsed trigger '$TRIGGER_PREFIX$trigger'" .
      ((startsWith($adminChatOut , chatOut($BRIDGE_NICK , "") . $UNKNOWN_MSG))?
          " ($UNKNOWN_MSG)" : "") . "\n" ;

if ($DEBUGVB && count($theseChannels) > 1)
  $DBG .= "msgOut = '$chatOut'\nrelaying to " . (count($theseChannels) - 1) . " channels\n" ;

echo $DBG ; $DBG = "" ;
