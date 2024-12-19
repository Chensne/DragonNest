#pragma once

class CEtOptionController
{
public:
	CEtOptionController(void);
	virtual ~CEtOptionController(void);

protected:
	SGraphicOption m_Option;

public:
	enum ReloadType
	{
		RELOAD_NONE,
		RELOAD_TERRAIN_MATERIAL,
		RELOAD_ALL_MATERIAL,
	};
	void Initialize( SGraphicOption &Option );
	void ValidateOption();
	void GenerateOptionMacro();
	void SetGraphicOption( SGraphicOption &Option );
	void GetGraphicOption( SGraphicOption &Option ) { Option = m_Option; }

	ShadowType GetShadowType() { return m_Option.DynamicShadowType; }
	bool IsEnableDynamicShadow() { return m_Option.DynamicShadowType != ST_NONE; }

	void CheckShadowMap();

	WaterQuality GetWaterQuality() { return m_Option.WaterQuality; }
	int GetMaxDirLightCount() { return m_Option.nMaxDirLightCount; }
	int GetMaxPointLightCount() { return m_Option.nMaxPointLightCount; }
	int GetMaxSpotLightCount() { return m_Option.nMaxSpotLightcount; }

	bool IsEnableDOF() { return m_Option.bEnableDOF; }
	bool IsEnableMotionBlur() { return m_Option.bEnableMotionBlur; }

	int GetGraphicQuality() { return m_Option.nGraphicQuality; }
};

extern CEtOptionController g_OptionController;
inline CEtOptionController *GetEtOptionController() { return &g_OptionController; }