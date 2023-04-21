xinlian L22 project:

Due to the touch ask close the timer interrupt to avoid the PTC accquire the channals ,
it lead to the timer counter have a delay, 1s will be delayed to 1040ms. and the timer is used
to schedule the LoRA communication, the delay will lead the LoRA transmission in the same time.
the timer counter should less than 1ms per second . so, add another Timer to count the 1ms tick,
divide the MCLK 48MHz by GCLK(Divid by 47), it ouput 1021276Hz,  then the Timer prescale by 1024,
it will inc 1 per 1ms.

also XinLian complains the RTC will lose when power off, add RTC in the project and checked the issued board.
can't find the issue. and check xinlian's application again, if the board run a long time, the loss issue will 
disappeared  .