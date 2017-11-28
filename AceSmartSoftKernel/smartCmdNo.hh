// --------------------------------------------------------------------------
//
//  Copyright (C) 2008 Alex Lotz
//
//        lotz@hs-ulm.de
//        schlegel@hs-ulm.de
//
//        Prof. Dr. Christian Schlegel
//        University of Applied Sciences
//        Prittwitzstr. 10
//        D-89075 Ulm
//        Germany
//
//
//  This file is part of ACE/SmartSoft.
//
//  ACE/SmartSoft is free software: you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  ACE/SmartSoft is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License
//  along with ACE/SmartSoft.  If not, see <http://www.gnu.org/licenses/>.
//
// --------------------------------------------------------------------------

#ifndef SMARTCMDNO_HH_
#define SMARTCMDNO_HH_

namespace SmartACE {

/// this enum defines <b>all</b> possible commands between smartsoft-<b>patterns</b>.
/** @enum SmartCommand
 * This emuneration defines <b>all</b> possible <b>commands</b> between <em>client-</em> and <em>server-patterns</em> in <b>smartsoft-kernel</b>.
 * This command-ids are used as first parameter in every kernel-message-header.<br>
 *
 * 1) Pattern <b>independent</b> commands<br>
 * <br>
 * Two way <b>connect</b>:
 * <table>
 * <tr> <td>CMD_CONNECT</td> <td>R0 <b>connect</b> command (used in <b>ALL</b> <em>clients</em>)</td></tr>
 * <tr> <td>CMD_ACK_CONNECT</td> <td>A0 <b>acknowledge-connect</b> command (used in <b>ALL</b> <em>servers</em>)</td></tr>
 * </table>
 * <br>One way <b>discard</b>:
 * <table>
 * <tr> <td>CMD_DISCARD</td>  <td>R1 <b>discard</b> command (used in <b>ALL</b> <em>clients</em>)</td> </tr>
 * </table>
 * <br>Two way <b>disconnect</b>:
 * <table>
 * <tr> <td>CMD_DISCONNECT</td>  <td>R2 <b>disconnect</b> command (used in <b>ALL</b> <em>clients</em>)</td> </tr>
 * <tr> <td>CMD_ACK_DISCONNECT</td>  <td>A2 <b>acknowledge-disconnect</b> command (used in <b>ALL</b> <em>servers</em>)</td> </tr>
 * </table>
 * <br>One way <b>server-initiated-disconnect</b>:
 * <table>
 * <tr> <td>CMD_SERVER_INITIATED_DISCONNECT</td>  <td>R3 <b>server-initiated-disconnect</b> command (used in <b>ALL</b> <em>servers</em>)</td> </tr>
 * </table>
 * <br>2) Pattern <b>specific</b> commands:<br>
 * <br><b>SEND</b> pattern:
 * <table>
 * <tr> <td>CMD_COMMAND</td>  <td><b>send-data</b> command of <b>SEND</b> <em>client</em> pattern</td> </tr>
 * </table>
 * <br><b>QUERY</b> pattern:
 * <table>
 * <tr> <td>CMD_REQUEST</td> <td>D <b>request-data</b> command of <b>QUERY</b> <em>client</em> pattern</td> </tr>
 * <tr> <td>CMD_ANSWER</td> <td>D <b>answer-data</b> command of <b>QUERY</b> <em>server</em> pattern</td> </tr>
 * </table>
 * <br><b>PUSH_NEWEST</b> pattern:
 * <table>
 * <tr> <td>CMD_UPDATE</td>  <td>D <b>update-data</b> command of <b>PUSH_NEWEST</b> <em>server</em> pattern</td> </tr>
 * <tr> <td>CMD_SUBSCRIBE</td>  <td>R4 <b>subscribe</b> command of <b>PUSH_NEWEST</b> <em>client</em> pattern</td> </tr>
 * <tr> <td>CMD_ACK_SUBSCRIBE</td>  <td>A4 <b>acknowledge-subscribe</b> command of <b>PUSH_NEWEST</b> <em>server</em> pattern</td> </tr>
 * <tr> <td>CMD_UNSUBSCRIBE</td>  <td>R5 <b>unsubscribe</b> command of <b>PUSH_NEWEST</b> <em>client</em> pattern</td> </tr>
 * </table>
 * <br><b>PUSH_TIMED</b> pattern:
 * <table>
 * <tr> <td>CMD_GET_SERVER_INFO</td> <td>R6 <b>get-server-info</b> command of <b>PUSH_TIMED</b> <em>client</em> pattern</td> </tr>
 * <tr> <td>CMD_SERVER_INFO</td> <td>A6 <b>send-server-info</b> command of <b>PUSH_TIMED</b> <em>server</em> pattern</td> </tr>
 * <tr> <td>CMD_ACTIVATION_STATE</td> <td>R7 <b>send-activation-state</b> command of <b>PUSH_TIMED</b> <em>server</em> pattern</td> </tr>
 * </table>
 * <br><b>EVENT</b> pattern:
 * <table>
 * <tr> <td>CMD_EVENT</td> <td>D <b>event-data</b> command of <b>EVENT</b> <em>server</em> pattern</td> </tr>
 * <tr> <td>CMD_ACTIVATE</td> <td>R4 <b>activate</b> command of <b>EVENT</b> <em>client</em> pattern</td> </tr>
 * <tr> <td>CMD_ACK_ACTIVATE</td> <td>A4 <b>acknowledge-activate</b> command of <b>EVENT</b> <em>server</em> pattern</td> </tr>
 * <tr> <td>CMD_DEACTIVATE</td> <td>R5 <b>deactivate</b> command of <b>EVENT</b> <em>client</em> pattern</td> </tr>
 * </table>


 */
enum SmartCommand {
   // Pattern independent commands
   CMD_CONNECT,                     ///< R0 <b>connect</b> command 
   CMD_ACK_CONNECT,                 ///< A0 <b>acknowledge-connect</b> command

   CMD_DISCARD,                     ///< R1 <b>discard</b> command

   CMD_DISCONNECT,                  ///< R2 <b>disconnect</b> command
   CMD_ACK_DISCONNECT,              ///< A2 <b>acknowledge-disconnect</b> command

   CMD_SERVER_INITIATED_DISCONNECT, ///< R3 <b>server-initiated-disconnect</b> command

   //SEND pattern
   CMD_COMMAND,                     ///< D <b>send-data</b> command

   //QUERY pattern
   CMD_REQUEST,                     ///< D <b>request-data</b> command
   CMD_ANSWER,                      ///< D <b>answer-data</b> command

   // PUSH_NEWEST pattern
   CMD_UPDATE,                      ///< D <b>update-data</b> command
   CMD_SUBSCRIBE,                   ///< R4 <b>subscribe</b> command 
   CMD_ACK_SUBSCRIBE,               ///< A4 <b>acknowledge-subscribe</b> command
   CMD_UNSUBSCRIBE,                 ///< R5 <b>unsubscribe</b> command

   // PUSH_TIMED pattern
   CMD_GET_SERVER_INFO,             ///< R6 <b>get-server-info</b> command
   CMD_SERVER_INFO,                 ///< A6 <b>send-server-info</b> command
   CMD_ACTIVATION_STATE,            ///< R7 <b>send-activation-state</b> command

   // EVENT pattern
   CMD_EVENT,                       ///< D <b>event-data</b> command
   CMD_ACTIVATE,                    ///< R4 <b>activate</b> command
   CMD_ACK_ACTIVATE,                ///< A4 <b>acknowledge-activate</b> command
   CMD_DEACTIVATE                   ///< R5 <b>deactivate</b> command
};

}  // END namespace SmartACE

#endif /*SMARTCMDNO_HH_*/
