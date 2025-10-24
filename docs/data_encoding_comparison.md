# DSL (Data Serialization Languages)

</br>

DSL/JSON 의 Binary Encoding 방식 비교    
   
</br>

[Go Back Main Index](./index.md)

</br>

## Binary 부분 개요

</br>




| 구분          | **TLV (Tag-Length-Value)**     | **ASN.1 (Abstract Syntax Notation One)**    | **Protobuf (Protocol Buffers)**                  | **CBOR (Concise Binary Object Representation)**            |
| ----------- | ------------------------------ | ------------------------------------------- | ------------------------------------------------ | ---------------------------------------------------------- |
| **핵심 구조**   | Tag + Length + Value           | Type + Length + Value (BER/DER)             | Field Number + Type + Value                      | Key + Value (Binary JSON)                                  |
| **형식**      | Binary                         | Binary (BER/DER/CER)                        | Binary                                           | Binary                                                     |
| **대표 사용처**  | USIM, NFC, ISO7816, 통신 프로토콜    | X.509, LTE/5G NAS, SNMP, Cryptography       | gRPC, Cloud RPC, IPC, Storage                    | IoT, CoAP, CBOR over MQTT, REST Binary                     |
| **가독성**     | 낮음                             | 낮음                                          | 낮음                                               | 중간 (Key 구조 유지)                                             |
| **확장성**     | 낮음 (고정 Tag 구조)                 | 중간 (스키마 기반)                                 | 높음 (Backward/Forward 호환성)                        | 높음 (Self-describing, Key 포함)                               |
| **성능**      | 매우 빠름 (단순 파싱)                  | 중간 (정의 복잡)                                  | 매우 빠름 (컴파일된 구조체 기반)                              | 빠름 (Key 포함, 동적 파싱)                                         |
| **스키마 의존성** | 낮음                             | 높음 (ASN.1 Schema 필요)                        | 높음 (`.proto` 정의 필요)                              | 낮음 (Schema 불필요)                                            |
| **표준화**     | 산업별 사양 (비공식적)                  | ITU-T X.680 계열 표준                           | Google Protocol Buffers 표준                       | RFC 8949 (IETF 표준)                                         |
| **예시 표현**   | `0x5A 0x08 0x1234567890ABCDEF` | `SEQUENCE { name UTF8String, age INTEGER }` | `message Person { string name=1; int32 age=2; }` | `{ "name": "Lee", "age": 42 } → b'\xa2dnamecLeeeage\x18*'` |
| **장점**      | 단순, 효율적, 헤더 짧음                 | 엄격한 타입 관리, 보안 표준 적합                         | 초고속, 경량, 버전 호환성 우수                               | JSON 호환 + 압축형, 자가 기술(Self-describing)                      |
| **단점**      | 의미 부족 (Tag 기반)                 | 복잡한 규격, 디버깅 어려움                             | 스키마 필요, 사람이 읽기 어려움                               | 크기 약간 큼 (Key 포함), 표준 다양                                    |



</br>

## 각 Binary 사용할 경우 구조 비교

</br>

* TLV → “가장 원시적이고 단순한 구조 (하드웨어/USIM용)”                
* 아래 2개는 스키마 방식로 거의 유사   
    * ASN.1 → “보안·통신 표준에 강한 구조적 스키마형”            
    * Protobuf → “성능·효율 극대화된 구조적 스키마형”          
* CBOR → “JSON과 호환되는 자가기술형(Binary JSON)”               

</br>

### 1. TLV (Tag-Length-Value)

</br>

``` plaintext
┌────────┬────────┬────────────────────┐
│  Tag   │ Length │       Value        │
├────────┼────────┼────────────────────┤
│ 0x01   │ 0x04   │ 0xDEADBEEF         │
└────────┴────────┴────────────────────┘
```

</br>

-   **장점:** 파싱 단순, 임베디드·RTOS 환경에 적합\
-   **단점:** 의미(context)가 부족해 명시적 매핑 테이블 필요\
-   **예시 용도:** USIM, EAP-AKA, EMV 카드, LTE NAS 메시지

</br>

### 2. ASN.1 (Abstract Syntax Notation One)

</br>

* 동작        
ITU-T 계열의 형식 언어로, 통신 프로토콜(Q.931 부터 다양함)의 데이터 구조를 기술함     
인코딩은 BER / DER / CER / PER 등 다양한 규칙을 지원.      
ITU-T 문서를 봤다면, 모를 수가 없다 

</br>

* 구조
    * Type → Length → Value
    *  DER (Distinguished Encoding Rules): 인증서 등 보안 표준에 필수사용 (ITU-T 관련부분 참조 )

* 예시

``` asn1
Person ::= SEQUENCE {
    name UTF8String,
    age INTEGER
}
```

인코딩 결과 (DER):
```
    0x30 0x0A 0x0C 0x04 4C6565 0x02 0x01 2A
    → `SEQUENCE(length=10) { UTF8String("Lee"), INTEGER(42) }`

```

</br>

### 3. Protobuf (Protocol Buffers)       

</br>

탄생 자체가 Protocol 용이고, 거의 ASN.1과 거의 유사하다.  
ASN.1 과 Protobuf는 같이 생각해야함      

</br>

.proto 스키마 기반의 Encoding/Decoding 
    * ASN.1 가 거의 유사    
```proto
message SensorData {
    float temperature = 1;
    string unit = 2;
    repeated float history = 3;
}


┌────────────┬──────────────┬────────────────────────────┐
│ Field Num. │ Wire Type    │         Encoded Data       │
├────────────┼──────────────┼────────────────────────────┤
│     1      │ Varint(5)    │  0x0D 0x9A 0x99 0x19 0x41  │  → temperature=9.6
│     2      │ Length-delim │  0x12 0x01 0x43            │  → unit="C"
│     3      │ Length-delim │  0x1A 0x06 0x00 0x00 0x20… │  → history[ ]
└────────────┴──────────────┴────────────────────────────┘
```

* 장점:     
    * 구조적·정형화된 데이터 (정적 .proto 스키마 기반)
    * 매우 빠른 직렬화/역직렬화 속도
    * Backward/Forward 호환성 (필드 번호 기반)

* 단점:
    * 스키마 필수 (.proto 파일 없으면 해석 불가)
    * 문자열 가독성 낮음 (field number 중심 구조)

* 예시 용도:
    * gRPC, Cloud RPC, 내부 IPC
    * 대규모 분산 시스템, 머신러닝 모델 파이프라인

</br>

* JSON 과 Protobuf 변환 시 문제사항     

```JSON
{
  "device_id": "esp32-s3-001",
  "temperature_c": 25.3,
  "tags": ["wifi","ble"],
  "seq": 1024,
  "ts_unix_ms": 1729770000123
}
```

* JSON to Bin 의 변환의 의미    
    * 사실 순서를 변경하지 않고 사용하면, 이전과 호환이 가능하지만, Checksum/CRC 문제        
    * 환경변수로 사용시 고려해볼만 하지만, 이전 proto 와 변경후 proto의 호환성 문제      
    * 만약 사용해야 한다면, 각 CRC or Checksum을 Update 할때마다 용량별로 구조를 구성해야함 
        * v1 checksum 범위 와 checksum   
        * v2 checksum 범위 와 checksum   
```.proto(proto3)
syntax = "proto3";

package telemetry;

message SensorData {
  string device_id      = 1;
  float  temperature_c  = 2;
  repeated string tags  = 3;
  uint32 seq            = 4;
  uint64 ts_unix_ms     = 5; // ms 단위 타임스탬프
}
```

</br>


| 구분        | JSON                               | Protobuf                      | 관계                             |
| --------- | ---------------------------------- | ----------------------------- | ------------------------------ |
| **형식**    | 텍스트(Text)                          | 이진(Binary)                    | ✅ 구조적 호환 (데이터 구조는 같지만 인코딩은 다름) |
| **직접 호환** | O (사람이 읽음)                         | ❌ (바이너리 전용, Base64 등으로 감싸야 함) | 변환 필요                          |
| **변환 방식** | `json -> proto_obj -> serialize()` | `deserialize() -> json`       | 변환기 또는 라이브러리 사용                |
| **핵심 차이** | Key 이름 존재                          | Field 번호 기반                   | 호환성은 스키마(proto 파일)에 의존         |


</br>


### 4. CBOR (Concise Binary Object Representation)     

</br>

현재 CBOR 대신, 직접 구성해서 만들어 볼 생각이며, CBOR의 불편사항을 일단 파악해야함 

</br>

JSON
```json 
{
    "temperature": 25.3,
    "unit": "C",
    "timestamp": 173505.12
}
```

</br>

* **JSON to CBOR**     
흔히 Json 을 Cbor 변경   
```Cbor
┌────────────┬─────────────┬──────────────────────────────┐
│   Key Len  │  Key        │           Value              │
├────────────┼─────────────┼──────────────────────────────┤
│    0x6B    │"temperature"│ 0xFB 0x40 0x39 0x33 0x33 ... │
│    0x04    │"unit"       │ 0x61 0x43                    │
│    0x09    │"timestamp"  │ 0xFB 0x40 F5 44 7A E1 47 AE  │
└────────────┴─────────────┴──────────────────────────────┘
```

</br>

* 장점:
    * JSON과 구조 1:1 매핑 가능 (Self-describing)
    * Schema 불필요, Key 포함 → 유연한 확장성
    * Binary 포맷으로 효율적 전송 (텍스트 대비 30~60% 절감)

* 단점:
    * Key 문자열 포함으로 Protobuf보다 약간 큼
    * 디버깅 불편 (Raw binary)

* 예시 용도:
    * IoT(ESP32/STM32) 센서 데이터 전송
    * CoAP, MQTT, LwM2M, WebAuthn
    * HTTP REST Binary Payload 교환

</br>



