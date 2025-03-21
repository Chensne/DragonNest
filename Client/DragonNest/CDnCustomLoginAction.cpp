#include "stdafx.h"
#include <vector>
#include <string>
#include "Singleton.h"
#include "CDnCustomLoginAction.h"
#include "DnPlayerActor.h"
#include "DnCompoundBase.h"

CDnCustomLoginAction::CDnCustomLoginAction()
{
}

CDnCustomLoginAction::~CDnCustomLoginAction()
{	
} 

void CDnCustomLoginAction::PlayLoginAction(DnActorHandle player)
{
	std::vector<std::string> CustomActions;// = new std::vector<std::string>(10);
	if(CustomActions.size() == 0)
	{
		CustomActions.push_back("Social_Anger01");
		CustomActions.push_back("Social_Bow01");
		CustomActions.push_back("Social_Greet01");
		CustomActions.push_back("Social_Laugh01");
		CustomActions.push_back("Social_No01");
		CustomActions.push_back("Social_Sorrow01");
		CustomActions.push_back("Social_Yes01"); 
	}
	
	srand((size_t)time(NULL));
	int no=rand()%CustomActions.size();
	//player->SetActionQueue( "PutIn_Weapon", 0, 0.f, 0.f, false, false );
	player->SetActionQueue(CustomActions[no].c_str(), 0, 0.f, 0.f, false, false );
}