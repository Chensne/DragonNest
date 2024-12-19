
class CDnCustomLoginAction : public CSingleton<CDnCustomLoginAction>
{
public:
	CDnCustomLoginAction();
	~CDnCustomLoginAction();
	void PlayLoginAction(DnActorHandle player);
};