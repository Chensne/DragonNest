** 주의

 다른 프로젝트(메이플 스토리) 등에서의 STL 사용 중복/충돌로 인한 문제로 인해서
MachineIdLib 의 SystemInfo 모듈은 STL 을 사용하지 않은 모듈이며, 따라서 
CA 의 SystemInfo 모듈과 같지 않다.

 게임방 클라이언트 정보를 추가할때는 CSystemInfo::GetGameRoomClient() 메써드 내
INSERT_CANDIDATE() 들로 이루어진 블럭만을 CA 의 SystemInfo 와 맞추면 된다.

( MachineId 에 대한 보다 자세한 사항은
  "PC방 Blocking을 위한 MachineId Library System.doc" 문서 참조)
