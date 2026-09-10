# GAMETECHLAB04 Week02 Team10 Project

## 팀원
박석원 권성욱 황혜진

## 코딩 규약 (Coding Convention)

### Upper Camel Case
- Camel Case : 낙타 모양에서 따온 방법으로 첫 글자는 소문자로 시작, 두 번째 단어부터는 대문자로 표현하는 방식으로 많이 사용합니다.
- Upper Camel Case or Pascal Case : 첫 글자를 대문자로 시작하는 Camel Case
  
### 기타 클래스 명명법
파일 이름에는 접두사 X
예시) 
Object.h
class UObject (O)

UObject.h
class UObject (X)

- Generic Class 의 접두사 T
- Structure의 접두사 F
- Non-Skeleton Mesh - Static Mesh
- Skeleton Mesh - Skeletal Mesh
- Effect - Particle
- 3D Vector - FVector
- 4D Vector - FVector4
- 4x4 Matrix - FMatrix

### Object와 Class의 이름은 UE를 따른다.
- Unreal Engine - Actor, UObject
- Unity Engine - GameObject, MonoBehaviour
- CryEngine - Entity

## 1. 브랜치 구조 및 역할

### main
- 실제 최종 빌드 및 배포가 이루어지는 프로덕션 브랜치입니다.
- 직접적인 커밋은 금지하며, development 브랜치에서 충분한 검증을 마친 후 병합(Merge)합니다.
- 항상 즉시 빌드 및 실행이 가능한 안정적인 상태를 유지합니다.

### development
- 모든 개발 및 수정 사항이 통합되는 중심 브랜치입니다.
- 작업 브랜치(feat, fix 등)는 이 브랜치에서 분기하고, 작업 완료 후 PR을 통해 이 브랜치로 병합합니다.
### 작업 브랜치 (feat, fix, refactor, chore)
- 단위 작업(기능 추가, 버그 수정 등)을 위해 일시적으로 생성하는 브랜치입니다.
- development 브랜치로부터 생성되며, 작업 완료 및 PR 병합 후 삭제합니다.

## 2. 브랜치 명명 규칙 (Branch Naming Convention)
- 작업 브랜치는 생성 전 반드시 GitHub 이슈를 먼저 등록한 뒤, 해당 이슈 번호를 포함하여 명명합니다.
- 네이밍 형식: <타입>/#<이슈번호>-<간략한작업내용>
- 브랜치 예시:
  - 기능 개발: feat/#12-login-ui
  - 버그 수정: fix/#23-network-disconnect
  - 코드 개선: refactor/#30-player-controller
  - 환경 설정: chore/#5-project-setup
## 3. Git 커밋 컨벤션 (Conventional Commits 기반)
- 커밋 메시지는 제목, 본문, 바닥글 (Footer) 형태로 작성하며, 본문과 바닥글은 선택 사항입니다.
```
<타입>:<제목>

[본문(선택)]

[바닥글(선택)]
```
- Feat: 새로운 기능 추가
- Fix: 버그 수정
- Refactor: 프로덕션 코드 리팩터링 (기능 변화 없음)
- Style: 코드 포맷팅, 세미콜론 누락 등 (비즈니스 로직 변화 없음)
- Docs: 문서 수정 (README, 주석 등)
- Test: 테스트 코드 추가 또는 수정
- Chore: 빌드 설정, 패키지 매니저 관리 등 기타 변경사항

### 작성 규칙
- 제목은 50자 이내의 명령조(동사 원형)로 작성합니다.
- 제목 끝에 마침표(.)를 찍지 않습니다.
- 본문에는 '무엇을', '왜' 변경했는지를 상세히 작성합니다.

## 4. 풀 리퀘스트(Pull Request) 템플릿

```
## 📌 개요
- 작업 목적 및 변경 사항을 간략히 요약합니다.

## 🔗 관련 이슈
- Closes #이슈번호

## 🛠 주요 변경 사항
- 주요 로직 변경점 1
- 주요 로직 변경점 2

## 📸 스크린샷 / 실행 결과 (선택)
- UI 변경 또는 기능 동작을 확인할 수 있는 이미지/GIF

## 🧪 테스트 방법
- [ ] 테스트 케이스 1 통과 여부
- [ ] 테스트 케이스 2 통과 여부

## 💬 리뷰어에게
- 특별히 확인을 요청하는 부분이나 고려해야 할 사항을 기재합니다.
```
## 5. 이슈(Issue) 템플릿
1. 기능 제안 (.github/ISSUE_TEMPLATE/feature_request.md)
```
## 🚀 기능 설명
- 제안하려는 기능에 대해 명확하고 간결하게 설명합니다.

## 🎯 목표 및 필요성
- 이 기능이 왜 필요한지와 기대 효과를 작성합니다.

## 📋 세부 작업 항목 (To-Do)
- [ ] 구현 작업 1
- [ ] 구현 작업 2
- [ ] 테스트 작성

## 💡 대안 및 기타 고려사항
- 고려했던 다른 대안이나 추가 참고 자료를 작성합니다.
```
2. 버그 리포트 (.github/ISSUE_TEMPLATE/bug_report.md)

```
---
name: Bug Report
about: 버그 제보
title: '[FIX] '
labels: bug
---

## 🐛 버그 설명
- 발생한 버그에 대해 간략히 설명합니다.

## 🔁 재현 단계
1. '...' 페이지로 이동
2. '....' 버튼 클릭
3. '....' 에러 발생 확인

## 🖥 예상 동작 vs 실제 동작
- **예상 동작**: 정상 동작 설명
- **실제 동작**: 실제 발생한 오류 상태 설명

## 📋 환경 정보
- OS: 
- 브라우저/플랫폼 버전: 
- 관련 로그/스크린샷:
```
