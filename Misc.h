#pragma once
#include "SDK.h"

class CMisc
{
public:

	void Run(CBaseEntity* pLocal, CUserCmd* pCommand, bool bSendPacket);
	bool Spam;
	int timescale = 1;

private:
	void NoisemakerSpam(PVOID kv);
};

extern CMisc gMisc;