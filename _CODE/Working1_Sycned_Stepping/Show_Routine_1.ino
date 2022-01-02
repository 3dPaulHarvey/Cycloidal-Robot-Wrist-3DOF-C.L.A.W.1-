void showRoutine1 (){


    //0 J5 +up -down
  //1 j6 +CW -CCW
  //2 j4 +CW -CCW
  
  //lookup
  prepareMove(0, 500);
  prepareMove(1, 500);
  prepareMove(2, 1200);
  runAndWait();
  delay(500);

  //j6 shake
  // prepareMove(0, 500); //j5
  // prepareMove(1, 1000);//j6
  // prepareMove(2, 1200);
  //  runAndWait();
  delay(100);

  prepareMove(0, 500);
  prepareMove(1, -500);
  prepareMove(2, 1200);
  runAndWait();
  delay(100);

  prepareMove(0, 500);
  prepareMove(1, 500);
  prepareMove(2, 1200);
  runAndWait();


  //j5 shake
  prepareMove(0, 1000);
  prepareMove(1, 500);
  prepareMove(2, 1200);
  runAndWait();

  prepareMove(0, -500);
  prepareMove(1, 500);
  prepareMove(2, 1200);
  runAndWait();

  prepareMove(0, 500);
  prepareMove(1, 1000);
  prepareMove(2, 1200);
  runAndWait();


  //j4 shake
  prepareMove(0, 500);
  prepareMove(1, 1000);
  prepareMove(2, 1700);
  runAndWait();

  prepareMove(0, 500);
  prepareMove(1, -500);
  prepareMove(2, 700);
  runAndWait();

  prepareMove(0, 500);
  prepareMove(1, 1000);
  prepareMove(2, 1200);
  runAndWait();

  //all full rotate

  prepareMove(0, 0);
  prepareMove(1, 0);
  prepareMove(2, 0);
  runAndWait();

  delay(100);


  
  //show off

    prepareMove(0, 1500);
  prepareMove(1, 7000);
  prepareMove(2, -1500);
  runAndWait();
  delay(150);

 // prepareMove(0, 0);
 // prepareMove(1, 0);
  //prepareMove(2, 0);
 // runAndWait();

  prepareMove(0, jointArray[0].stepsPerRevolution);
  prepareMove(1, 4500);
  prepareMove(2, 3000);
  runAndWait();
  delay(150);

  //prepareMove(0, 0);
 // prepareMove(1, 0);
 // prepareMove(2, 0);
  //runAndWait();

  prepareMove(0, (jointArray[2].position + jointArray[0].stepsPerRevolution - 1250) );
  prepareMove(1, (jointArray[2].position + jointArray[1].stepsPerRevolution - 230)/2 );
  prepareMove(2, (jointArray[2].position + jointArray[2].stepsPerRevolution)/2 );
  runAndWait();

  prepareMove(0, 0);
  prepareMove(1, 0);
  prepareMove(2, 0);
  runAndWait();
    delay(170);

  //end show off

  //full revolution
  prepareMove(0, (jointArray[2].position + jointArray[0].stepsPerRevolution - 1250 - 20) );
  prepareMove(1, (jointArray[2].position + jointArray[1].stepsPerRevolution - 230 - 53)  );
  prepareMove(2, (jointArray[2].position + jointArray[2].stepsPerRevolution) );
  runAndWait();
  delay(100);
 // prepareMove(0, 0);
 // prepareMove(1, 0);
 // prepareMove(2, 0);
 // runAndWait();
}
