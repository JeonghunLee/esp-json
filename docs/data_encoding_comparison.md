# DSL (Data Serialization Languages)

</br>

각 DSL의 Encoding ? 

[Go Back](./index.md)

</br>

## Binary 부분 개요

| 구분 | TLV (Tag-Length-Value) | ASN.1 (Abstract Syntax Notation One) | JSON (JavaScript Object Notation) |
|------|-------------------------|--------------------------------------|-----------------------------------|
| **핵심 구조** | Tag + Length + Value | Type + Length + Value (BER/DER) | Key + Value (Text-based) |
| **형식** | Binary | Binary (BER/DER/CER) | Text |
| **대표 사용처** | 통신 프로토콜, USIM, NFC, TLV-encoded certificates | X.509, LTE/5G NAS, SNMP, Cryptography | Web API, Config, Log, IoT |
| **가독성** | 낮음 | 낮음 | 매우 높음 |
| **확장성** | 낮음 (고정 Tag 구조) | 중간 (스키마 기반) | 높음 (Key-value 자유 형식) |
| **성능** | 빠름 (파싱 단순) | 중간 (복잡한 규격 파싱 필요) | 느림 (문자열 처리) |
| **스키마 의존성** | 낮음 | 높음 (ASN.1 Schema 필요) | 낮음 |
| **표준화** | 비공식적 (산업별 정의) | ITU-T X.680 계열 표준 | ECMA-404, RFC 8259 |
| **예시 표현** | `0x5A 0x08 0x1234567890ABCDEF` | `SEQUENCE { name UTF8String, age INTEGER }` | `{ "name": "Lee", "age": 42 }` |
| **장점** | 단순, 효율적, 짧은 헤더 | 엄격한 타입 관리, 인증서 등 보안 표준 적합 | 직관적, 범용, 언어 독립 |
| **단점** | 의미적 메타데이터 부족 | 복잡, 디버깅 어려움 | 크고 느림, 이진 전송 비효율 |

------------------------------------------------------------------------

## 각 Binary 사용할 경우 구조 비교

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

------------------------------------------------------------------------

### 2. ASN.1 (Abstract Syntax Notation One)

</br>

-   **정의:** 데이터 구조를 *형식 언어로 기술*하고, 인코딩은 BER/DER/CER
    등으로 수행\

-   **구조:**

    -   Type → Length → Value\
    -   DER (Distinguished Encoding Rules): 인증서 등 보안 표준에 필수
        사용\

-   **예시:**

    ``` asn1
    Person ::= SEQUENCE {
        name UTF8String,
        age INTEGER
    }
    ```

    인코딩 결과 (DER):

        0x30 0x0A 0x0C 0x04 4C6565 0x02 0x01 2A

    → `SEQUENCE(length=10) { UTF8String("Lee"), INTEGER(42) }`

--------

</br>

### 3.JSON (JavaScript Object Notation)

-   **정의:** 텍스트 기반 직렬화 포맷으로 사람이 읽기 쉬움\

-   **예시:**

    ``` json
    {
      "name": "Lee",
      "age": 42
    }
    ```

-   **장점:** Key 기반 구조로 의미 명확, 다양한 언어에서 지원\

-   **단점:** Binary 전송에 비효율적, 구조 변경 시 파서 수정 필요  --> Binary 변경고려 하여 극복 

</br>

## 4. 선택 기준


</br>


  |환경                           |  추천 포맷        | 이유            |        
  ------------------------------- | -----------------| --------------------------- |
  | MCU / RTOS 기반 (리소스 제한)  | **TLV**           | 메모리 효율, 파싱 단순       |
  | 보안·인증 시스템               |   **ASN.1 (DER)** |  표준화된 서명/암호화 구조    |
  | REST / IoT / Web API         |    **JSON**      |    범용성, 가독성            |


</br>

## 5. 혼합 구조 (현대적 접근)


</br> 

최근 시스템은 세 가지 방식을 **혼합**해서 사용. 
- **헤더:** TLV        
- **본문:** JSON (문자열 혹은 Base64 인코딩)     
- **메타데이터 서명:** ASN.1 DER

예시:
``` plaintext
Tag=0x10, Len=0x40, Value={ "cmd": "update", "ver": "1.2.0" }
Signature: ASN.1 DER encoded
```


## 6. 정리 요약

</br>

-   **TLV**: 구조 단순, 빠르지만 의미 표현력 낮음         
-   **ASN.1**: 표준화·보안용 강점, 복잡하지만 정형화  --> ITU-T의 대부분 포맷      
-   **JSON**: 범용적·가독성 높음, 성능은 낮음  


</br>