#include "Misc.h"
#include "Util.h"
#include "Client.h"
#include "Aimbot.h"

CMisc gMisc;

#define TICK_INTERVAL			(gInts.globals->interval_per_tick)


#define TIME_TO_TICKS( dt )		( (int)( 0.5f + (float)(dt) / TICK_INTERVAL ) )
#define TICKS_TO_TIME( t )		( TICK_INTERVAL *( t ) )

#define tick_interval gInts.globals->interval_per_tick
#define tick_intervalsqr tick_interval * tick_interval

float AngleNormalize(float angle)
{
	while (angle < -180)    angle += 360;
	while (angle > 180)    angle -= 360;

	return angle;
}

void CMisc::Run(CBaseEntity* pLocal, CUserCmd* pCommand, bool bSendPacket)
{
	if (gCvars.misc_serverlag && Util->IsKeyPressed2(gCvars.misc_serverlag_key))
		for (int i = 0; i < (int)gCvars.misc_serverlag_value; i++)
			gInts.Engine->ServerCmd("use", false);

	if (!pLocal || pLocal->IsAlive() == false)
		return;

	if (!(pLocal->GetFlags() & FL_ONGROUND) && pCommand->buttons & IN_JUMP)
	{
		//Autostrafe
		if (gCvars.misc_autostrafe)
			if (pCommand->mousedx > 1 || pCommand->mousedx < -1)  //> 1 < -1 so we have some wiggle room
				pCommand->sidemove = pCommand->mousedx > 1 ? 450.f : -450.f;

		//Bunnyhop
		if (gCvars.misc_bunnyhop)
			pCommand->buttons &= ~IN_JUMP;
		
		if (gCvars.misc_speedhack)
		{
			gInts.cvar->ConsoleColorPrintf(Color(255, 105, 180, 255), "host_timescale 7");
		}

		if (gCvars.misc_speedhack2)
		{
			gInts.cvar->ConsoleColorPrintf(Color(255, 105, 180, 255), "host_timescale 1");
		}
	}

	Vector m_vOldViewAngle = pCommand->viewangles;
	float m_fOldSideMove = pCommand->sidemove;
	float m_fOldForwardMove = pCommand->forwardmove;

	CBaseCombatWeapon *pWeapon = pLocal->GetActiveWeapon();
	if (gCvars.misc_angles)
	{
		if (pCommand->buttons & IN_ATTACK)
			return;

		if (gCvars.misc_aax > 0) //Pitch
		{
			if (gCvars.misc_aax == 1)//Fake Up
				pCommand->viewangles.x = -271.0;

			if (gCvars.misc_aax == 2)//Up
				pCommand->viewangles.x = -89.0f;

			if (gCvars.misc_aax == 3)//Fake Down
				pCommand->viewangles.x = 271.0;

			if (gCvars.misc_aax == 4)//Down
				pCommand->viewangles.x = 89.0f;
		}

		if (gCvars.misc_aay > 0) //Yaw
		{
			if (gCvars.misc_aay == 1)
			{
				if (bSendPacket)
					pCommand->viewangles.y += 90.0f;
				else
					pCommand->viewangles.y += -90.0f;
			}

			if (gCvars.misc_aay == 2)
			{
				if (bSendPacket)
					pCommand->viewangles.y += -90.0f;
				else
					pCommand->viewangles.y += 90.0f;
			}

			if (gCvars.misc_aay == 3)
			{
				if (bSendPacket) pCommand->viewangles.y += -90.0f;
				else pCommand->viewangles.y += 0.0f;
			}

			if (gCvars.misc_aay == 4)//Half back left
			{
				if (bSendPacket)
					pCommand->viewangles.y += 135.0f;
				else
					pCommand->viewangles.y += -135.0f;
			}

			if (gCvars.misc_aay == 5)//Half left right
			{
				if (bSendPacket)
					pCommand->viewangles.y += -135.0f;
				else
					pCommand->viewangles.y += 135.0f;
			}

			if (gCvars.misc_aay == 6)
			{
				if (bSendPacket) 
					pCommand->viewangles.y += 90.0f;
				else 
					pCommand->viewangles.y += 0.0f;
			}

			if (gCvars.misc_aay == 7)
			{
				{
					static bool right = false, left = false;
					if (GetAsyncKeyState(VK_LEFT) & 1) {
						right = false, left = true;
					}
					else if (GetAsyncKeyState(VK_RIGHT) & 1) {
						right = true, left = false;
					}

					if (left && !right) {
						if (bSendPacket) pCommand->viewangles.y += -90.0f;
						else pCommand->viewangles.y += 90.0f;
					}
					else if (right && !left) {
						if (bSendPacket) pCommand->viewangles.y += 90.0f;
						else pCommand->viewangles.y += -90.0f;
					}
				}
			}
		}
		
	}
	

	ConVar* viewmodel_fov = gInts.cvar->FindVar("viewmodel_fov");
	if (gCvars.misc_viewmodelfovoverride)
		viewmodel_fov->SetValue(gCvars.misc_viewmodelfov_value);

	if (gCvars.misc_fastcrouch && (pCommand->buttons & IN_DUCK))
	{
		if (pCommand->buttons & IN_ATTACK)
			return;
		
		Vector vLocalAngles = pCommand->viewangles;
		float speed = pCommand->forwardmove;
		if (fabs(speed) > 0.0f) 
		{
			pCommand->forwardmove = -speed;
			pCommand->sidemove = 0.0f;
			pCommand->viewangles.y = vLocalAngles.y;
			pCommand->viewangles.y -= 180.0f;
			if (pCommand->viewangles.y < -180.0f) pCommand->viewangles.y += 360.0f;
			pCommand->viewangles.z = 90.0f;
		}
	}

	if (GAME_TF2)
	{
		if (gCvars.misc_noisemaker_spam)
		{
			PVOID kv = Util->InitKeyValue();
			if (kv != NULL)
			{
				NoisemakerSpam(kv);
				gInts.Engine->ServerCmdKeyValues(kv);
			}
		}
	}

	Util->FixMove(pCommand, m_vOldViewAngle, m_fOldForwardMove, m_fOldSideMove);
}

void CMisc::NoisemakerSpam(PVOID kv) //Credits gir https://www.unknowncheats.me/forum/team-fortress-2-a/141108-infinite-noisemakers.html
{
	char chCommand[30] = "use_action_slot_item_server";
	typedef int(__cdecl* HashFunc_t)(const char*, bool);
	static DWORD dwHashFunctionLocation = gSignatures.GetClientSignature("FF 15 ? ? ? ? 83 C4 08 89 06 8B C6");
	static HashFunc_t s_pfGetSymbolForString = (HashFunc_t)**(PDWORD*)(dwHashFunctionLocation + 0x2);
	*(PDWORD)((DWORD)kv + 0x4) = 0;
	*(PDWORD)((DWORD)kv + 0x8) = 0;
	*(PDWORD)((DWORD)kv + 0xC) = 0;
	*(PDWORD)((DWORD)kv + 0x10) = /*0x10000*/0xDEADBEEF;
	*(PDWORD)((DWORD)kv + 0x14) = 0;
	*(PDWORD)((DWORD)kv + 0x18) = 0; //Extra one the game isn't doing, but if you don't zero this out, the game crashes.
	*(PDWORD)((DWORD)kv + 0x1C) = 0;
	*(PDWORD)((DWORD)kv + 0) = s_pfGetSymbolForString(chCommand, 1);
}