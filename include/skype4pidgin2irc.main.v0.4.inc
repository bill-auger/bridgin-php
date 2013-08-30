<?php

/* main */

$NickPrefix = $NICK_PREFIX ; $theseChannels = [] ;
$chatOut = $adminChatOut = chatOut($BRIDGE_NICK , "") ;
$isFromAdmin = ($msgType == $LOCAL_OUTGOING_MESSAGE) ;
$thisBridge = &getBridgeByCh($thisChannelId) ;


include $DEBUG_IN_FILENAME ;


if (!$isFromAdmin) // user chat messages
{
  if (!$thisBridge || !$thisBridge[$ENABLED_KEY]) return ["" , ""] ;

  // parse and/or strip IRC commands
  foreach ($STAR_IRC_COMMANDS as $token) if (startsWith($chatIn , $token))
    { $chatIn = lstrip($chatIn , $token) ; $NickPrefix = $STAR_PREFIX ; break ; }
  foreach ($STRIP_IRC_COMMANDS as $token) if (startsWith($chatIn , $token))
    { $chatIn = lstrip($chatIn , $token) ; break ; }

  // set outgoing message and channels
  $chatOut = chatOut($senderNick , $chatIn) ;
  $theseChannels = &$thisBridge[$CHANNELS_KEY] ;
}
else // admin control messages
{
  if (!startsWith($chatIn , $TRIGGER_PREFIX)) return ["" , ""] ;

  // tokenize and switch on admin triggers
  $tokens = explode(" " , lstrip($chatIn , $TRIGGER_PREFIX) , 2) ;
  $trigger = $tokens[0] ;
  $chatIn = $thatBridgeName = (count($tokens) > 1)? $tokens[1] : "" ;
  switch ($trigger)
  {
    case $ADD_TRIGGER:
    {
      if (!$thatBridgeName) $thatBridgeName = $DEFAULT_BRIDGE_NAME ;
      if (!$thisBridge)
      {
        if (!array_key_exists($thatBridgeName , $Bridges))
        {
          $Bridges[$thatBridgeName]                   = [] ;
          $Bridges[$thatBridgeName][$BRIDGE_NAME_KEY] = $thatBridgeName ;
          $Bridges[$thatBridgeName][$ENABLED_KEY]     = true ;
          $Bridges[$thatBridgeName][$CHANNELS_KEY]    = [] ;
        }
        $Bridges[$thatBridgeName][$CHANNELS_KEY][$thisChannelId] = getChannelName($thisChannelId) ;
        $adminChatOut .= "$CH_SET_MSG '$thatBridgeName'" . bridgeStatsMsg($thatBridgeName) ;
      }
      elseif ($thisBridge === $Bridges[$thatBridgeName])
        $adminChatOut .= "$THIS_CHANNEL_MSG $CHANNEL_EXISTS_MSG '$thatBridgeName'" ;
      else $adminChatOut .= $BRIDGE_CONFLICT_MSG ;
    } break ;
    case $REMOVE_TRIGGER:
    {
      if ($thisBridge)
      {
        $thisBridgeName = $thisBridge[$BRIDGE_NAME_KEY] ;
        unset($Bridges[$thisBridgeName][$CHANNELS_KEY][$thisChannelId]) ;
        $adminChatOut .= "$CHANNEL_REMOVED_MSG" . bridgeStatsMsg($thisBridgeName) ;
        if (!count($Bridges[$thisBridgeName][$CHANNELS_KEY]))
          unset($Bridges[$thisBridgeName]) ;
      }
      else $adminChatOut .= channelStateMsg($thisChannelId) ;
    } break ;
    case $DISABLE_TRIGGER:
    case $ENABLE_TRIGGER:
    {
      $enableBridge = function($bridgeName , $isEnable)
      {
        global $Bridges , $ENABLED_KEY , $ENABLE_MSG , $ENABLED_MSG , $DISABLED_MSG ;
        if (!$Bridges[$bridgeName]) return bridgeStatsMsg($bridgeName) ;
        else if ($Bridges[$bridgeName][$ENABLED_KEY] = $isEnable)
              return "$ENABLE_MSG '$bridgeName' $ENABLED_MSG" ;
        else return "$ENABLE_MSG '$bridgeName' $DISABLED_MSG" ;
      } ;

      $isEnable = ($trigger == $ENABLE_TRIGGER) ;
      if (!count($Bridges)) $adminChatOut .= bridgeStatsMsg("") ;
      else if (!$thatBridgeName) foreach ($Bridges as $bridgeName => $aBridge)
            $adminChatOut .= "\n" . $enableBridge($bridgeName , $isEnable) ;
      else $adminChatOut .= $enableBridge($thatBridgeName , $isEnable) ;
    } break ;
    case $STATS_TRIGGER:
    {
      $adminChatOut .= channelStateMsg($thisChannelId) ;
      if (!count($Bridges)) $adminChatOut .= bridgeStatsMsg("") ;
      else if (!$thatBridgeName) foreach ($Bridges as $bridgeName => $aBridge)
            $adminChatOut .= bridgeStatsMsg($bridgeName) ;
      else $adminChatOut .= bridgeStatsMsg($thatBridgeName) ;
      $adminChatOut .= getAdmins() ;
    } break ;
    case $ECHO_TRIGGER: { $adminChatOut = chatOut($senderNick , $chatIn) ; } break ;
    case $CHAT_TRIGGER:
    {
      if ($thisBridge)
      {
        $theseChannels = &$thisBridge[$CHANNELS_KEY] ;
        $chatOut = chatOut($senderNick , $chatIn) ; $adminChatOut = "" ;
      }
      else $adminChatOut .= channelStateMsg($thisChannelId) ;
    } break ;
    case $BROADCAST_TRIGGER:
    {
      foreach ($Bridges as $bridgeName => $aBridge)
        $theseChannels += $aBridge[$CHANNELS_KEY] ;
      $chatOut .= $chatIn ;
      $adminChatOut .= $BROADCAST_MSGa . (count($theseChannels) - 1) . $BROADCAST_MSGb ;
    } break ;
    case $EXIT_TRIGGER: { $Done = true ; $adminChatOut .= $EXIT_MSG ; } break ;
    default: { $adminChatOut .= "$UNKNOWN_MSG '$TRIGGER_PREFIX$trigger'" ; }
  }

  // admin response
  postMessage($thisChannelId , $adminChatOut) ;
}


include $DEBUG_OUT_FILENAME ;


// relay chat
foreach ($theseChannels as $channelId => $channelName)
  if ($channelId != $thisChannelId) postMessage($channelId , $chatOut) ;

return [$chatOut , $adminChatOut] ;
