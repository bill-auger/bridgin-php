<?php
$DEBUG = true ; $DEBUGVB = $DEBUG && true ; $DEBUG_ALL = $DEBUG && true ;
if ($DEBUG)
{
  $DEBUG_HELPERS_FILENAME = './DEBUG/skype4pidgin2irc.dbg.helpers.inc' ;
  $DEBUG_IN_FILENAME      = './DEBUG/skype4pidgin2irc.dbg.in.inc' ;
  $DEBUG_USER_FILENAME    = './DEBUG/skype4pidgin2irc.dbg.user.inc' ;
  $DEBUG_ADMIN_FILENAME   = './DEBUG/skype4pidgin2irc.dbg.admin.inc' ;
  $DEBUG_OUT_FILENAME     = './DEBUG/skype4pidgin2irc.dbg.out.inc' ;
}
// TODO:
//  apply some sane upper bound on # of channels
//  implement load and store config commands based on accountId amd room name?
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
$STATS_TRIGGER     = 'status' ;
$ECHO_TRIGGER      = 'echo' ;
$CHAT_TRIGGER      = 'chat' ;
$BROADCAST_TRIGGER = 'broadcast' ;
$AUTH_TRIGGER      = 'auth' ;
$EXIT_TRIGGER      = 'exit' ;

// init messages
$CONFIG_ERROR_MSG   = "configuration error - quitting\n" ;
$DBUS_OK_MSG        = "created dbus proxy" ;
$DBUS_FATAL_MSG     = "dbus error - quitting" ;
$PURPLE_OK_MSG      = "found purple instance" ;
$PURPLE_ERROR_MSG   = "no purple instance found - is pidgin running?" ;
$LIST_ACCOUNTS_MSGa = "found" ;
$LIST_ACCOUNTS_MSGb = "pidgin accounts:" ;
$DBUS_ERROR_MSG     = "connection error" ;
$READY_MSGa         = "ready to bridge channels\nsend the chat message:" ;
$READY_MSGb         = "\ninto all chats you would like bridged" .
                      "\nrefer to the file 'README' for more options" ;

// main messages
$BOGUS_DATA_MSG      = "bogus data received" ;
$CH_SET_MSG          = "channel set to bridge" ;
$BRIDGE_CONFLICT_MSG = "each channel may only be on one bridge" ;
$THIS_CHANNEL_MSG    = "this channel" ;
$CHANNEL_EXISTS_MSG  = "already exists on bridge" ;
$THIS_BRIDGE_MSG     = "is on bridge" ;
$UNBRIDGED_MSG       = "is not bridged" ;
$CHANNEL_REMOVED_MSG = "channel removed from bridge" ;
$ENABLE_MSG          = "bridge" ;
$ENABLED_MSG         = "is enabled" ;
$DISABLED_MSG        = "is disabled" ;
$NO_SUCH_BRIDGE_MSG  = "no such bridge" ;
$NO_BRIDGES_MSG      = "no bridges exist" ;
$STATS_MSGa          = "bridge" ;
$STATS_DELETED_MSG   = "- deleted" ;
$STATS_ENABLED_MSG   = "- enabled -" ;
$STATS_DISABLED_MSG  = "- disabled -" ;
$STATS_MSGb          = "channels bridged" ;
$BROADCAST_MSGa      = "broadcast message relayed to " ;
$BROADCAST_MSGb      = " channels" ;
$EXIT_MSG            = "shutting down" ;
$UNKNOWN_MSG         = "unknown command" ;

// dictionary keys
$BRIDGE_NAME_KEY = 'bridgeName' ;
$CHANNELS_KEY    = 'channels' ;
$ENABLED_KEY     = 'isEnabled' ;

// pidgin dbus objects
$PURPLE_INTERFACE = 'im.pidgin.purple.PurpleInterface' ;
$PURPLE_SERVICE   = 'im.pidgin.purple.PurpleService' ;
$PURPLE_OBJECT    = '/im/pidgin/purple/PurpleObject' ;
$IM_METHOD        = 'ReceivedImMsg' ;
$CHAT_METHOD      = 'ReceivedChatMsg' ;


/* schema */

/*
$Admins =
{
  accountId : ['$adminNick' , .... ] ,
  ....
}

$Bridges =
{
  'bridgeName' :
  {
    $BRIDGE_NAME_KEY : 'bridgeName' ,
    $ENABLED_KEY     : aBool ,
    $CHANNELS_KEY    : { channelId : 'channelName' , .... }
  } ,
  ....
}
*/