<?php
$DEBUG = true ;

// add all relevant nicks/aliases to $ADMIN_NICKS to avoid infinite loopback
$ADMIN_NICKS     = ['IRC' , 'SKYPE' , 'YOUR_IRC_NICK' , 'YOUR_SKYPE_NICK'] ;


$ADMIN_TRIGGER   = '?/' ;
$SERVER_NICK     = 'BRIDGE' ;
$SAID_TOKENa     = "[" ; // dont use '<' it will look like html to some clients
$SAID_TOKENb     = ' said] : ' ;
$MYIP_URL        = 'http://ifconfig.me/ip' ;
$SET_CH1_TRIGGER = 'setch1' ;
$SET_CH2_TRIGGER = 'setch2' ;
$ECHO_TRIGGER    = 'echo' ;
$GET_IP_TRIGGER  = 'myip' ;
$SLEEP_TRIGGER   = 'sleep' ;
$KILL_TRIGGER    = 'kill' ;
$CH1_SET_MSG     = "channel 1 set - channel 2 " ;
$CH2_SET_MSG     = "channel 2 set - channel 1 " ;
$LOOPBACK_MSG    = " - (loopback mode)" ;
$SLEEPING_MSG    = "sleeping" ;
$KILLED_MSG      = "shutting down" ;
$UNKNOWN_MSG     = "unknown trigger " ;

$INTERFACE   = "im.pidgin.purple.PurpleInterface" ;
$CHAT_METHOD = "ReceivedChatMsg" ;
$NULL_ID     = -999 ;


$Ch1Id = $Ch1Ch = $Ch2Id = $Ch2Ch = $NULL_ID ;


$Dbus = new Dbus(Dbus::BUS_SESSION);
$Dbus->addWatch($INTERFACE, $CHAT_METHOD);
$Proxy = $Dbus->createProxy("im.pidgin.purple.PurpleService",
                            "/im/pidgin/purple/PurpleObject",
                            "im.pidgin.purple.PurpleInterface");

$ShouldKill = false ;
do
{
  $signal = $Dbus->waitLoop(1000) ;
  if (!$signal instanceof DbusSignal) continue ;
  if (!$signal->matches($INTERFACE , $CHAT_METHOD)) continue ;

  $data = $signal->getData()->getData() ;
  $serviceId = $data[0] ;
  $sender = $data[1] ;
  $msgIn = $data[2] ;
  $chatInCh = $data[3] ;

  if (!$Proxy->PurpleAccountIsConnected($serviceId)) continue ;

  $chatOutId = $chatInCh ; $msgOut = $SAID_TOKENa . $SERVER_NICK . $SAID_TOKENb ;
  $isFromAdmin = in_array($sender , $ADMIN_NICKS) ;
  $isAdminTrigger = (strpos($msgIn , $ADMIN_TRIGGER) === 0) ;

if ($DEBUG) {
echo "\n" ;
if ($signal->matches($INTERFACE , $CHAT_METHOD)) echo "$CHAT_METHOD" ;
if ($serviceId == $Ch1Id && $chatInCh == $Ch1Ch) echo " from irc" ;
if ($serviceId == $Ch2Id && $chatInCh == $Ch2Ch) echo " from skype" ;
echo "\nreceived msg " . ((!$isFromAdmin)? "from user" :
      "from admin" . ((!$isAdminTrigger)? " - not trigger" : " - got trigger")) ;
echo "\ndata[]=\n" ;
echo "serviceId=$serviceId\n" ;
echo "sender=$sender\n" ;
echo "chatInCh=$chatInCh\n" ;
echo "flags=$data[4]\n" ;
echo "msgIn='$msgIn'\n" ; }

  if (!$isFromAdmin || !$isAdminTrigger)
  {
if ($DEBUG && $isFromAdmin && strpos($msgIn , $SAID_TOKENb)) echo "echo - dropping message\n" ;
if ($DEBUG && $Ch2Ch == $NULL_ID) echo "SkypeCh not set - dropping message\n" ;
if ($DEBUG && $Ch1Ch == $NULL_ID) echo "IrcCh not set - dropping message\n" ;

    // filter server and relayed messages
    if ($isFromAdmin && strpos($msgIn , $SAID_TOKENb) !== false) continue ;

    // set outgoing message and channel
    $msgOut = $SAID_TOKENa . $sender . $SAID_TOKENb . $msgIn ;
    $wasFromIrcChannel = ($serviceId == $Ch1Id && $chatInCh == $Ch1Ch) ;
    $wasFromSkypeChannel = ($serviceId == $Ch2Id && $chatInCh == $Ch2Ch) ;
    if ($wasFromIrcChannel && $Ch2Id != $NULL_ID) $chatOutId = $Ch2Ch ;
    elseif ($wasFromSkypeChannel && $Ch1Id != $NULL_ID) $chatOutId = $Ch1Ch ;
    else continue ;
  }
  else
  {
    // parse and switch on admin triggers
    $tokens = explode(" " , substr($msgIn , strlen($ADMIN_TRIGGER)) , 2)  ;
    $trigger = $tokens[0] ; if (isset($tokens[1])) $msgIn = $tokens[1] ;
    switch ($trigger)
    {
      // admin control triggers
      case $SET_CH1_TRIGGER:
      {
        $Ch1Id = $serviceId ; $Ch1Ch = $chatInCh ;
        $msgOut .= $CH1_SET_MSG . (($Ch2Ch == $NULL_ID)? "not" : "") . " set" ;
        if ($Ch1Id == $Ch2Id && $Ch1Ch == $Ch2Ch) $msgOut .= $LOOPBACK_MSG ;
      } break ;
      case $SET_CH2_TRIGGER:
      {
        $Ch2Id = $serviceId ; $Ch2Ch = $chatInCh ;
        $msgOut .= $CH2_SET_MSG . (($Ch1Ch == $NULL_ID)? "not" : "") . " set" ;
        if ($Ch1Id == $Ch2Id && $Ch1Ch == $Ch2Ch) $msgOut .= $LOOPBACK_MSG ;
      } break ;
      case $ECHO_TRIGGER: { $msgOut .= "$sender said=\: '$msgIn'" ; } break ;
      case $GET_IP_TRIGGER: { $msgOut .= file_get_contents(MYIP_URL) ; } break ;
      case $SLEEP_TRIGGER:
      {
        $Ch2Id = $Ch2Ch = $Ch1Id = $Ch1Ch = $NULL_ID ;
        $msgOut .= $SLEEPING_MSG ;
      } break ;
      case $KILL_TRIGGER: { $ShouldKill = true ; $msgOut .= $KILLED_MSG ; } break ;
      default: { $msgOut .= $UNKNOWN_MSG . $msgIn ; }
    }
  }

  // relay chat
  $Proxy->PurpleConvChatSend($Proxy->PurpleConvChat($chatOutId) , $msgOut) ;

if ($DEBUG) echo "msgOut='$msgOut'\n" ;

} while (!$ShouldKill) ;

?>
