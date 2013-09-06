<?php

$thisAccountId = $accountId ; $thisChannelId = $channelId ;

// NOTE: $isFromAdmin and $thisBridge should mirror their counterparts in 'main.inc'
$isFromAdmin = ($msgType == $OUTGOING_MESSAGE) ;
$thisBridge = &getBridge($thisAccountId , $thisChannelId) ;

$thisBridgeName = (array_key_exists($BRIDGE_NAME_KEY , $thisBridge))? $thisBridge[$BRIDGE_NAME_KEY] : "" ;

$convtype = (($signal->matches($PURPLE_INTERFACE , $IM_METHOD))? $IM_METHOD :
            (($signal->matches($PURPLE_INTERFACE , $CHAT_METHOD))? $CHAT_METHOD : "" )) ;
$DBG .= "\n$convtype " .
    (($thisBridgeName)? "from bridge '$thisBridgeName'" : "(unbridged)") . " " .
    ((!$isFromAdmin)? "from user $senderNick" :
        "from admin $senderNick" . ((!startsWith($chatIn , $TRIGGER_PREFIX))?
            " - not trigger" : " - got trigger")) . "\n" ;

if ($DEBUGVB)
{
  $protocol = getProtocol($thisAccountId) ;
  $accountName = getUsername($thisAccountId) ;
  $channelName = getChannelName($thisChannelId) ;
  $DBG .=   "\taccountId  = $thisAccountId ($protocol:$accountName)" .
          "\n\tchatInCh   = $thisChannelId ($channelName)" .
          "\n\tsenderNick = $senderNick" .
          "\n\tchatIn     = '$chatIn'" .
          "\n\tmsgType    = $msgFlags ($msgType)\n" ;
}

if (!$isFromAdmin) // normal chat
{
  if (!$thisBridge)
    $DBG .= "from unbridged channel - dropping message\n" ;

  if ($thisBridge && !$thisBridge[$ENABLED_KEY])
    $DBG .= "bridge disabled - dropping message\n" ;

  if ($thisBridge && count($thisBridge[$CHANNELS_KEY]) < 2)
    $DBG .= "bridge '" . $thisBridgeName . "' has no receivers - dropping message\n" ;

  if ($DEBUG_ALL && (!$thisBridge || !$thisBridge[$ENABLED_KEY]))
    { echo $DBG ; $DBG = "" ; }
}
else if (startsWith($chatIn , chatOut($NICK_PREFIX , $BRIDGE_NICK , ""))) // admin commands
  $DBG .= "bridge chat - dropping message\n" ;
else if (startsWith($chatIn , $NICK_PREFIX) || startsWith($chatIn , $STAR_PREFIX))
  $DBG .= "loopback - dropping message\n" ;
else if (!startsWith($chatIn , $TRIGGER_PREFIX))
  $DBG .= "admin chat - dropping message\n" ;

if ($DEBUG_ALL) { echo $DBG ; $DBG = "" ; }
