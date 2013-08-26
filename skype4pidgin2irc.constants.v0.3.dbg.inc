<?php
$DEBUG = true ; $DEBUGVB = $DEBUG && true ; $DEBUG_ALL = $DEBUG && true ;
// TODO:
//  require initialization from what will be the master control channel
//    ideally a private password protected channel before any channels may be bridged
//    then require that any nick posting admin commands be authorized manually
//    on a per nick per network basis (perhaps via an ?/auth command)
//  apply some sane upper bound on # of channels
//  implement load and store config commands based on accountId amd room name?

/*\
|*| CONFIGURATION:
|*|   *IMPORTANT* - 
|*|   add to $ADMIN_NICKS nicks for ALL networks that pidgin will connect to
|*|   this must be done in order to prevent infinte loopback
|*|   *IMPORTANT* - 
|*|   this script only matches nicknames and will consider to be admin
|*|     ANY nick on ANY network that matches ANY of the nicks listed in $ADMIN_NICKS
|*|     so be certain that your nicks are quite unique (e.g. 'fred' is not recommended)
|*|     this bug/feature is at the top of the TODO: list but for this version be aware
|*|   add to $ADMIN_NICKS any additional nicks that require admin access
|*|     but note that $ADMIN_NICKS may chat on individual channels as usual
|*|     and their messages will not propogate to other channels unless properly prefixed
|*|   add to $STAR_IRC_COMMANDS any IRC commands that should append $STAR_PREFIX to nick
|*|   add to $STRIP_IRC_COMMANDS any IRC commands that you would like simply stripped
\*/
$ADMIN_NICKS         = [] ; // e.g ['MyIrcNick' , 'MyYahooNick' , ] ;
$STAR_IRC_COMMANDS   = ['/me ' , '\01/me'] ; // TODO: untested
$STRIP_IRC_COMMANDS  = ['/'] ;
$STAR_PREFIX         = "* " ;
$NICK_PREFIX         = "(from " ; // dont use '<' some clients will supress it as html
$NICK_POSTFIX        = ') ' ;
$BRIDGE_NICK         = 'BRIDGE' ;
$DEFAULT_BRIDGE_NAME = 'default' ;

$TRIGGER_PREFIX    = '?/' ;
$ADD_TRIGGER       = 'add' ;
$REMOVE_TRIGGER    = 'rem' ;
$DISABLE_TRIGGER   = 'disable' ;
$ENABLE_TRIGGER    = 'enable' ;
$STATS_TRIGGER     = 'status' ;
$ECHO_TRIGGER      = 'echo' ;
$CHAT_TRIGGER      = 'chat' ;
$BROADCAST_TRIGGER = 'broadcast' ;
$EXIT_TRIGGER      = 'shutdown' ;

$USAGE_MSGa          = "to begin send the chat message:" ;
$USAGE_MSGb          = "\ninto all chats you would like to bridge" .
                       "\nrefer to the file 'README' for more options" ;
$BOGUS_DATA_MSG      = "bogus data received" ;
$CH_SET_MSG          = "channel set to bridge" ;
$BRIDGE_CONFLICT_MSG = "each channel may only be on one bridge" ;
$THIS_CHANNEL_MSG    = "this channel [" ;
$CHANNEL_EXISTS_MSG  = "] already exists on bridge" ;
$THIS_BRIDGE_MSG     = "] is on bridge" ;
$UNBRIDGED_MSG       = "] is not bridged" ;
$CHANNEL_REMOVED_MSG = "channel removed from bridge" ;
$ENABLE_MSG          = "bridge" ;
$ENABLED_MSG         = "is enabled" ;
$DISABLED_MSG        = "is disabled" ;
$NO_SUCH_BRIDGE_MSG  = "no such bridge" ;
$NO_BRIDGES_MSG      = "no bridges exist" ;
$STATS_MSGa          = "bridge" ;
$STATS_MSGb1         = "- enabled -" ;
$STATS_MSGb2         = "- disabled -" ;
$STATS_MSGc          = "channels bridged" ;
$BRIDGE_DELETED_MSG  = "bridge deleted" ;
$EXIT_MSG            = "shutting down" ;
$UNKNOWN_MSG         = "unknown command" ;

$NAME_KEY            = 'bridgeName' ;
$CHANNELS_KEY        = 'channels' ;
$ENABLED_KEY         = 'isEnabled' ;

$INTERFACE        = 'im.pidgin.purple.PurpleInterface' ;
$CHAT_METHOD      = 'ReceivedChatMsg' ;
$PURPLE_SERVICE   = 'im.pidgin.purple.PurpleService' ;
$PURPLE_OBJECT    = '/im/pidgin/purple/PurpleObject' ;
$PURPLE_INTERFACE = 'im.pidgin.purple.PurpleInterface' ;
