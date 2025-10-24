# Linux Device Tree 

</br>

이 부분은 과연 FreeRTOS에서 필요한가??             
필요하다면, Why? Linux의 경우 각 다 세부적인 것을 변경을 쉽게 하려하지만, 
FreeRTOS의 경우, 보통 OS 작어서 불필요 할 듯??       
RTOS는 전체구조를 환경변수 기반으로 바꾸는게 낮을 듯함     

</br>

[Go Back Main Index](./index.md)

</br>

## DTB/DTC 와 libfdt

</br>

Linux 의 Device Tree Blob/Compiler (DTB/DTC) 와 libfdt 를 이용하여, 쉽게 외부에서 Image를 변경시 변경이 가능함 


* DTB/DTC 와 libfdt             
    예전부터 Device Tree 부분을 Flex 와 Bison 기반으로 구현을 했을 거라고, 생각을 했는데,      
    이부분이 ChatGPT와 하면서 생각이 변경됨                  

</br>




