

<sdoa4server.ini 설명>

sdoa4server.ini는 OA 서버단 SDK의 config 파일입니다.
정확한 내용은 아래와 같습니다.

#config 서버 주소
CONF_SERVER = 210.51.30.84:80

#OAS 서버 주소
OAS_SERVER = 210.51.30.84:9061

#해당 서버명칭, sdoa.sdo.com 사이트에 등록한것과 일치해야 함
LOCAL_ALIAS = defaultalias

#log output 경로, 설정하지 않으면 디폴트로 현재 목록아래의 log 목록임
#LOG_PATH = C:\

그 중 CONF_SERVER는 샨다의 config 서버로서, 요청을 해오는 서버에 대응되는 config 파일을 할당해줍니다. 
OAS_SERVER는 샨다의 OAS authentication 서버로서, 서버가 샨다 OA gateway의 방문권한을 컨트롤합니다.
CONF_SERVER = 210.51.30.84:80와 OAS_SERVER = 210.51.30.84:9061는 OA joint debugging 환경 설정으로서 개발팀에서 테스트 시 변경할 필요 없습니다. (실서비스 환경 설정은 정식 서비스 시 제공)
LOCAL_ALIAS는 서버 별명입니다. 샨다 gateway 방문 authentication은 IP + 별명(alias)의 방식으로 유니크하게 서버 신분을 감별합니다. Gateway를 방문할 서버는 우선 open.sdo.com(OA foreground)에 로그인하여 서버 IP와 별명을 등록해야 합니다. 
등록 후 sdoa4server.ini config 파일의LOCAL_ALIAS에 해당 사이트에 등록한 별명을 기입하시면 됩니다.


