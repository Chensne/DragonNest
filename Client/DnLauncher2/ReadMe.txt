
-- �ű� ���� ��ó --

�ۼ��� : ������
�ۼ��� : 2010.07.02

* ��ó ���� ���� ���� ����
	- ��ó���� ����ϴ� ���ҽ��� �������� ������ Resource������.lib�� ����Ѵ�.
	- ������ lib ������Ʈ�� DnLauncher���� ������ ��ġ��
	
* Resource lib ���� ����
	1. DnLauncher ������Ʈ Define.h���� ������ ���ҽ� ���� define�� �����Ѵ�.
	2. Visual Studio���� �� ������Ʈ Win32 -> Win32 Project (Static Library) DnLauncher ������Ʈ ������ ����
	3. ������Ʈ �������� Use MFC in a Static Library ���
	4. DnLauncher ������Ʈ�� �߰� (������ ��ó���� ���Ǵ� C++, Resource �� �� ���� �� ���� ��) ��� ���� Output���� ���� �ٽ� ����
	5. ResourceInit.h, cpp ���� ���� �߰� / ���ҽ� ���� resource.h ���� ���� ���ҽ� Copy & Paste (�̹��� ���� ����)
	6. DnLauncher������Ʈ���� rc���� ���� �� ��ΰ� �ٸ��� ������ rc���� �� bitmap ��θ� ������ define���� �����ؼ� �߰�
	7. rc ���� �ϴ� rc2 include ���� �߰� / ������Ʈ Resource Files�� �߰�
	8. Resource������.rc2 ���Ͽ� String ���� (����)
	
	
* ������ ���̺귯�� ������Ʈ ���� �� Ȯ�� ����
	- ������Ʈ ���ÿ��� Output Directory ���� : $(SolutionDir)\Output\$(ConfigurationName)
	- �̹��� jpg Ȯ���� �߰��� ���ҽ� ������ �̿����� �� �� rc ���� ���� edit

* Country ������ ������ ServiceModule, DnLauncherDlg ��� �޾� �ۼ� �Ѵ� ( ������ �䱸���׿� �����ϱ� ���� �и� )
* ������Ʈ �Ӽ� �߰����Ե��丮�� ��� �߰�

	
	
	
	
 * ���ǻ���
	- ���ҽ� ������� ������ �� rc ���Ͽ� �������� ���Ե� ���׵� ������� ��
	(DnLauncher.rc : #include "Define.h" / Resource������.rc : rc2���� include �� �̹��� ���� ���� define���� ������ �κ�)
	
