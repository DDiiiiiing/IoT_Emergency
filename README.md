# IoT_Emergency
 IoT camera for Emergency situation

## H/W requirements
- Arduino Uno
- DF Player Mini
- ESP 01
- 3 color LED
- DC-DC Stepdown Module (afford 12V -> 5V)
- Battery socket and battery suppling 12V (e.f. 1.5V AA battery * 8)
- Smartphone can run IV Cam
- Google Teachable Machine trained model

## Overview
 We assume PC as a Server, Arduino with Smartphone as a Remote Device. Smartphone transfers video image to server, server predicts possibility of emergency situation and signals to arduino. Arduino measures how long did the situation continue and alerts to users by LED and warning sound. After arduino signals server that situation continued more than certain time, server messages to caregiver. 
 
