# JSON 관련 기능 

</br>

* Embedded Board    
    * TEST Board: ESP32-S3 Devkit EVM        
    * ESP-IDF: v5.4      


JSON Encode/Decode를 Embedded Board에서 TEST 하기 위해서 ESP32-S3 EVM 선택     
추후 RTOS에서 활용???     

</br>


## DTB/DTC 와 libfdt

</br>

Linux 의 Device Tree Blob/Compiler (DTB/DTC) 와 libfdt 를 이용하여, 쉽게 외부에서 Image를 변경시 변경이 가능함 


* DTB/DTC 와 libfdt             
    예전부터 Device Tree 부분을 Flex 와 Bison 기반으로 구현을 했을 거라고, 생각을 했는데,      
    이부분이 ChatGPT와 하면서 생각이 변경됨                  

</br>

## JSON to Bin Encoding/Bin Decoding

</br>

처음 Flex 와 Bison 으로 구현을 생각했으나, JSON parsing 은 도 다 chatGPT가 구현을 함              
현재 모든 생각/개념이 변경되어짐        
추후 DSL(Domain Specific Language)(yaml)도???          


* JSON to Bin Encoding/Decoding
    - JSON Parsing to Bin Encoding 기능 구현     
    - Bin Decoding 은 구현      

</br> 

## DSL(Domain Specific Language)

</br>    

ESP-IDF의 경우, 별도의 yaml 을 자동으로 사용하는데, 이부분 확인   

</br>    

* ESP-IDF 의 YAML        
    [ESP-IDF](https://github.com/espressif/esp-idf/blob/master/examples/bluetooth/.build-test-rules.yml)        
    [ESP-IDF](https://github.com/espressif/esp-idf/blob/master/examples/build_system/.build-test-rules.yml)

</br>    

* Android 의 XML  
    [Manifest.xml](https://developer.android.com/guide/topics/manifest/manifest-intro?hl=ko)   
    [manifest.xml](https://developer.android.com/guide/topics/manifest/manifest-element?hl=ko)         

</br>    

* DSL 문법 과 Encoding 
    * [DSL 문법](./dsl_yamlvstoml.md)  
    * [DSL Encoding](./data_encoding_comparison.md)         
    * [DSL Encoding](./data_encoding_deep_dive.md)       

</br> 