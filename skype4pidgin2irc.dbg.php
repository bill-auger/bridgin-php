<?php
$DEBUG = true ; $DEBUGVB = $DEBUG && true ;
// NEW: to test multiple channels for each of 2 bridges (add del sleep wake chat bcast)
// TODO:
//    apply some sane upper bound on # of channels
//    using $ALLOW_ADMIN_CHAT is insecure as it merely matches nicks
//      options include matching against the accountId also
//      or there is possibly there is something more in the signal objects or API
//      or remove the feature for production version
//    refactor chatOut formatting (4x nearly verbatim)
//    refactor the two hard-coded bridges into an dictionary with named keys
//      for add rem start stop status - (include isEnabled - lose Idle1Chs)
//      will allow interfacing such as:
//        ?/add 'BRIDGE_NAME'
//          - add this channel to the bridge 'BRIDGE_NAME'
//        ?/status 'BRIDGE_NAME'
//          - shows status information for bridge 'BRIDGE_NAME'
//    refactor all (array_key_exists($chatInCh , $Bridge1Chs)) into getBridge($chatInCh)
//        that returns a bridge (channels are restricted to one bridge)
//    refactor all (strpos($chatIn , $cmd) === 0) into startsWith($chatIn , $cmd)
//    refactor all (strpos($chatIn , $NICK_POSTFIX) !== false) into contains($chatIn , $cmd)
//    refactor all substr($chatIn , strlen($cmd) into lstrip($chatIn , $cmd)
//    refactor all $Proxy->PurpleConvChatSend($Proxy->PurpleConvChat($chatInCh) , $chatOut)
//        into postMessage($chatInCh , $chatOut)
//    refactor all $THIS_BRIDGE_MSG clauses (3x verbatim)


$APP_NAME = 'skype4pidgin2irc' ; $VERSION = 0.2 ;
/*\
|*| CONFIGURATION:
|*| when $ALLOW_ADMIN_CHAT is true
|*|   add ALL relevant nicks to $ADMIN_NICKS to avoid infinite loopback
|*|   and ensure that $NICK_POSTFIX is distinct from typical chat
|*| else just add those nicks that require admin access
|*| when $ALLOW_ADMIN_CHAT is false $ADMIN_NICKS may chat on individual channels as usual
|*|   but their messages will not propogate to other channels unless properly prefixed
\*/
$ALLOW_ADMIN_CHAT = false ;
$ADMIN_NICKS      = [] ;
if ($ALLOW_ADMIN_CHAT) $ADMIN_NICKS += ['IRC' , 'SKYPE'] ;
$IRC_COMMANDS = ['/me ' , '\01/me'] ;

$NICK_PREFIX   = "(" ; // dont use '<' it will look like html to some clients
$NICK_POSTFIX  = ($ALLOW_ADMIN_CHAT)? ' said): ' : ') ' ;
$BRIDGE_NICK   = 'BRIDGE' ;

$ADMIN_TRIGGER      = '?/' ;
$ADD_CH1_TRIGGER    = 'add1' ;
$ADD_CH2_TRIGGER    = 'add2' ;
$REMOVE_CH1_TRIGGER = 'rem1' ;
$REMOVE_CH2_TRIGGER = 'rem2' ;
$DISABLE1_TRIGGER   = 'disable1' ;
$DISABLE2_TRIGGER   = 'disable2' ;
$ENABLE1_TRIGGER    = 'enable1' ;
$ENABLE2_TRIGGER    = 'enable2' ;
$STATS_TRIGGER      = "status" ;
$ECHO_TRIGGER       = 'echo' ;
$CHAT_TRIGGER       = 'chat' ;
$BROADCAST_TRIGGER  = 'broadcast' ;
$EXIT_TRIGGER       = 'shutdown' ;

$USAGE_MSGa          = "to begin send the chat message: " ;
$USAGE_MSGb          = "\ninto all chats you would like to bridge" .
                       "\nrefer to the file 'README' for more commands" ;
$CH_SET_MSGa         = "bridge " ;
$CH_SET_MSGb         = " channel set - " ;
$STATS_MSG           = " channels bridged" ;
$BRIDGE_CONFLICT_MSG = "each channel may only be on one bridge" ;
$CHANNEL_EXISTS_MSG  = "this channel already exists on this bridge" ;
$WRONG_BRIDGE_MSG    = "this channel is not on bridge" ;
$THIS_BRIDGE_MSG     = "this channel is on bridge " ;
$NO_SUCH_CHANNEL_MSG = "this channel is not bridged" ;
$CHANNEL_REMOVED_MSG = "channel removed from bridge" ;
$IDLE_MSGa           = "bridge " ;
$IDLE_MSGb           = " is idle" ;
$WAKE_MSGa           = "bridge " ;
$WAKE_MSGb           = " is active" ;
$EXIT_MSG            = "shutting down" ;
$UNKNOWN_MSG         = "unknown trigger " ;

$INTERFACE        = "im.pidgin.purple.PurpleInterface" ;
$CHAT_METHOD      = "ReceivedChatMsg" ;
$PURPLE_SERVICE   = "im.pidgin.purple.PurpleService" ;
$PURPLE_OBJECT    = "/im/pidgin/purple/PurpleObject" ;
$PURPLE_INTERFACE = "im.pidgin.purple.PurpleInterface" ;


// initialize messaging and register event dependencies
$Dbus = new Dbus(Dbus::BUS_SESSION) ;
$Dbus->addWatch($INTERFACE, $CHAT_METHOD) ;
$Proxy = $Dbus->createProxy($PURPLE_SERVICE , $PURPLE_OBJECT , $PURPLE_INTERFACE) ;

// initialize state
$Bridge1Chs = [] ; $Idle1Chs = [] ;
$Bridge2Chs = [] ; $Idle2Chs = [] ;
$done = false ;

// lets rock
echo "\n$APP_NAME v$VERSION\n\n$USAGE_MSGa$ADMIN_TRIGGER$ADD_CH1_TRIGGER$USAGE_MSGb\n\n" ;
while (!$done)
{
  // trap signals
  $signal = $Dbus->waitLoop(1000) ;
  if (!$signal instanceof DbusSignal) continue ;
  if (!$signal->matches($INTERFACE , $CHAT_METHOD)) continue ;

  // parse data
  $data = $signal->getData()->getData() ;
  $accountId = $data[0] ; $senderNick = $data[1] ;
  $chatIn    = $data[2] ; $chatInCh   = $data[3] ;
  if (!$Proxy->PurpleAccountIsConnected($accountId)) continue ;

  // set loop invariants
  $chatOut = $adminChatOut = $NICK_PREFIX . $BRIDGE_NICK . $NICK_POSTFIX ;
  $chatOutChs = [] ;

  $isFromAdmin = in_array($senderNick , $ADMIN_NICKS) ;
  $isAdminTrigger = (strpos($chatIn , $ADMIN_TRIGGER) === 0) ;

if ($DEBUG) echo "\n$CHAT_METHOD " ;
if ($DEBUG) echo (array_key_exists($chatInCh , $Bridge1Chs)? "from bridge 1" : ((array_key_exists($chatInCh , $Bridge2Chs))? "from bridge 2" : "(unbridged)")) ;
if ($DEBUG) echo " " . ((!$isFromAdmin)? "from user $senderNick" : "from admin $senderNick" . ((!$isAdminTrigger)? " - not trigger" . (($ALLOW_ADMIN_CHAT)? "" : " (supressing)") : " - got trigger")) ; if ($DEBUG) echo "\n" ;
if ($DEBUGVB) echo "\taccountId = $accountId\n\tsenderNick = $senderNick\n\tchatInCh  = $chatInCh\n\tflags     = $data[4]\n\tchatIn    = '$chatIn'\n" ;

  if (!$isFromAdmin || !$isAdminTrigger) // chat messages
  {
    // filter bridge messages
    $isRelayedChat = (strpos($chatIn , $NICK_POSTFIX) !== false) ;

if ($DEBUG && $isFromAdmin && ($isRelayedChat || !$ALLOW_ADMIN_CHAT)) echo (($isRelayedChat)? "relayed" :  "admin") . " chat - dropping message\n" ;
if ($DEBUG && !array_key_exists($chatInCh , $Bridge1Chs) && !array_key_exists($chatInCh , $Bridge2Chs)) echo "from unbridged channel - dropping message\n" ;
if ($DEBUG &&  array_key_exists($chatInCh , $Bridge1Chs) && count($Bridge1Chs) < 2) echo "channel 1 has no receivers - dropping message\n" ;
if ($DEBUG &&  array_key_exists($chatInCh , $Bridge2Chs) && count($Bridge2Chs) < 2) echo "channel 2 has no receivers - dropping message\n" ;

    if ($isFromAdmin && ($isRelayedChat || !$ALLOW_ADMIN_CHAT)) continue ;

    // parse and strip IRC commands
    $ircCmd = '' ;
    foreach ($IRC_COMMANDS as $cmd) if (strpos($chatIn , $cmd) === 0)
      { $chatIn = substr($chatIn , strlen($cmd)) ; $ircCmd = $cmd ; break ; }

    // set outgoing message and channel
    if ($ircCmd) $senderNick = "* $senderNick" ;
    $chatOut = $NICK_PREFIX  . $senderNick . $NICK_POSTFIX . $chatIn ;
    if ((array_key_exists($chatInCh , $Bridge1Chs))) $chatOutChs = $Bridge1Chs ;
    elseif ((array_key_exists($chatInCh , $Bridge2Chs))) $chatOutChs = $Bridge2Chs ;
    else continue ;
  }
  else // admin control messages
  {
    // tokenize and switch on admin triggers
    $tokens = explode(" " , substr($chatIn , strlen($ADMIN_TRIGGER)) , 2) ;
    $trigger = $tokens[0] ; if (isset($tokens[1])) $chatIn = $tokens[1] ;
    switch ($trigger)
    {
      case $ADD_CH1_TRIGGER:
      {
        if (array_key_exists($chatInCh , $Bridge2Chs)) $adminChatOut .= $BRIDGE_CONFLICT_MSG ;
        else if (array_key_exists($chatInCh , $Bridge1Chs)) $adminChatOut .= $CHANNEL_EXISTS_MSG ;
        else
        {
          $Bridge1Chs[$chatInCh] = true ;
          $adminChatOut .= "$CH_SET_MSGa'1'$CH_SET_MSGb" . count($Bridge1Chs) . $STATS_MSG ;
        }
      } break ;
      case $ADD_CH2_TRIGGER:
      {
        if (array_key_exists($chatInCh , $Bridge1Chs)) $adminChatOut .= $BRIDGE_CONFLICT_MSG ;
        else if (array_key_exists($chatInCh , $Bridge2Chs)) $adminChatOut .= $CHANNEL_EXISTS_MSG ;
        else
        {
          $Bridge2Chs[$chatInCh] = true ;
          $adminChatOut .= "$CH_SET_MSGa'2'$CH_SET_MSGb" . count($Bridge1Chs) . $STATS_MSG ;
        }
      } break ;
      case $REMOVE_CH1_TRIGGER:
      {
        if (!array_key_exists($chatInCh , $Bridge1Chs))
        {
          $adminChatOut .= "$WRONG_BRIDGE_MSG '1'\n" ;
          $bridge1Chs = $Bridge1Chs + $Idle1Chs ; $bridge2Chs = $Bridge2Chs + $Idle2Chs ;
          $adminChatOut .= ((array_key_exists($chatInCh , $bridge1Chs))?
            "$THIS_BRIDGE_MSG'1'" : ((array_key_exists($chatInCh , $bridge2Chs))?
            "$THIS_BRIDGE_MSG'2'" : $NO_SUCH_CHANNEL_MSG)) ;
        }
        else
        {
          unset($Bridge1Chs[$chatInCh]) ;
          $adminChatOut .= "$CHANNEL_REMOVED_MSG\n" . count($Bridge1Chs) . $STATS_MSG ;
        }
      } break ;
      case $REMOVE_CH2_TRIGGER:
      {
        if (!array_key_exists($chatInCh , $Bridge2Chs))
        {
          $adminChatOut .= "$WRONG_BRIDGE_MSG '2'\n" ;
          $bridge1Chs = $Bridge1Chs + $Idle1Chs ; $bridge2Chs = $Bridge2Chs + $Idle2Chs ;
          $adminChatOut .= ((array_key_exists($chatInCh , $bridge1Chs))?
            "$THIS_BRIDGE_MSG'1'" : ((array_key_exists($chatInCh , $bridge2Chs))?
            "$THIS_BRIDGE_MSG'2'" : $NO_SUCH_CHANNEL_MSG)) ;
        }
        else
        {
          unset($Bridge2Chs[$chatInCh]) ;
          $adminChatOut .= "$CHANNEL_REMOVED_MSG\n" . count($Bridge2Chs) . $STATS_MSG ;
        }
      } break ;
      case $DISABLE1_TRIGGER:
      {
        $Idle1Chs = $Bridge1Chs ; $Bridge1Chs = [] ;
        $adminChatOut .= "$IDLE_MSGa'1'$IDLE_MSGb" ;
      } break ;
      case $DISABLE2_TRIGGER:
      {
        $Idle2Chs = $Bridge2Chs ; $Bridge2Chs = [] ;
        $adminChatOut .= "$IDLE_MSGa'2'$IDLE_MSGb" ;
      } break ;
      case $ENABLE1_TRIGGER:
      {
        $Bridge1Chs = $Idle1Chs ; $Idle1Chs = [] ;
        $adminChatOut .= "$WAKE_MSGa'1'$WAKE_MSGb" ;
      } break ;
      case $ENABLE2_TRIGGER:
      {
        $Bridge2Chs = $Idle2Chs ; $Idle2Chs = [] ;
        $adminChatOut .= "$WAKE_MSGa'2'$WAKE_MSGb" ;
      } break ;
      case $STATS_TRIGGER:
      {
        $bridge1IsEnabled = !!count($Bridge1Chs) ; $bridge2IsEnabled = !!count($Bridge2Chs) ;
        $bridge1Chs = $Bridge1Chs + $Idle1Chs ; $bridge2Chs = $Bridge2Chs + $Idle2Chs ;
        $adminChatOut .= ((array_key_exists($chatInCh , $bridge1Chs))?
            "$THIS_BRIDGE_MSG'1'" : ((array_key_exists($chatInCh , $bridge2Chs))?
            "$THIS_BRIDGE_MSG'2'" : $NO_SUCH_CHANNEL_MSG)) ;
        //foreach ($bridges as $name => $bridge) // TODO:
        $adminChatOut .= "\nbridge '1' " . (($bridge1IsEnabled)? "enabled" : "disabled") . " - " . count($bridge1Chs) . $STATS_MSG ;
        $adminChatOut .= "\nbridge '2' " . (($bridge2IsEnabled)? "enabled" : "disabled") . " - " . count($bridge2Chs) . $STATS_MSG ;
      } break ;
      case $ECHO_TRIGGER:
      {
        $adminChatOut = $NICK_PREFIX . $senderNick . $NICK_POSTFIX . $chatIn ;
      } break ;
      case $CHAT_TRIGGER:
      {
        if (array_key_exists($chatInCh , $Bridge1Chs)) $chatOutChs = $Bridge1Chs ;
        if (array_key_exists($chatInCh , $Bridge2Chs)) $chatOutChs = $Bridge2Chs ;
        $chatOut = $adminChatOut = $NICK_PREFIX . $senderNick . $NICK_POSTFIX . $chatIn ;
      } break ;
      case $BROADCAST_TRIGGER:
      {
        $chatOutChs = $Bridge1Chs + $Bridge2Chs ;
        $chatOut = ($adminChatOut .= $chatIn) ;
      } break ;
      case $EXIT_TRIGGER: { $done = true ; $adminChatOut .= $EXIT_MSG ; } break ;
      default: { $adminChatOut .= $UNKNOWN_MSG . $chatIn ; }
    }
if ($DEBUG) echo "parsed trigger '$trigger'" . (($adminChatOut == $UNKNOWN_MSG . $chatIn)? " (invalid)" : "") . "\n";

    // admin echo
    $Proxy->PurpleConvChatSend($Proxy->PurpleConvChat($chatInCh) , $adminChatOut) ;
  }

  // relay chat
  foreach ($chatOutChs as $ch => $unsused) if ($ch != $chatInCh)
    $Proxy->PurpleConvChatSend($Proxy->PurpleConvChat($ch) , $chatOut) ;

if ($DEBUGVB && count($chatOutChs) > 1) echo "msgOut = '$chatOut'\nrelaying to " . count($chatOutChs) . " channels\n" ;
}

?>
