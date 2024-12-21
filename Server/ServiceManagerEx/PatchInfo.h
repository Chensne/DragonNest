#pragma once

enum PatchTarget
{
	Exe,
	Full
};

struct PatchInfo
{
	wstring url;
	PatchTarget target;
};