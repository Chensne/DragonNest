#pragma once


// ���ҽ� �н� ������ ���� �������̽�
// RTCutSceneCore ���� �н� ������ ���� ���� ���� �� ���� ������ �̷� ������� �ذ�..
class IResourcePathFinder
{
public:
	IResourcePathFinder(void);
	virtual ~IResourcePathFinder(void);

	virtual void GetFullPath( const char* pFileName, /*IN OUT*/string& strFullPath ) { };
};
