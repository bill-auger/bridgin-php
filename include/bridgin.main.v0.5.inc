<?php

function handleChat($thisAccountId , $senderNick , $chatIn , $thisChannelId , $msgType)
{
  global $NICK_PREFIX , $STAR_PREFIX , $BRIDGE_NICK , $OUTGOING_MESSAGE ;
  global $ENABLED_KEY , $STAR_IRC_COMMANDS , $STRIP_IRC_COMMANDS ;
  global $BRIDGE_NAME_KEY , $CHANNELS_KEY , $CHANNEL_ID_KEY ;
  global $TRIGGER_PREFIX , $ADD_TRIGGER , $REMOVE_TRIGGER , $DISABLE_TRIGGER ;
  global $ENABLE_TRIGGER , $ECHO_TRIGGER , $CHAT_TRIGGER , $BROADCAST_TRIGGER ;
  global $STATUS_TRIGGER , $AUTH_TRIGGER , $HELP_TRIGGER , $EXIT_TRIGGER ;
  global $DEFAULT_BRIDGE_NAME , $Bridges , $Done ;

  $chatOut = $adminChatOut = "" ; $theseChannels = [] ;
  $isFromAdmin = ($msgType == $OUTGOING_MESSAGE) ;
  $thisBridge = &getBridge($thisAccountId , $thisChannelId) ;

  if (!$isFromAdmin) // user chat messages
  {
    if (!$thisBridge || !$thisBridge[$ENABLED_KEY]) return [[] , "" , "" , ""] ;

    // parse and/or strip IRC commands
    $nickPrefix = $NICK_PREFIX ;
    foreach ($STAR_IRC_COMMANDS as $token) if (startsWith($chatIn , $token))
      { $chatIn = lstrip($chatIn , $token) ; $nickPrefix = $STAR_PREFIX ; break ; }
    foreach ($STRIP_IRC_COMMANDS as $token) if (startsWith($chatIn , $token))
      { $chatIn = lstrip($chatIn , $token) ; break ; }

    // set outgoing message and channels
    $chatOut = chatOut($nickPrefix , $senderNick , $chatIn) ;
    $theseChannels = &$thisBridge[$CHANNELS_KEY] ;
  }
  else // admin control messages
  {
    // ignore all local non-trigger chat to avoid infinite loopback
    if (!startsWith($chatIn , $TRIGGER_PREFIX)) return [[] , "" , "" , ""] ;

    // tokenize and switch on admin triggers
    $tokens = explode(" " , lstrip($chatIn , $TRIGGER_PREFIX) , 2) ;
    $trigger = $tokens[0] ;
    $chatIn = $thatBridgeName = ((count($tokens) > 1)? $tokens[1] : "") ;
    $adminChatOut = chatOut($NICK_PREFIX , $BRIDGE_NICK , "") ;
    switch ($trigger)
    {
      case $ADD_TRIGGER:
      {
        if (!$thatBridgeName) $thatBridgeName = $DEFAULT_BRIDGE_NAME ;
        if (!$thisBridge)
        {
          setChannel($thatBridgeName , $thisAccountId , $thisChannelId) ;
          storeSession() ; $adminChatOut .= addResp($thatBridgeName) ;
        }
        elseif (array_key_exists($thatBridgeName , $Bridges) &&
            $thisBridge === $Bridges[$thatBridgeName])
          $adminChatOut .= addExistsResp($thatBridgeName) ;
        else $adminChatOut .= addConflictResp() ;
      } break ;
      case $REMOVE_TRIGGER:
      {
        if ($thisBridge)
        {
          $thisBridgeName = $thisBridge[$BRIDGE_NAME_KEY] ;
          unsetChannel($thisBridgeName , $thisAccountId , $thisChannelId) ;
          storeSession() ; $adminChatOut .= removeResp($thisBridgeName) ;
        }
        else $adminChatOut .= removeUnbridgedResp($thisAccountId , $thisChannelId) ;
      } break ;
      case $DISABLE_TRIGGER:
      case $ENABLE_TRIGGER:
      {
        $isEnable = ($trigger == $ENABLE_TRIGGER) ;
        if (!count($Bridges)) $adminChatOut .= enableNoneResp("") ;
        else if (!$thatBridgeName)
        {
          $adminChatOut .= enableAllResp($isEnable) ;
          foreach ($Bridges as $bridgeName => $aBridge)
              $adminChatOut .= "\n" . enableBridge($bridgeName , $isEnable) ;
        }
        else $adminChatOut .= enableBridge($thatBridgeName , $isEnable) ;
        storeSession() ;
      } break ;
      case $ECHO_TRIGGER: { $adminChatOut = echoResp($senderNick , $chatIn) ; } break ;
      case $CHAT_TRIGGER:
      {
        if ($thisBridge)
        {
          $theseChannels = &$thisBridge[$CHANNELS_KEY] ;
          $chatOut = chatOut($NICK_PREFIX , $senderNick , $chatIn) ; $adminChatOut = "" ;
        }
        else $adminChatOut .= chatUnbridgedResp($thisAccountId , $thisChannelId) ;
      } break ;
      case $BROADCAST_TRIGGER:
      {
        foreach ($Bridges as $bridgeName => $aBridge)
          $theseChannels += $aBridge[$CHANNELS_KEY] ;
        $chatOut = chatOut($NICK_PREFIX , $BRIDGE_NICK , $chatIn) ;
        $adminChatOut .= broadcastResp($theseChannels) ;
      } break ;
      case $STATUS_TRIGGER:
      {
        $adminChatOut .= statusResp($thisAccountId , $thisChannelId , $thatBridgeName) ;
      } break ;
      case $HELP_TRIGGER: { $adminChatOut .= helpResp() ; } break ;
      case $EXIT_TRIGGER: { $Done = true ; $adminChatOut .= exitResp() ; } break ;
      default: { $adminChatOut .= (!$chatIn)? helpResp() : defaultResp() ; }
    }

    // admin response
    postMessage($thisChannelId , $adminChatOut) ;
  }

  // relay chat
  foreach ($theseChannels as $channelKey => $aChannel)
    if ($aChannel[$CHANNEL_ID_KEY] != $thisChannelId)
      postMessage($aChannel[$CHANNEL_ID_KEY] , $chatOut) ;

  return [$theseChannels , $chatOut , $adminChatOut] ;
}
