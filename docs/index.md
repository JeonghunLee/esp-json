# JSON 기능 

</br>

* Embedded Board    
    * TEST Board: **ESP32-S3 DevkitC-1**        
    * ESP-IDF: v5.4      


## JSON <-> Bin 변환목적    

</br>

JSON Encode/Decode를 Embedded Board에서 TEST 하기 위해서 ESP32-S3 EVM 선택     
추후 RTOS에서 활용기능   
그냥 CBOR/TinyCBor로 사용?  

</br>


### JSON to Bin Encoding/Bin Decoding

</br>

처음 Flex 와 Bison 으로 구현을 생각했으나, JSON parsing 은 도 다 chatGPT가 구현을 함              
현재 모든 생각/개념이 변경되어짐        

</br>

* JSON to Bin Encoding/Decoding
    - JSON Parsing to Bin Encoding 기능 구현     
    - Bin Decoding 은 구현      
[TEST Result ](test_result.md)

</br> 



## DSL(Domain Specific Language)

</br>    

이 부분은 각 사람 보기 쉬운 언어이며, Description 언어기반으로 관리를 중점을 두지만, 
이를 해석하고, 이해해야 하는 것은 기계이며, 이를 항상 어떻게 쓸지를 모색??  
   
</br>    

* DSL/JSON 문법 과 Encoding 방법    
    * [DSL 문법](./dsl_yamlvstoml.md)  
    * [JSON/DSL Encoding 비교](./data_encoding_comparison.md)         
    * [JSON/DSL Encoding 고려](./data_encoding_deep_dive.md)       

</br>    

### DSL(Domain Specific Language) 의 사용되는 예들 

</br>    

ESP-IDF의 경우, 별도의 yaml 을 자동으로 사용하며, 요즘 Yocto 뿐만 아니라, 거의 Description 언어로 많이 사용되어짐   
Github Action 뿐만 아니라 대 부분 자체로 사용해서 사용하는 경우가 많음     

</br>    

* ESP-IDF 의 YAML        
    [ESP-IDF](https://github.com/espressif/esp-idf/blob/master/examples/bluetooth/.build-test-rules.yml)        
    [ESP-IDF](https://github.com/espressif/esp-idf/blob/master/examples/build_system/.build-test-rules.yml)

상위구조를 보면, Cmake 빠른 자동 생성
    * 추후 관련 Paser Python 과 관련부분 조사   

어떻게 보면 거의 Github Action 과도 비슷함??   

</br>    

* Android 의 XML   
    [Manifest.xml-1](https://developer.android.com/guide/topics/manifest/manifest-intro?hl=ko)   
    [manifest.xml-2](https://developer.android.com/guide/topics/manifest/manifest-element?hl=ko)         

Android의 경우, 다양하게 사용 PDK에서 전체 빌드 하거나 App 빌드 

</br> 

### Github Copliot TEST 

</br> 


Markdown으로 아래와 같이 사용할 방법을 모색  
이유는 Code Convention 과 각 부분의 규칙 정의   

.github 부분에 정의  

</br> 