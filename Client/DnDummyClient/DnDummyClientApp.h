#pragma once

class DnDummyClientApp : public wxApp
{
public:
	virtual bool OnInit();
	virtual void CleanUp();
};

DECLARE_APP(DnDummyClientApp)


