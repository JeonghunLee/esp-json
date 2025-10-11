# 🧩 DSL (Domain-Specific Language)

</br>

[Go Back](index.md)

요즘 Github Action 부터 Cmake 의 기술언어? 부터 다양하게 사용,        
이유는 사람이 보기 편하며, 이 기반으로 Parsing해서 각 쉽게 적용하여 진행        

</br>

## 1. DSL 개요

</br>

DSL(Domain-Specific Language)은 **특정 도메인(분야)에 특화된 언어**를 의미하며,  
설정 파일 포맷인 **YAML**과 **TOML**도 대표적인 DSL로 분류.
거의 JSON에서 다 같이 출발한 것 같음   



</br>

## 2. YAML vs TOML 개요 비교

</br>


| 항목 | **YAML (YAML Ain’t Markup Language)** | **TOML (Tom’s Obvious Minimal Language)** |
|------|----------------------------------------|--------------------------------------------|
| 목적 | 계층적이고 사람 친화적인 설정 표현 | 단순하고 명확한 키-값 설정 표현 |
| 구조 방식 | 들여쓰기 기반 계층 구조 | `[section]` 기반 명시적 구조 |
| 주요 사용처 | Kubernetes, Docker Compose, ESP-IDF 등 | Python, Rust, AI Agent 설정 등 |
| 표준화 상태 | 느슨한 비공식 표준 | 명확한 1.0.0 공식 표준 존재 |


</br>

</br>

## 3. 문법 비교 예시

### YAML


</br>

```yaml
server:
  host: "localhost"
  port: 8080

database:
  user: "root"
  password: "1234"
  timeout: 5.0

features:
  - wifi
  - bluetooth
  - sensor
```

</br>

### TOML


</br>

```toml
[server]
host = "localhost"
port = 8080

[database]
user = "root"
password = "1234"
timeout = 5.0

features = ["wifi", "bluetooth", "sensor"]
```

</br>



## 4. 구조 및 문법 차이


</br>

| 구분 | YAML | TOML |
|------|------|------|
| 계층 구조 | 들여쓰기(공백 중요) | `[section]` / `[sub.section]` |
| 데이터 타입 | 문자열, 숫자, 불리언, 리스트, 맵 등 | 문자열, 숫자, 배열, 날짜/시간 등 |
| 주석 | `#` | `#` |
| 배열 | `- item1` | `[ "item1" ]` |
| 멀티라인 문자열 | `\|` 또는 `>` | `\"\"\" ... \"\"\"` |
| 문법 오류 허용 | 민감함 | 엄격하지만 일관적 |
| 표준화 | 비공식 | v1.0.0 정식 표준 |


</br>

## 5. 장단점 비교


</br>

| 구분 | **YAML** | **TOML** |
|------|-----------|-----------|
| 장점 | - 계층 구조 표현이 직관적<br>- 사람이 읽기 쉬움<br>- 주석 자유로움 | - 문법 단순<br>- 파서 일관성<br>- 표준화 완비 |
| 단점 | - 공백/들여쓰기 오류 발생<br>- 복잡한 구조는 가독성 저하 | - 복잡한 구조 표현 제한<br>- 배열+구조체 조합 불편 |
| 대표 사용처 | DevOps, 임베디드 설정, 배포 시스템 | AI Agent, Python/Rust 설정, BSP 세팅 |



</br>

## 6. 선택 가이드


</br>

| 상황 | 추천 형식 | 이유 |
|------|------------|------|
| AI Agent / SDK 구성 | **TOML** | 단순한 키-값 기반, 표준화 |
| Firmware / RTOS 설정 | **YAML** | 계층 구조 표현 용이 |
| 단일 앱 설정 | **TOML** | 간결한 구조 유지 |
| 복잡한 시스템 관리(K8s 등) | **YAML** | 계층형 데이터 표현에 강함 |


</br>

## 7. 요약


</br>

| 항목 | YAML | TOML |
|------|------|------|
| 문법 복잡도 | ★★★ | ★ |
| 가독성 | ★★☆ | ★★★ |
| 계층 표현력 | ★★★ | ★★ |
| 표준화 정도 | ★ | ★★★ |
| AI Agent / Tool Chain 적합성 | △ | ◎ |


</br>

## 8. 결론


</br>

- **YAML** → 복잡한 시스템 구성 표현에 강함  
- **TOML** → 단순하고 일관된 설정 관리에 강함  


</br>

> 즉, **“사람이 읽기 쉬운 DSL”은 YAML**,  
> **“기계가 안정적으로 처리하기 쉬운 DSL”은 TOML** 이라고 정리


</br>

[Go Back](./index.md)        