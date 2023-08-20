## 프로젝트 설명
DirectX 12 API를 이용한 Multithreaded rendering 엔진입니다.

현재 개발 중인 프로젝트입니다.<br>Visual studio 솔루션으로 빌드할 수 있습니다. 현재 DirectX 12 device, Task graph, Vertex to GPU를 구현한 상태이며, 빌드 시 기본 렌더링 화면을 볼 수 있습니다.

### 개발 목표
* 크게 RHI, Rendering, Object로 나누어 모듈로 제작하고, 최종적으로 각 모듈을 개별 배포할 수 있도록 합니다.
* DirectX 12로 렌더링을 하는 것에 그치지 않고, 멀티 쓰레딩을 사용하여 효율적인 렌더링 파이프라인 관리 시스템을 제공합니다.
* RHI에서 사용하는 API가 변경되었을 때에도 나머지 모듈은 수정없이 재사용할 수 있게 합니다.<br>e.g) 만약 Vulkan으로 현재 설계된 RHI의 규격에 맞춰 개발한다면 나머지 모듈은 재사용할 수 있어야 합니다.

### 프로젝트 설계 목표
> [프로젝트에 대해서](https://github.com/popoory67/DX12_Rendering/wiki/문서#프로젝트에-대해서)
<br>

## Branch 구조
**master, stable, test** 브랜치로 관리합니다.<br>개발 완료 내용에 따라 각 브랜치에 **test->stable->master** 순서로 머지합니다.

1) test에서 개발하고, 테스트합니다.<br>test 단계에서는 크래시나 버그, 예외 등이 발생할 수 있습니다.
2) 프로그램 실행에 특별한 이슈가 없다면 stable에 머지합니다.<br>stable에서는 반드시 문제가 발생하지 않도록 합니다.
3) 목표로 했던 피쳐 개발이 완료가 되었다면 master에 머지하여 개발을 완료합니다.

<br>

정리하자면, 각 브랜치의 용도는 다음과 같습니다.

* test<br>최신 코드를 적용하지만, 안정적이지 않아 문제가 발생할 수 있음
* stable<br>예외 발생이 없으며 개발 도중인 피쳐가 적용되어 있음
* master<br>개발이 완료된 피쳐가 안정적으로 적용되어 있음

<br>

## 문제점, 제한사항
미구현 사항이 매우 많습니다.
> Task graph를 추상적으로 구현하여 아직은 상황별로 발생하는 문제에 취약합니다.<br>이는 Task graph가 실제로 사용될 Render pass를 구현하면서 함께 안정화될 것으로 예상합니다.

### 미완성 기능
구체적인 내용은 구현 우선순위에 따라 다음과 같습니다.
	
    0) Task를 쓰레드 단위로 처리
    1) Vertex 처리 (Base pass)
    2) PSO load/save, 관리
    3) Basic material, Texture 처리 (Material pass, Post-process pass)
    4) Autodesk fbx load
    5) Skeletal animation
    6) Thread pooling

그 외에 흥미있는 것 위주로 추가 구현할 수 있습니다.

## 개발자 연락처
popoory.dev@gmail.com
