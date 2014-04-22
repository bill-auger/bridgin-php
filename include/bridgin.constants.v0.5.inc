<?php
$DEBUG         = false ;
$DEBUGVB       = $DEBUG && true ;
$DEBUG_ALL     = $DEBUG && true ;
$DEBUG_SESSION = $DEBUG && false ;
if ($DEBUG)
{
  $DEBUG_HELPERS_FILENAME = './DEBUG/bridgin.dbg.helpers.inc' ;
  $DEBUG_SESSION_FILENAME = './DEBUG/bridgin.dbg.session.inc' ;
  $DEBUG_IN_FILENAME      = './DEBUG/bridgin.dbg.in.inc' ;
  $DEBUG_OUT_FILENAME     = './DEBUG/bridgin.dbg.out.inc' ;
}
// TODO:
//  bridging im's is implemented but disabled
//      until postMessage() is able to distinguish between im and chat channels
//  apply some sane upper bound on # of channels
//  tests

/*\
|*| CONFIGURATION:
|*|   *IMPORTANT* -
|*|   add to $STAR_IRC_COMMANDS any IRC commands that should append $STAR_PREFIX to nick
|*|   add to $STRIP_IRC_COMMANDS any IRC commands that you would like simply stripped
|*|   if you should change either of $NICK_PREFIX or $STAR_PREFIX - be certain that
|*|   neither of them begins with $TRIGGER_PREFIX in order to prevent infinte loopback
\*/
$STAR_IRC_COMMANDS   = ['/me ' , '\01/me'] ;
$STRIP_IRC_COMMANDS  = ['?/' , '/'] ;
$NICK_PREFIX         = "(from " ; // dont use '<' some clients will supress it as html
$STAR_PREFIX         = "(* " ;
$NICK_POSTFIX        = ') ' ;
$BRIDGE_NICK         = 'BRIDGE' ;
$DEFAULT_BRIDGE_NAME = 'default' ;
$DBUS_POLL_IVL       = 1000 ;

$TRIGGER_PREFIX    = '?/' ;
$ADD_TRIGGER       = 'add' ;
$REMOVE_TRIGGER    = 'rem' ;
$DISABLE_TRIGGER   = 'disable' ;
$ENABLE_TRIGGER    = 'enable' ;
$STATUS_TRIGGER    = 'status' ;
$ECHO_TRIGGER      = 'echo' ;
$CHAT_TRIGGER      = 'chat' ;
$BROADCAST_TRIGGER = 'broadcast' ;
$AUTH_TRIGGER      = 'auth' ; // unused
$HELP_TRIGGER      = 'help' ;
$EXIT_TRIGGER      = 'exit' ;

// init messages
$APP_NAME           = 'bridgin' ;
$INIT_CONFIG_MSG    = "\n\ninitialized empty session\n" ;
$LOAD_CONFIG_MSG    = "\n\nsession restored\n" ;
$LOAD_FAIL_MSG      = "\n\nerror restoring session\n" ;
$INVALID_CONFIG_MSG = "invalid configuration - quitting\n" ;
$DBUS_OK_MSG        = "created dbus proxy\n" ;
$DBUS_FATAL_MSG     = "dbus error - quitting\n" ;
$PURPLE_OK_MSG      = "found purple instance\n" ;
$PURPLE_ERROR_MSG   = "no purple instance found - is pidgin running?\n" ;
$LIST_ACCOUNTS_MSGa = "found" ;
$LIST_ACCOUNTS_MSGb = "pidgin accounts:" ;
$LIST_BRIDGES_MSG   = " bridges configured:\n" ;
$LIST_ACCOUNTS_MSGa = "found" ;
$READY_MSGa         = "ready" ;
$READY_MSGb         = "\nsend the chat message: $TRIGGER_PREFIX$ADD_TRIGGER" .
                      "\ninto all chats you would like bridged" .
                      "\nrefer to the file 'README.md' for more options" ;

// main messages
$DBUS_ERROR_MSG      = "connection error\n" ;
$BOGUS_DATA_MSG      = "bogus data received\n" ;
$UNKNOWN_CHANNEL_MSG = "unknown channel: " ;
$CH_SET_MSG          = "channel set to bridge" ;
$BRIDGE_CONFLICT_MSG = "each channel may only be on one bridge" ;
$THIS_CHANNEL_MSG    = "this channel" ;
$CHANNEL_EXISTS_MSG  = "already exists on bridge" ;
$THIS_BRIDGE_MSG     = "is on bridge" ;
$UNBRIDGED_MSG       = "is not bridged" ;
$CHANNEL_REMOVED_MSG = "channel removed from bridge" ;
$BRIDGE_REMOVED_MSG  = "\nbridge removed" ;
$ENABLING_ALL_MSG    = "enabling all bridges" ;
$DISABLING_ALL_MSG   = "disabling all bridges" ;
$ENABLE_MSG          = "bridge" ;
$ENABLED_MSG         = "is enabled" ;
$DISABLED_MSG        = "is disabled" ;
$BROADCAST_MSGa      = "broadcast message relayed to " ;
$BROADCAST_MSGb      = " channels" ;
$NO_SUCH_BRIDGE_MSG  = "no such bridge" ;
$NO_BRIDGES_MSG      = "no bridges exist" ;
$STATS_MSGa          = "bridge" ;
$STATS_DELETED_MSG   = "- deleted" ;
$STATS_ENABLED_MSG   = "- enabled -" ;
$STATS_DISABLED_MSG  = "- disabled -" ;
$STATS_MSGb          = "channels bridged" ;
$EXIT_MSG            = "shutting down" ;
$UNKNOWN_MSG         = "unknown command" ;
$HELP_MSG            = "the following admin commands are currently supported:" ;
$HELP_MSGS           =
[
  "$TRIGGER_PREFIX$ADD_TRIGGER"                     => "add this channel to the default bridge" ,
  "$TRIGGER_PREFIX$ADD_TRIGGER 'BRIDGE_NAME'"       => "add this channel to the bridge 'BRIDGE_NAME'" ,
  "$TRIGGER_PREFIX$REMOVE_TRIGGER"                  => "remove this channel from the default bridge" ,
  "$TRIGGER_PREFIX$REMOVE_TRIGGER 'BRIDGE_NAME'"    => "remove this channel from the bridge 'BRIDGE_NAME'" ,
  "$TRIGGER_PREFIX$DISABLE_TRIGGER"                 => "temporarily disable all bridges" ,
  "$TRIGGER_PREFIX$DISABLE_TRIGGER 'BRIDGE_NAME'"   => "temporarily disable the bridge 'BRIDGE_NAME'" ,
  "$TRIGGER_PREFIX$ENABLE_TRIGGER"                  => "enable all bridges" ,
  "$TRIGGER_PREFIX$ENABLE_TRIGGER 'BRIDGE_NAME'"    => "enable the bridge 'BRIDGE_NAME'" ,
  "$TRIGGER_PREFIX$ECHO_TRIGGER 'SOME_TEXT'"        => "echo text to the same channel" ,
  "$TRIGGER_PREFIX$CHAT_TRIGGER 'SOME_TEXT'"        => "relay text to the all channels on this bridge" ,
  "$TRIGGER_PREFIX$BROADCAST_TRIGGER 'SOME_TEXT'"   => "relay text to the all channels on all bridges as $BRIDGE_NICK" ,
//  "$TRIGGER_PREFIX$AUTH_TRIGGER"                    => "authorize remote admins" ,
  "$TRIGGER_PREFIX$STATUS_TRIGGER"                  => "show status information for all bridges" ,
  "$TRIGGER_PREFIX$STATUS_TRIGGER 'BRIDGE_NAME'"    => "show status information for the bridge 'BRIDGE_NAME'" ,
  "$TRIGGER_PREFIX$HELP_TRIGGER or $TRIGGER_PREFIX" => "show avaiable admin commands" ,
  "$TRIGGER_PREFIX$EXIT_TRIGGER"                    => "kill the bridgin process"
] ;

// dictionary keys
$BRIDGE_NAME_KEY       = 'bridgeName' ;
$ENABLED_KEY           = 'isEnabled' ;
$CHANNELS_KEY          = 'channels' ;
$CHANNEL_KEY_SEPERATOR = '::' ;
$PROTOCOL_KEY          = 'channel-protocol' ;
$ACCOUNT_ID_KEY        = 'channel-account' ;
$CHANNEL_ID_KEY        = 'channel-id' ;
$CHANNEL_NAME_KEY      = 'channel-name' ;
$USERNAME_KEY          = 'channel-username' ;

// pidgin dbus constants
$PURPLE_INTERFACE      = 'im.pidgin.purple.PurpleInterface' ;
$PURPLE_SERVICE        = 'im.pidgin.purple.PurpleService' ;
$PURPLE_OBJECT         = '/im/pidgin/purple/PurpleObject' ;
$CHANNEL_CLOSED_SIGNAL = 'DeletingConversation' ;
$IM_METHOD             = 'ReceivedImMsg' ;
$CHAT_METHOD           = 'ReceivedChatMsg' ;
$OUTGOING_MESSAGE      = 'PURPLE_MESSAGE_SEND' ;
$MESSGAE_FLAGS         =
[
  hexdec('0x0001') => $OUTGOING_MESSAGE ,
  hexdec('0x0002') => 'PURPLE_MESSAGE_RECV' ,
  hexdec('0x0004') => 'PURPLE_MESSAGE_SYSTEM' ,
  hexdec('0x0008') => 'PURPLE_MESSAGE_AUTO_RESP' ,
  hexdec('0x0010') => 'PURPLE_MESSAGE_ACTIVE_ONLY' ,
  hexdec('0x0020') => 'PURPLE_MESSAGE_NICK' ,
  hexdec('0x0040') => 'PURPLE_MESSAGE_NO_LOG' ,
  hexdec('0x0080') => 'PURPLE_MESSAGE_WHISPER' ,
  hexdec('0x0200') => 'PURPLE_MESSAGE_ERROR' ,
  hexdec('0x0400') => 'PURPLE_MESSAGE_DELAYED' ,
  hexdec('0x0800') => 'PURPLE_MESSAGE_RAW' ,
  hexdec('0x1000') => 'PURPLE_MESSAGE_IMAGES' ,
  hexdec('0x2000') => 'PURPLE_MESSAGE_NOTIFY' ,
  hexdec('0x4000') => 'PURPLE_MESSAGE_NO_LINKIFY' ,
  hexdec('0x8000') => 'PURPLE_MESSAGE_INVISIBLE'
] ;
$PURPLE_CONV_TYPE_ANY = 4 ;


/* schema */

$BRIDGES_FILENAME = 'session.dump' ; // refer to validateConfig() function for types

/*
$Bridges =
{
  'bridgeName' :
  {
    $BRIDGE_NAME_KEY : 'bridgeName' ,
    $ENABLED_KEY     : aBool ,
    $CHANNELS_KEY    :
    {
      'channelKey' :
      {
        $PROTOCOL_KEY     : 'protocolName'
        $ACCOUNT_ID_KEY   : accountId ,
        $CHANNEL_ID_KEY   : channelId ,
        $CHANNEL_NAME_KEY : 'channelName' ,
        $USERNAME_KEY     : 'username'
      } ,
      ....
    }
  } ,
  ....
}
*/
