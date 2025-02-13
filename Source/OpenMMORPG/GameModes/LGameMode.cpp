


#include "LGameMode.h"
#include <Kismet/GameplayStatics.h>

void ALGameMode::OnLogin(FString Account, FString Password)
{
	UGameplayStatics::OpenLevel(this, FName("127.0.0.1"));

	GetWorld()->ServerTravel("/Game/Maps/TestMap");
}
