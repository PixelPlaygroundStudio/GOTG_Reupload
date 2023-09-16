#include <Wiwa/utilities/Reflection.h>
enum class GoToMilanoHub_ {hola};
enum class GoMainMenu_ { hola };
enum class Quit_ { hola };
enum class Resume_PauseGame_ { hola };
enum class ActivateOptionsCanvas_ { hola };
enum class DeActivateOptionsCanvas_ { hola };
enum class SetFullScreen_ { hola };
enum class SetVsync_ { hola };
enum class SetVolumeMusic_ { hola };
enum class SetVolumeFX_ { hola };
enum class PlaceHolderButton_ { hola };
enum class ActivateOptionsMenu_ { hola };
enum class DeActivateOptionsMenu_ { hola };
enum class GoToVideo_ { hola };

enum class AddHealthCapacitor_ { hola };
enum class AddNanooBoost_ { hola };
enum class AddEasyTrigger_ { hola };
enum class AddFancyBoots_ { hola };
enum class AddLethalShooter_ { hola };
enum class AddShieldFan_ { hola };
enum class AddNanoMachines_ { hola };
enum class AddRecoveryShield_ { hola };
enum class AddDevourer_ { hola };
enum class AddFanatic_ { hola };
enum class AddRecoveryHealth_ { hola };

enum class GoToHub_ { hola };

enum class GoToOutro_ { hola };

enum class Continue_ { hola };

// Declare
GoToMilanoHub_ GoToMilanoHub();

GoMainMenu_ GoMainMenu();

Quit_ Quit();

Resume_PauseGame_ Resume_PauseGame();

ActivateOptionsCanvas_ ActivateOptionsCanvas();

DeActivateOptionsCanvas_ DeActivateOptionsCanvas();

SetFullScreen_ SetFullScreen(bool ret);

SetVsync_ SetVsync(bool ret);

SetVolumeMusic_ SetVolumeMusic(float level);

SetVolumeFX_ SetVolumeFX(float level);

PlaceHolderButton_ PlaceHolderButton();

ActivateOptionsMenu_ ActivateOptionsMenu();

DeActivateOptionsMenu_ DeActivateOptionsMenu();

GoToVideo_ GoToIntroLoreVideo();

AddHealthCapacitor_ AddHealthCapacitor();

AddNanooBoost_ AddNanooBoost();

AddEasyTrigger_ AddEasyTrigger();

AddFancyBoots_ AddFancyBoots();

AddLethalShooter_ AddLethalShooter();

AddShieldFan_ AddShieldFan();

AddNanoMachines_ AddNanoMachines();

AddRecoveryShield_ AddRecoveryShield();

AddDevourer_ AddDevourer();

AddFanatic_ AddFanatic();

AddRecoveryHealth_ AddRecoveryHealth();

GoToHub_ GoToHub();

GoToOutro_ GoToOutro();

Continue_ Continue();

//// Register
REGISTER_FUNCTION(GoToMilanoHub);
REGISTER_FUNCTION(GoMainMenu);
REGISTER_FUNCTION(Quit);
REGISTER_FUNCTION(Resume_PauseGame);
REGISTER_FUNCTION(ActivateOptionsCanvas);
REGISTER_FUNCTION(DeActivateOptionsCanvas);
REGISTER_FUNCTION(SetFullScreen);
REGISTER_FUNCTION(SetVsync);
REGISTER_FUNCTION(SetVolumeMusic);
REGISTER_FUNCTION(SetVolumeFX);
REGISTER_FUNCTION(PlaceHolderButton);
REGISTER_FUNCTION(ActivateOptionsMenu);
REGISTER_FUNCTION(DeActivateOptionsMenu);
REGISTER_FUNCTION(GoToIntroLoreVideo);

REGISTER_FUNCTION(AddHealthCapacitor);
REGISTER_FUNCTION(AddNanooBoost);
REGISTER_FUNCTION(AddEasyTrigger);
REGISTER_FUNCTION(AddFancyBoots);
REGISTER_FUNCTION(AddLethalShooter);
REGISTER_FUNCTION(AddShieldFan);
REGISTER_FUNCTION(AddNanoMachines);
REGISTER_FUNCTION(AddRecoveryShield);
REGISTER_FUNCTION(AddDevourer);
REGISTER_FUNCTION(AddFanatic);
REGISTER_FUNCTION(AddRecoveryHealth);
REGISTER_FUNCTION(GoToHub);
REGISTER_FUNCTION(GoToOutro);
REGISTER_FUNCTION(Continue);