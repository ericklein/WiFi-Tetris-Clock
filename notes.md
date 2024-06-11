the AM/PM should drop once and not change until required?
when the AM/PM drops, does the M go first (code order)

I'm not sure how animateIntro is called except through the auto activeAnimation call, which in turns changes activeAnimation via conditional. Notice the conditional goes before the scale is set, is animateTwelveHour or animateTwentyFourHour called initially at default scale then set to scale = 2 for subsequent calls?

pretty sure animateIntro does little now that drawIntro is gone, as it has a delay that is not needed and I tweaked down to 1000 without understanding why. You could replace with?

tetris.scale = 2;
auto activeAnimation = time12HourFormat ? animateTwelveHour : animateTwentyFourHour;

delete animateIntro(bool showColon)

could you then move tetris.scale = 2 to setup(), leaving auto activeAnimation ... as a global scope definition?

the original reason I reopened this project is when the NTP server doesn't return a value and we get a clock at 12:00. I need to detect this problem after setLocation and restart the hardware, or just wait until I get a valid time? What can ezTime do for me here in terms of error handling/reporting?

what's up with drawChar spacing being +6 then switching to +5? If it was consistent, I could just pass in a string into a generic function to do this...

I noticed I took the default x=0,y=0 out of drawXXX(), they aren't used but probably a good practice. Of course, I should error check inbound values...

would be pretty straightforward to switch ezTime for esperiff time.h