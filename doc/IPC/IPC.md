# IPC

## 파이프
- 프로세스가 아닌 운영체제에 속하는 자원이다.
	- fork함수의 호출에 의한 복사대상이 아니다.
- OS가 마련해준 메모리공간을 통해 두 프로세스가 통신할 수 있게된다.
```C
int pipe(int filedes[2]);
//		=====PIPE=====
//write[1]            [0]read
//		==============
```