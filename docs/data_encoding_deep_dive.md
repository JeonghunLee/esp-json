# DSL (Data Serialization Languages) 

</br>

  * TLV 
  * ASN.1 
  * JSON + 심화


</br>

## 1. 기본 비교 (TLV / ASN.1 / JSON)

</br>

| 항목 | TLV | ASN.1 (BER/DER) | JSON |
|---|---|---|---|
| 핵심 구조 | Tag + Length + Value | Type + Length + Value | Key + Value |
| 인코딩 형식 | Binary | Binary | Text |
| 스키마 | 불필요 | 필수(ASN.1 명세) | 불필요 |
| 대표 사용처 | USIM, EMV, 임베디드 | X.509, LTE/5G NAS, SNMP | Web API, 설정, 로그 |
| 장점 | 단순·빠름·오버헤드 작음 | 타입 안전·표준 적합성 | 가독성·개발자 친화 |
| 단점 | 의미 표현력 낮음 | 구현/디버깅 복잡 | 이진 비효율/부피 큼 |

</br>

## 2. 직렬화 포맷의 진화 과정

</br>

| 세대 | 대표 기술 | 주요 특징 | 등장 배경 |
|---|---|---|---|
| 1세대 | TLV, Bencoding | 단순 태그 기반 이진 | 초기 통신·저장 효율 중심 |
| 2세대 | ASN.1, XDR, Protobuf | 스키마 기반 이진 | 상호운용성·타입 안전 |
| 3세대 | JSON, XML, YAML | 사람이 읽는 텍스트 | 웹·REST 확산 |
| 4세대 | CBOR, MessagePack, Avro | 하이브리드/자기기술형 | IoT·엣지·클라우드 |

</br>

## 3. TLV 구조 개요

</br>

```
┌────────┬────────┬────────────────────┐
│ Tag    │ Length │ Value │
├────────┼────────┼────────────────────┤
│ 0x01 │ 0x04 │ 0xDEADBEEF │
└────────┴────────┴────────────────────┘
```
- 장점: 파싱 단순, MCU/RTOS에 적합, 고정 오버헤드
- 유의: 의미(context) 부족 → 매핑 테이블/사양서 필요
- 사용: USIM/EAP-AKA/EMV, 임베디드 펌웨어 헤더 등

</br>
## 4. ASN.1 개요와 DER 인코딩 예
</br>

- ASN.1은 **데이터 타입/구조를 정의**하는 IDL이며, BER/DER/CER 등 규칙으로 인코딩함
- DER은 보안 표준(X.509 등)에 요구되는 **단일/정규화 인코딩**

```
-- 정의 (ASN.1)
Person ::= SEQUENCE {
name UTF8String,
age INTEGER
}

-- DER (예시)
30 0A 0C 04 4C 65 65 02 01 2A
SEQUENCE(10) { UTF8String("Lee"), INTEGER(42) }
```


</br>

## 5. JSON 개요와 예

</br>

- 텍스트 기반 직렬화, 가독성 및 도구 생태계 우수
- 이진 전송에는 비효율적이므로 필요 시 Base64/별도 바이너리 포맷 결합

```json
{
  "name": "Lee",
  "age": 42
}
```

## 6. 환경별 선택 가이드

</br>

| 환경 | 추천 포맷 | 이유 |
|------|------------|------|
| MCU / RTOS (리소스 제한) | TLV | 파서 단순, 메모리 및 오버헤드 적음 |
| PKI / 보안 인증 | ASN.1 (특히 DER) | 글로벌 표준 적합 (예: X.509) |
| 개발자 친화 / 사람이 읽는 데이터 | JSON / YAML | 디버깅과 도구 지원 용이 |
| 실시간 / RPC / 스트리밍 | Protobuf / FlatBuffers | 성능 우수, 버전 관리 용이 |
| IoT / 엣지 환경 | CBOR / MessagePack | JSON 의미 + 이진 효율 |
| 데이터 분석 / 저장 | Avro / Parquet | 스키마 진화, 압축 효율 |

</br>

## 7. 실제 결합 사례

</br>

| 시스템 | 헤더 | 페이로드 | 보안 / 인증 |
|---------|-------|------------|--------------|
| 5G NAS (EAP-AKA) | TLV | ASN.1 (DER) | USIM 기반 키 인증 |
| TLS / SSL | 레코드 (TLV 유사) | ASN.1 (X.509) | 서명 / 암호화 |
| IoT (CoAP / ESP32) | CBOR / JSON | TLV 메타데이터 | AES / HMAC |
| REST API | HTTP 헤더 | JSON | TLS / JWT |
| AI 모델 설정 (ONNX / TF) | FlatBuffers / Proto | Binary Tensor | Hash / Signature |

</br>

## 8. IDL vs 직렬화 포맷

</br>

| 개념 | 정의 | 예시 |
|------|-------|------|
| IDL (Interface Definition Language) | 데이터 및 인터페이스 정의 언어 | ASN.1, Thrift, Protobuf (.proto), CORBA IDL |
| Serialization Format | 전송 가능한 바이트 형태로 인코딩 | DER, JSON, CBOR, TLV |

**설명:**  
- ASN.1과 Protobuf는 *IDL + 인코딩 규칙*을 포함함.  
- TLV와 JSON은 *순수 직렬화 포맷* 성격이 강함.

</br>

## 9. 요약     

</br>

| 포맷 | 특징 및 적용 방향 |
|------|------------------|
| TLV | 빠르고 단순하지만 의미 표현력 제한 → 명세로 보완 필요 |
| ASN.1 | 타입 안전성과 표준화 우수, 복잡도 존재 → 보안·인증용 적합 |
| JSON | 가독성·생태계 우수, 바이너리 비효율 → 하이브리드 구성 권장 |
| 현대적 조합 | **TLV(헤더) + JSON(페이로드) + ASN.1/DER(보안)** 형태의 계층 결합이 일반적 |