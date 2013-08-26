<?php
$DBG = "" ;


/* main */

  // set loop invariants
  $NickPrefix = $NICK_PREFIX ; $theseChannels = [] ;
  $chatOut = $adminChatOut = chatOut($BRIDGE_NICK , "") ;

  $isFromAdmin = in_array($senderNick , $ADMIN_NICKS) ;
  $thisBridge = &getBridgeByCh($thisChannelId) ;

if ($DEBUG)   $DBG .= "\n$CHAT_METHOD " ; $thisBridgeName = ($thisBridge && $thisBridge[$NAME_KEY])? $thisBridge[$NAME_KEY] : "" ;
if ($DEBUG)   $DBG .= (($thisBridgeName)? "from bridge '$thisBridgeName'" : "(unbridged)") ;
if ($DEBUG)   $DBG .= " " . ((!$isFromAdmin)? "from user $senderNick" : "from admin $senderNick" . ((!startsWith($chatIn , $TRIGGER_PREFIX))? " - not trigger" : " - got trigger")) ; if ($DEBUG) $DBG .= "\n" ;
if ($DEBUGVB) $DBG .= "\taccountId  = $accountId\n\tnick       = $senderNick\n\tchatInCh   = $thisChannelId\n\tflags      = $data[4]\n\tchatIn     = '$chatIn'\n" ;
if ($DEBUG_ALL) { echo $DBG ; $DBG = "" ; }

  if (!$isFromAdmin) // chat messages
  {
if ($DEBUG     &&  !$thisBridgeName) $DBG .= "from unbridged channel - dropping message\n" ;
if ($DEBUG     &&   $thisBridgeName &&      !$thisBridge[$ENABLED_KEY]) $DBG .= "bridge disabled - dropping message\n" ;
if ($DEBUG     &&   $thisBridgeName && count($thisBridge[$CHANNELS_KEY]) < 2) $DBG .= "bridge '$thisBridgeName' has no receivers - dropping message\n" ;
if ($DEBUG_ALL && (!$thisBridgeName ||      !$thisBridge[$ENABLED_KEY])) { echo $DBG ; $DBG = "" ; }

    if (!$thisBridge[$ENABLED_KEY]) return ["" , ""] ;

    // parse and strip IRC commands
    foreach ($STAR_IRC_COMMANDS as $token) if (startsWith($chatIn , $token))
      { $chatIn = lstrip($chatIn , $token) ; $NickPrefix = $STAR_PREFIX ; break ; }
    foreach ($STRIP_IRC_COMMANDS as $token) if (startsWith($chatIn , $token))
      { $chatIn = lstrip($chatIn , $token) ; break ; }

    // set outgoing message and channels
    $chatOut = chatOut($senderNick , $chatIn) ;
    if ($thisBridge) $theseChannels = $thisBridge[$CHANNELS_KEY] ; else return ["" , ""] ;
  }
  else // admin control messages
  {
    if (!startsWith($chatIn , $TRIGGER_PREFIX)) return ["" , ""] ;

    // tokenize and switch on admin triggers
    $tokens = explode(" " , lstrip($chatIn , $TRIGGER_PREFIX) , 2) ;
    $trigger = $tokens[0] ; $chatIn = $thatBridgeName = ($tokens[1])? $tokens[1] : "" ;
    switch ($trigger)
    {
      case $ADD_TRIGGER:
      {
        if (!$thatBridgeName) $thatBridgeName = $DEFAULT_BRIDGE_NAME ;
        if (!$thisBridge)
        {
          if (!$Bridges[$thatBridgeName])
          {
            $Bridges[$thatBridgeName]                = [] ;
            $Bridges[$thatBridgeName][$NAME_KEY]     = $thatBridgeName ;
            $Bridges[$thatBridgeName][$ENABLED_KEY]  = true ;
            $Bridges[$thatBridgeName][$CHANNELS_KEY] = [] ;
          }
          $thatBridge                                = &$Bridges[$thatBridgeName] ;
          $thatBridge[$CHANNELS_KEY][$thisChannelId] = getChannelName($thisChannelId) ;
          $adminChatOut .= "$CH_SET_MSG '$thatBridgeName'" . getBridgeStats($thatBridgeName) ;
        }
        elseif ($thisBridge === $Bridges[$thatBridgeName])
          $adminChatOut .= "$THIS_CHANNEL_MSG$thisChannelId$CHANNEL_EXISTS_MSG '$thatBridgeName'" ;
        else $adminChatOut .= $BRIDGE_CONFLICT_MSG ;
      } break ;
      case $REMOVE_TRIGGER:
      {
        if ($thisBridge)
        {
          $thisBridgeName = $thisBridge[$NAME_KEY] ;
          unset($Bridges[$thisBridgeName][$CHANNELS_KEY][$thisChannelId]) ;
          $adminChatOut .= "$CHANNEL_REMOVED_MSG" . getBridgeStats($thisBridgeName) ;
          if (!count($Bridges[$thisBridgeName][$CHANNELS_KEY]))
            unset($Bridges[$thisBridgeName]) ;
        }
        else $adminChatOut .= getChannelState($thisChannelId) ;
      } break ;
      case $DISABLE_TRIGGER:
      case $ENABLE_TRIGGER:
      {
        $enableBridge = function($bridgeName , $isEnable)
        {
          global $Bridges , $ENABLED_KEY , $ENABLE_MSG , $ENABLED_MSG , $DISABLED_MSG ;
          if (!$Bridges[$bridgeName]) return getBridgeStats($bridgeName) ;
          else if ($Bridges[$bridgeName][$ENABLED_KEY] = $isEnable)
               return "$ENABLE_MSG '$bridgeName' $ENABLED_MSG" ;
          else return "$ENABLE_MSG '$bridgeName' $DISABLED_MSG" ;
        } ;

        $isEnable = ($trigger == $ENABLE_TRIGGER) ;
        if (!count($Bridges)) $adminChatOut .= getBridgeStats([]) ;
        else if (!$thatBridgeName) foreach ($Bridges as $bridgeName => $aBridge)
             $adminChatOut .= $enableBridge($bridgeName , $isEnable) ;
        else $adminChatOut .= $enableBridge($thatBridgeName , $isEnable) ;
      } break ;
      case $STATS_TRIGGER:
      {
        $adminChatOut .= getChannelState($thisChannelId) ;
        if (!count($Bridges)) $adminChatOut .= getBridgeStats([]) ;
        else if (!$thatBridgeName) foreach ($Bridges as $bridgeName => $aBridge)
             $adminChatOut .= getBridgeStats($bridgeName) ;
        else $adminChatOut .= getBridgeStats($thatBridgeName) ;
      } break ;
      case $ECHO_TRIGGER: { $adminChatOut = chatOut($senderNick , $chatIn) ; } break ;
      case $CHAT_TRIGGER:
      {
        if ($thisBridge)
        {
          $theseChannels = &$thisBridge[$CHANNELS_KEY] ;
          $chatOut = chatOut($senderNick , $chatIn) ; $adminChatOut = "" ;
        }
        else $adminChatOut .= getChannelState($thisChannelId) ;
      } break ;
      case $BROADCAST_TRIGGER:
      {
        foreach ($Bridges as $bridgeName => $aBridge)
          $theseChannels += $aBridge[$CHANNELS_KEY] ;
        $chatOut = ($adminChatOut .= $chatIn) ;
      } break ;
      case $EXIT_TRIGGER: { $Done = true ; $adminChatOut .= $EXIT_MSG ; } break ;
      default: { $adminChatOut .= "$UNKNOWN_MSG '$TRIGGER_PREFIX$trigger'" ; }
    }
if ($DEBUG) $DBG .= "parsed trigger '$TRIGGER_PREFIX$trigger'" . (($adminChatOut == chatOut($BRIDGE_NICK , "") . "$UNKNOWN_MSG '$TRIGGER_PREFIX$trigger'")? " (invalid)" : "") . "\n" ;

    // admin echo
    postMessage($thisChannelId , $adminChatOut) ;
  }

  // relay chat
  foreach ($theseChannels as $channelId => $channelName)
    if ($channelId != $thisChannelId) postMessage($channelId , $chatOut) ;

if ($DEBUGVB && count($theseChannels) > 1) $DBG .= "msgOut = '$chatOut'\nrelaying to " . (count($theseChannels) - 1) . " channels\n" ;
if ($DEBUG) echo $DBG ;

return ["" , ""] ;
