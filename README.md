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

# 1. SSH - Raspberry Pi
* 라즈베리 파이는 부팅과 동시에 연결 가능한 네트워크를 찾고, 연결한다.
> **라즈베리 파이를 wifi에 연결할 수 없는 경우**
> 1. 다음의 코드를 실행하여 현재 사용 가능한 네트워크의 목록을 확인할 수 있다.
>    ```bash
>    $ nmcli device wifi list
>    ```
> 2. 사용 가능한 네트워크가 있다면, 다음의 코드를 통해 연결할 수 있다.
>    ```baseh
>    $ nmcli device wifi connect [SSID] password [PASSWORD]
>    ```
>    이때, SSID가 와이파이 이름, PASSWORD가 비번이다.
