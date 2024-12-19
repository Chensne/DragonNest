
-- 신규 통합 런처 --

작성자 : 조신찬
작성일 : 2010.07.02

* 런처 구동 관련 참고 사항
	- 런처에서 사용하는 리소스는 국가별로 생성된 Resource국가명.lib를 사용한다.
	- 국가별 lib 프로젝트는 DnLauncher폴더 하위에 위치함
	
* Resource lib 생성 순서
	1. DnLauncher 프로젝트 Define.h에서 국가별 리소스 관련 define을 설정한다.
	2. Visual Studio에서 새 프로젝트 Win32 -> Win32 Project (Static Library) DnLauncher 프로젝트 하위에 생성
	3. 프로젝트 설정에서 Use MFC in a Static Library 사용
	4. DnLauncher 프로젝트에 추가 (국가별 전처리기 정의는 C++, Resource 텝 두 곳에 다 넣을 것) 출력 폴더 Output지정 관련 다시 셋팅
	5. ResourceInit.h, cpp 파일 복사 추가 / 리소스 생성 resource.h 내용 복사 리소스 Copy & Paste (이미지 파일 복사)
	6. DnLauncher프로젝트에서 rc파일 읽을 때 경로가 다르기 때문에 rc파일 내 bitmap 경로명 국가별 define으로 구분해서 추가
	7. rc 파일 하단 rc2 include 문구 추가 / 프로젝트 Resource Files에 추가
	8. Resource국가명.rc2 파일에 String 정리 (번역)
	
	
* 국가별 라이브러리 프로젝트 생성 시 확인 사항
	- 프로젝트 셋팅에서 Output Directory 설정 : $(SolutionDir)\Output\$(ConfigurationName)
	- 이미지 jpg 확장자 추가시 리소스 편집기 이용하지 말 것 rc 파일 직접 edit

* Country 폴더에 국가별 ServiceModule, DnLauncherDlg 상속 받아 작성 한다 ( 국가별 요구사항에 대응하기 위해 분리 )
* 프로젝트 속성 추가포함디렉토리에 경로 추가

	
	
	
	
 * 주의사항
	- 리소스 편집기로 편집할 시 rc 파일에 수동으로 기입된 사항들 사라지게 됨
	(DnLauncher.rc : #include "Define.h" / Resource국가명.rc : rc2파일 include 및 이미지 파일 국가 define으로 구분한 부분)
	
