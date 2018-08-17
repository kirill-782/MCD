/*

   Copyright [2008] [Trevor Hogan]

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.

   CODE PORTED FROM THE ORIGINAL GHOST PROJECT: http://ghost.pwner.org/

*/

#ifndef GAMESLOT_H
#define GAMESLOT_H

#include "includes.h"

#define SLOTSTATUS_OPEN		0
#define SLOTSTATUS_CLOSED	1
#define SLOTSTATUS_OCCUPIED	2

#define SLOTRACE_HUMAN		1
#define SLOTRACE_ORC		2
#define SLOTRACE_NIGHTELF	4
#define SLOTRACE_UNDEAD		8
#define SLOTRACE_RANDOM		32
#define SLOTRACE_SELECTABLE	64

#define SLOTCOMP_EASY		0
#define SLOTCOMP_NORMAL		1
#define SLOTCOMP_HARD		2

const int MAX_SLOTS = 12;

//
// CGameSlot
//

class CGameSlot
{
private:
	unsigned char m_PID;				// player id
	unsigned char m_DownloadStatus;		// download status (0% to 100%)
	unsigned char m_SlotStatus;			// slot status (0 = open, 1 = closed, 2 = occupied)
	unsigned char m_Computer;			// computer (0 = no, 1 = yes)
	unsigned char m_Team;				// team
	unsigned char m_Colour;				// colour
	unsigned char m_Race;				// race (1 = human, 2 = orc, 4 = night elf, 8 = undead, 32 = random, 64 = selectable)
	unsigned char m_ComputerType;		// computer type (0 = easy, 1 = human or normal comp, 2 = hard comp)
	unsigned char m_Handicap;			// handicap

public:
	GAPI CGameSlot( BYTEARRAY &n );
	GAPI CGameSlot( unsigned char nPID, unsigned char nDownloadStatus, unsigned char nSlotStatus, unsigned char nComputer, unsigned char nTeam, unsigned char nColour, unsigned char nRace, unsigned char nComputerType = 1, unsigned char nHandicap = 100 );
	GAPI ~CGameSlot( );

	GAPI unsigned char GetPID( )				{ return m_PID; }
	GAPI unsigned char GetDownloadStatus( )	{ return m_DownloadStatus; }
	GAPI unsigned char GetSlotStatus( )		{ return m_SlotStatus; }
	GAPI unsigned char GetComputer( )		{ return m_Computer; }
	GAPI unsigned char GetTeam( )			{ return m_Team; }
	GAPI unsigned char GetColour( )			{ return m_Colour; }
	GAPI unsigned char GetRace( )			{ return m_Race; }
	GAPI unsigned char GetComputerType( )	{ return m_ComputerType; }
	GAPI unsigned char GetHandicap( )		{ return m_Handicap; }

	GAPI void SetPID( unsigned char nPID )							{ m_PID = nPID; }
	GAPI void SetDownloadStatus( unsigned char nDownloadStatus )		{ m_DownloadStatus = nDownloadStatus; }
	GAPI void SetSlotStatus( unsigned char nSlotStatus )				{ m_SlotStatus = nSlotStatus; }
	GAPI void SetComputer( unsigned char nComputer )					{ m_Computer = nComputer; }
	GAPI void SetTeam( unsigned char nTeam )							{ m_Team = nTeam; }
	GAPI void SetColour( unsigned char nColour )						{ m_Colour = nColour; }
	GAPI void SetRace( unsigned char nRace )							{ m_Race = nRace; }
	GAPI void SetComputerType( unsigned char nComputerType )			{ m_ComputerType = nComputerType; }
	GAPI void SetHandicap( unsigned char nHandicap )					{ m_Handicap = nHandicap; }

	GAPI BYTEARRAY GetByteArray( ) const;
};

#endif
