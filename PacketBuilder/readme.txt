기존 패킷 쳬계는 확장성이 떨어짐
	새 패킷 종류 도입 시 소스를 열어 기존 함수의 switch 부분을 수정해야 함.
	-> 오류 발생 가능성 높음

새 패킷 체계는 Machine과 loose coupling을 추구하여 응집성을 높이고 결합도를 낮춰 확장성을 높임.
	이를 위해 builder 패턴, mediator 패턴을 도입.