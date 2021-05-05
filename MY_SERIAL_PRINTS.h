// #include <MY_SERIAL_PRINTS.h>
// Must #define MY_SERIAL_PRINT true or false


#if MY_SERIAL_PRINT_ON // expand into Serial.print..
  #define mySerialBegin(s) (Serial.begin(s)) //speed

  #define mySerialP(a)   (Serial.print(a))
  #define mySerialPln(a) (Serial.println(a))
  #define mySerialPfmt(datastr,fmt) (Serial.print(datastr,fmt)) // datastructure fields formatted printout

  #define mySerialPf1(fmt,a1)  (Serial.printf(fmt,a1))
  #define mySerialPf2(fmt,a1,a2)  (Serial.printf(fmt,a1,a2))
  #define mySerialPf3(fmt,a1,a2,a3)  (Serial.printf(fmt,a1,a2,a3))
  #define mySerialPf4(fmt,a1,a2,a3,a4)  (Serial.printf(fmt,a1,a2,a3,a4))
  #define mySerialPf5(fmt,a1,a2,a3,a4,a5)  (Serial.printf(fmt,a1,a2,a3,a4,a5))
  #define mySerialPf6(fmt,a1,a2,a3,a4,a5,a6)  (Serial.printf(fmt,a1,a2,a3,a4,a5,a6))
  #define mySerialPf7(fmt,a1,a2,a3,a4,a5,a6,a7)  (Serial.printf(fmt,a1,a2,a3,a4,a5,a6,a7))

#else               // delete all the Serial.print.. commands entirely
  #define mySerialBegin(s) // (Serial.begin(s)) //speed

  #define mySerialP(a)   // (Serial.print(a))   #define as blank
  #define mySerialPln(a) // (Serial.println(a)) #define as blank
  #define mySerialPfmt(datastr,fmt) // (Serial.print(datastr,fmt)) // datastructure fields formatted printout

  #define mySerialPf1(fmt,a1)  // (Serial.printf(fmt,a1))
  #define mySerialPf2(fmt,a1,a2)  // (Serial.printf(fmt,a1,a2))
  #define mySerialPf3(fmt,a1,a2,a3)  // (Serial.printf(fmt,a1,a2,a3))
  #define mySerialPf4(fmt,a1,a2,a3,a4)  // (Serial.printf(fmt,a1,a2,a3,a4))
  #define mySerialPf5(fmt,a1,a2,a3,a4,a5)  // (Serial.printf(fmt,a1,a2,a3,a4,a5))
  #define mySerialPf6(fmt,a1,a2,a3,a4,a5,a6) // (Serial.printf(fmt,a1,a2,a3,a4,a5,a6))
  #define mySerialPf7(fmt,a1,a2,a3,a4,a5,a6,a7) // (Serial.printf(fmt,a1,a2,a3,a4,a5,a6,a7))

#endif

// https://gist.github.com/ridencww/4e5d10097fee0b0f7f6b  for printf doc

// Better printf code info
// https://diyprojects.io/getting-started-arduino-functions-for-combining-formatting-variables-to-the-serial-port-esp32-esp8266-compatible/


