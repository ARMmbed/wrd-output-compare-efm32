#import "gpio/GPIO.h"
#import "yterror/YTError.h"


@interface LeTimer : YTStatic


+(YTError)togglePin:(enum PinName)pin everyMilliseconds:(uint32_t)period;


@end




