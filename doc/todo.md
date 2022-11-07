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