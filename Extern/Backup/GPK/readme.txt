From: Eric Lee [mailto:ericlee@eyedentitygames.com] 
Sent: Wednesday, November 05, 2008 3:12 PM
To: '장지환'
Cc: '우승연'; '오태훈'
Subject: RE: GPK 샘플폴더에 있던 readme.txt 인데요 번역좀 해주세여 엉엉 ㅠ

 

기술적인 용어도 있고 전문(全文)이 요약 내용이라서 번역이 다소 잘못 됐을 수도 있다는 점 참조 바라며 실제 작업 중 이상 있다면 언제든지 알려주세요. 

 

???明：테스트 설명

Demo1 - 不???加解密的GPK演示 다이나믹 암호화를 포함하지 않는 GPK 데모

Demo2 - ???加解密的一???演示 다이나믹 암호화를 포함하는 간단한 데모

Demo3 - ???加解密的?完整的Client/Server的GPK演示 다이나믹 암호화를 포함하는 비교적 온전한 클라이언트/서버 GPK 데모

 

各示例的正常?行需要一?Web服?器(假?在本地)，用于GPK?接到上面?行升???。각 예제의 정상적인 실행은 웹 서버(현지에 임시설치)를 필요로 하며, GPK가 웹 서버에 접속해 업데이트를 검사하는 데 사용된다. 

????行如下配置: 테스트 시 아래의 배치(할당)을 진행한다. 

1. 假?本地服?器的根?http://127.0.0.1 임시설치한 현지 서버의 루트를 http://127.0.0.1로 한다. 

2. 在根下建一? [/Updates/TestGame/] 子目? 루트 아래 [/Updates/TestGame/] 서브 리스트(?) 를 만든다.

3. 把[Redist\Client\gpk]目?下的文件 ?制到 [/Updates/TestGame/] 目?下 [Redist\Client\gpk] 리스트 아래의 파일을  [/Updates/TestGame/] 리스트 아래 복사한다. 

 

像??配置好后我?的升?URL地址就?http://127.0.0.1/Updates/TestGame 이와 같이 배치(할당)하고 나면 (우리들의?!) 업데이트 URL 주소가 http://127.0.0.1/Updates/TestGame로 된다.

 

注意：주의사항

1. 在linux之?的系?上url是大小?敏感的，?注意程序中url的大小? linux와 같은 시스템에서는 (문자의?) 대소 구분이 민감하므로 프로그램 중 URL의 대소 구분에 주의해야 한다. 

2. ?不要在web站点上?用gzip，否?程序不能解析升?信息 웹사이트 상에서 gzip을 실행하지 말아야 한다. 그렇지 않으면 프로그램이 업데이트 정보를 해석 및 분석할 수 없다. 

 

演示程序??好后，?行之前需要注意以下事?：데모 프로그램의 편집(edit 또는 compile) 후 실행 전에 아래의 사항에 대해 주의해야 한다.

1. ??? [Redist\Server\DynCodeBin] 中的??代?是否已?放到了程序中指定的路?下，Server端?用到 [Redist\Server\DynCodeBin]의 다이나믹 코드가 프로그램 중 지정한 방법(또는 길?) 아래 놓여졌고 (또는 복사됐고 or 만들어졌고) 서버가 사용할 수 있는 지 확인한다. 

2. Client端主程序目?下需要建一?名叫gpk的子目?，然后放上 [Redist\Client\gpk] 中的AutoUpdate.dll和SddynDll.dll。（其他文件可以不放，程序??的?候?自?升?下?）클라이언트 메인 프로그램 리스트 아래 gpk라는 이름으로 서브 리스트를 만든다. 다음으로 [Redist\Client\gpk] 중 AutoUpdate.dll과 SddynDll.dll을 놓는다(복사한다?). (다른 파일들은 복사하지 않아도 된다. 프로그램이 실행될 때 자동으로 업데이트할 것이다.) 

3. ??保Client端和Server端的主程序能?到他?需要的GPKitCltDll.dll和GPKitSvr.dll。클라이언트와 서버의 메인 프로그램이 GPKitCltDll.dll과 GPKitSvr.dll을 찾을 수 있는 지 확인한다.

 
