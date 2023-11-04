# 0. RaspberryPi
### 건국대학교 전기전자공학부 - 임베디드시스템 17조
> * **담당교수**  
>   조용범 교수님
> * **조원**  
>   202110965 이관호  
>   201810804 김세연  
>   202014122 김진구

### Raspberry Pi Info
* OS : ~~Buildroot~~ Raspbian (RaspberryPi OS)  
* Version : RaspberryPi 4 - Model B

### Github Clone Code
```
https://github.com/vanillaPenguin/RaspberryPi.git
```

# 1. SSH
* 라즈베리 파이는 부팅과 동시에 연결 가능한 네트워크를 찾고, 연결한다.
> **라즈베리 파이를 wifi에 연결할 수 없는 경우**
> 1. 다음의 코드를 실행하여 현재 사용 가능한 네트워크의 목록을 확인할 수 있다.
>    ```bash
>    # nmcli device wifi list
>    ```
> 2. 사용 가능한 네트워크가 있다면, 다음의 코드를 통해 연결할 수 있다.  
>    이때, SSID가 와이파이 이름, PASSWORD가 비번이다.
>    ```bash
>    # nmcli device wifi connect [SSID] password [PASSWORD]
>    ```
>    
* 만약 라즈베리 파이가 올바르게 연결되었다면, 외부에서 ssh를 통해서 접속할 수 있다.
그러나 라즈베리 파이와 ssh로 연결하려면, PC와 같은 네트워크에 연결되어 있어야 하고, 기타 네트워크 설정이 맞아야 한다 (어떤 wifi를 사용하느냐에 따라서 다른데, 내부 디바이스들의 통신을 막아둔 공유기들이 있다).
* ssh로 연결하려면 다음과 같은 코드를 연결하고자 하는 PC에서 실행해 주면 된다
```bash
$ ssh pi@raspberrypi.local
```
또는
```bash
$ ssh pi@[RaspberryPi_IP]
```
> **라즈베리 파이의 IP를 모르겠다면?**
> * 다음의 코드를 실행해서 IP를 알아낼 수 있다.  
>   이때 inet 옆에 xxx.xx.x.xx/xx 이런식으로 되어 있을 텐데, `/`가 나오기 이전까지가 ip 주소이다.
>    ```bash
>    # ip a
>    ```
>    

# 2. Raspberry Pi 사용이 끝났다면
* 다음과 같은 코드를 실행해서 라즈베리 파이를 올바르게 종료할 수 있다.
  이는 `# sudo shutdown -h now`와 같다.
  ```bash
  # sudo shutdown now
  ```  
* 혹은, 일정 시간이 지난 후에 전원이 꺼지기를 바란다면 다음과 같이 쓸 수 있다. 예를 들어, 5분 뒤에 전원을 끄는 명령어는 다음과 같다.
  ```bash
  # sudo shutdown -h +5
  ```
