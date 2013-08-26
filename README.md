/*\ skype4pidgin2irc -->
|*|   a nifty little script that enables you
|*|     to bridge IRC channels and skype chats
|*|     as well any other service that libpurple supports
|*|       (icq , yahoo , aim , msn , myspace , google talk ,
|*|         twitter , facebook , identi.ca , and many others)
|*|
|*| motivation and credits -->
|*|   originally developed for the students and TAs of cs169 on edX
|*|       https://www.edx.org/course-list/uc%20berkeleyx/computer%20science/allcourses
|*|     as part of the educhat project created by sam joseph
|*|       https://sites.google.com/site/saasellsprojects/projects/educhat
|*|     using php and the dbus library to interface with pidgin IM
|*|       http://www.freedesktop.org/wiki/Software/dbus/
|*|       http://pecl.php.net/package/DBus  http://pidgin.im/
|*|     with a little persuasion from the blog of robertbasic
|*|       http://robertbasic.com/blog/communicating-with-pidgin-from-php-via-d-bus/
\*/


skype4pidgin2irc install instructions for debian (ymmv)


/* install pidgin and the skype plugin */

ensure you have access to the multiverse repositories
then update if necessary and install pidgin and its skype plugin

    sudo apt-get update
    sudo apt-get install pidgin pidgin-skype


/* install the php bridge */

install build dependencies if necessary which may include

    sudo apt-get install php-pear pkg-config php5-dev libdbus-1-dev libxml2-dev

there may be others as well and you may need to use your penguin skills to
ferret out any hints given in the next step when installing the dbus php bindings

    pecl install dbus-beta

Add the dbus extension to your 'php.ini' command line config

    echo "extension=dbus.so" >> /etc/php5/cli/php.ini

type the following command into a terminal and verify that 'dbus' is listed
along with the 'DBUS_SESSION_BUS_ADDRESS' environment variable

    php -r 'echo phpinfo() ;' | grep dbus


/* configure pidgin */

launch pidgin and create and activate an 'IRC' account
and set 'Username' and 'Server' under 'Login Options'
launch pidgin and create and activate a 'skype(D-Bus)' account
and set 'Username' under 'Login Options'

you can test that your pidgin accounts are setup properly
by running the 'list-pidgin-accounts.php' script via the terminal

    php list-pidgin-accounts.php

launch skype and it will ask you to grant API permission to pidgin


/* configure skype4pidgin2irc */

open the file 'skype4pidgin2irc.php' in a text editor

    nano skype4pidgin2irc.php

*IMPORTANT*
add to $ADMIN_NICKS nicks for ALL networks that pidgin will connect to
this must be done in order to prevent infinte loopback
*IMPORTANT*
this script only matches nicknames and will consider to be admin
ANY nick on ANY network that matches ANY of the nicks listed in $ADMIN_NICKS
so be certain that your nicks are quite unique (e.g. 'fred' is not recommended)
this bug/feature is at the top of the TODO: list but for this version be aware

add to $ADMIN_NICKS any additional nicks that require admin access
but note that $ADMIN_NICKS may chat on individual channels as usual
and their messages will not propogate to other channels unless properly prefixed

save and launch 'skype4pidgin2irc.php' via the terminal

    php skype4pidgin2irc.php

in pidgin open a chat session with each channel you want to bridge
you may need to get someone else to chat into the skype group
before it will appear in pidgin - then post the following chat message
into each of the channels youd like to bridge

    ?/add
or
    ?/add a-bridge-name

this script handles multiple bridges with multiple channels each
there is currently no limit to the number of channels that may be bridged (uayor)
to create a another discrete bridge repeat as above with a distinct bridge name

    ?/add another-bridge-name


/* nifty tips for additional awesomeness */

for added coolness i suggest changing you IRC nick to 'SKYPE'

    /nick SKYPE

and in skype set 'Real Name' in your profile to 'IRC'


/* admin commands */

the following admin commands are currently supported:

    ?/add
        - adds this channel to the default bridge
    ?/add 'BRIDGE_NAME'
        - adds this channel to the bridge 'BRIDGE_NAME'
    ?/rem
        - removes this channel from the default bridge
    ?/rem 'BRIDGE_NAME'
        - removes this channel from the bridge 'BRIDGE_NAME'
    ?/disable
        - temporarily disables all bridges temporarily
    ?/disable 'BRIDGE_NAME'
        - temporarily disables the bridge 'BRIDGE_NAME'
    ?/enable
        - enables all bridges
    ?/enable 'BRIDGE_NAME'
        - enables the bridge 'BRIDGE_NAME'
    ?/status
        - shows status information for all bridges
    ?/status 'BRIDGE_NAME'
        - shows status information for the bridge 'BRIDGE_NAME'
    ?/echo 'SOME_TEXT'
        - echoes text to the same channel
    ?/chat 'SOME_TEXT'
        - relays text to the all channels on this bridge
    ?/broadcast 'SOME_TEXT'
        - relays text to the all channels on all bridges as 'BRIDGE'
    ?/shutdown
        - kills the skype4pidgin2irc process

enjoy :)
