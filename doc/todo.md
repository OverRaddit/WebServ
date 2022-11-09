설정파일
- main에 인자없을시 기본설정파일로 동작.
- listen 뒤에 여러 숫자 들어오게.
- delete, upload는 같은 root에서 동작.
- type 지우기

멀티플렉싱
-  하나의 서버블럭에 여러 포트존재.
-  노파일명 && 빈바디 -> 에러처리
-  파일명을 파싱해서 cgi요청인지 확인 후 플래그 설정.

Response
- =boundary는 form태그로 파일전송시에만 유효. 인지할것.
  - size_t i = content_type.find("boundary="); 이 부분 예외처리필요.
- sudo dir를 하드코딩에서 변수사용하도록 수정할 것.
- download는 mandatory달성 & 테스트통과전까지 보류.
- location block "return"

- GET
  - if file exist
    - view file
  - if file is directory
    - find index file
      - if not exist, 404

  -

- POST
  - file name exist
    - make file with req body
  - file name not exist
    - ...?

- DELETE
  - find file in root.
    - delete file
  - if file not exist
    - error

- type 모두대체.
  - cgi flag
  - upload flag
- 정적파일요청 수행후, 플래그에 따라 값 가공.



오늘의 할일
- 최종 request를 출력할것.
- cgi flag
-
- 현재 upload가 안되는 이유
  - HTTP method에서 get/post를 구분하지 않기 때문이다.
  - file을 찾지못할때,
    - POST/PUT	: 새로운 파일을 생성함
    - GET		: 404 error

파일명이 없을때. -> 무조건 404

파일명은 있는데 유효하지 않은 파일명일때.
- POST/PUT	: 새로운 파일을 생성함
- GET		: 404 error


원코드에서 파일명이 ""인 경우와 아닌경우를 나누었는데 굳이 나눌 필요가 있었을까?

[O]파이프에 데이터 보낼때, read한 뒤에 eof 붙이기.
	- 읽고 EOF찍고 strlen만큼만 write한다.

[O]파이프에서 eof를 만날때만 로그찍기. 안그러면 미완성본이 여러번 출력될테니..
request도 완성본을 만들고나서 로그찍기
	현재 memory leak 우려파트 존재..


파일fd가 업로드(쓰기용)인지 조회용(읽기용)인지 알아야함?

write, 업로드 -> write를 모두 수행.
	- 파일생성후, 생성한 파일을 반환.

read, 조회 -> write이벤트 감지할 필요 없음.
	- 파일읽어서 반환.\

====================================

error페이지를 반환하도록...!
Test GET Expected 404 on http://localhost:4240/directory/Yeah
content returned: H